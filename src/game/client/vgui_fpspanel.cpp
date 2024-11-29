//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Framerate indicator panel overhaul
//
//=====================================================================================//
#include "cbase.h"
#include "ifpspanel.h"
#include "vgui/IVGui.h"
#include "vgui/ISurface.h"
#include "vgui_controls/Panel.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "view.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar cl_showfps("cl_showfps", "0", FCVAR_ARCHIVE, "Draw FPS meter at top of screen");
static ConVar cl_showpos("cl_showpos", "0", FCVAR_ARCHIVE, "Draw current position at top of screen");

extern bool g_bDisplayParticlePerformance;
int GetParticlePerformance();

//-----------------------------------------------------------------------------
// Purpose: Framerate indicator panel with a modern UI
//-----------------------------------------------------------------------------
class CFPSPanel : public vgui::Panel {
    DECLARE_CLASS_SIMPLE(CFPSPanel, vgui::Panel);
public:
    CFPSPanel(vgui::VPANEL parent);
    virtual ~CFPSPanel(void);
    virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
    virtual void Paint();
    virtual void OnTick(void);
    virtual bool ShouldDraw(void);

protected:
    MESSAGE_FUNC_INT_INT(OnScreenSizeChanged, "OnScreenSizeChanged", oldwide, oldtall);

private:
    void ComputeSize(void);
    void UpdateFPS(float flFPS);

    vgui::HFont m_hFont;
    float m_flFPS;
    char m_szWatermark[256];
    Color m_fpsColor;
    int m_iFrameCount;
    float m_flLastFrameTime;
};

#define FPS_PANEL_WIDTH 400

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CFPSPanel::CFPSPanel(vgui::VPANEL parent) : BaseClass(NULL, "CFPSPanel"), m_hFont(0), m_flFPS(-1.0), m_iFrameCount(0), m_flLastFrameTime(0.0f)
{
    SetParent(parent);
    SetVisible(false);
    SetCursor(NULL);

    SetFgColor(Color(255, 255, 255, 255));
    SetPaintBackgroundEnabled(false);

    ComputeSize();
    g_pVGui->AddTickSignal(GetVPanel(), 250); // Update every 250ms

    Q_snprintf(m_szWatermark, sizeof(m_szWatermark), "%s %u.%u.%u (%s %s)", GAME_NAME, GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_PATCH, __DATE__, __TIME__);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CFPSPanel::~CFPSPanel(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: Updates panel on screen size change
//-----------------------------------------------------------------------------
void CFPSPanel::OnScreenSizeChanged(int iOldWide, int iOldTall)
{
    BaseClass::OnScreenSizeChanged(iOldWide, iOldTall);
    ComputeSize();
}

//-----------------------------------------------------------------------------
// Purpose: Calculates panel size and position based on screen size
//-----------------------------------------------------------------------------
void CFPSPanel::ComputeSize(void)
{
    int wide, tall;
    g_pVGuiPanel->GetSize(GetVParent(), wide, tall);

    // Positioning at top-right corner
    int x = wide - FPS_PANEL_WIDTH;
    int y = 0;
    SetPos(x, y);
    SetSize(FPS_PANEL_WIDTH, 6 * g_pVGuiSurface->GetFontTall(m_hFont) + 12);
}

//-----------------------------------------------------------------------------
// Purpose: Sets the font and applies the theme settings
//-----------------------------------------------------------------------------
void CFPSPanel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);

    m_hFont = pScheme->GetFont("DefaultFixedOutline");
    Assert(m_hFont);

    ComputeSize();
}

//-----------------------------------------------------------------------------
// Purpose: Updates the FPS and color dynamically
//-----------------------------------------------------------------------------
void CFPSPanel::UpdateFPS(float flFPS)
{
    if (flFPS >= 100) {
        m_fpsColor = Color(0, 255, 255); // Cyan for high FPS
    }
    else if (flFPS >= 50) {
        m_fpsColor = Color(0, 255, 0); // Green for medium FPS
    }
    else if (flFPS >= 30) {
        m_fpsColor = Color(255, 255, 0); // Yellow for lower FPS
    }
    else {
        m_fpsColor = Color(255, 0, 0); // Red for very low FPS
    }

    // Smooth FPS calculation (smoothing over multiple frames)
    if (m_flFPS < 0) {
        m_flFPS = flFPS;
    }
    else {
        m_flFPS = m_flFPS * 0.9f + flFPS * 0.1f;
    }
}

//-----------------------------------------------------------------------------
// Purpose: Called every tick to check if the panel should be visible
//-----------------------------------------------------------------------------
void CFPSPanel::OnTick(void)
{
    bool bVisible = ShouldDraw();
    if (IsVisible() != bVisible)
        SetVisible(bVisible);
}

//-----------------------------------------------------------------------------
// Purpose: Determines if the FPS panel should be shown
//-----------------------------------------------------------------------------
bool CFPSPanel::ShouldDraw(void)
{
    return cl_showfps.GetBool() || cl_showpos.GetBool() || g_bDisplayParticlePerformance;
}

//-----------------------------------------------------------------------------
// Purpose: Paints the FPS and position information on screen
//-----------------------------------------------------------------------------
void CFPSPanel::Paint()
{
    int ypos = 2;
    int iFontTall = g_pVGuiSurface->GetFontTall(m_hFont);

    // Draw watermark/game version
    g_pMatSystemSurface->DrawColoredText(m_hFont, 2, ypos, 255, 255, 255, 255, "%s %u.%u.%u (%s %s)", GAME_NAME, GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_PATCH, __DATE__, __TIME__);
    ypos += 2 + iFontTall;

    // Draw FPS with dynamic color
    if (cl_showfps.GetBool()) {
        float flFrametime = gpGlobals->absoluteframetime;
        float flFPS = (1.0f / flFrametime);
        UpdateFPS(flFPS);

        g_pMatSystemSurface->DrawColoredText(m_hFont, 2, ypos, m_fpsColor.r(), m_fpsColor.g(), m_fpsColor.b(), 255,
            "%.02f fps (%.02fms) at %s", m_flFPS, flFrametime * 1000.0, engine->GetLevelName());
        ypos += 2 + iFontTall;
    }

    // Draw position, angles, and speed if enabled
    int cl_showpos_int = cl_showpos.GetInt();
    if (cl_showpos_int > 0) {
        C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
        Vector origin = MainViewOrigin();
        QAngle angles = MainViewAngles();
        Vector velocity = Vector(0);
        if (pPlayer != NULL) {
            if (cl_showpos_int >= 2) {
                origin = pPlayer->GetLocalOrigin();
                angles = pPlayer->GetLocalAngles();
            }
            velocity = pPlayer->GetAbsVelocity();
        }

        // Draw position
        g_pMatSystemSurface->DrawColoredText(m_hFont, 2, ypos, 255, 255, 255, 255, "pos: %.02f %.02f %.02f", origin.x, origin.y, origin.z);
        ypos += 2 + iFontTall;

        // Draw rotation
        g_pMatSystemSurface->DrawColoredText(m_hFont, 2, ypos, 255, 255, 255, 255, "ang: %.02f %.02f %.02f", angles.x, angles.y, angles.z);
        ypos += 2 + iFontTall;

        // Draw speed
        g_pMatSystemSurface->DrawColoredText(m_hFont, 2, ypos, 255, 255, 255, 255, "spd: %.04f", velocity.Length());
        ypos += 2 + iFontTall;
    }
}

//-----------------------------------------------------------------------------
// Purpose: Creates and destroys the FPS panel
//-----------------------------------------------------------------------------
class CFPS : public IFPSPanel {
private:
    CFPSPanel* fpsPanel;
public:
    CFPS(void) { fpsPanel = NULL; }

    void Create(vgui::VPANEL parent) {
        fpsPanel = new CFPSPanel(parent);
    }

    void Destroy(void) {
        if (fpsPanel) {
            fpsPanel->SetParent((vgui::Panel*)NULL);
            delete fpsPanel;
            fpsPanel = NULL;
        }
    }
};

static CFPS g_FPSPanel;
IFPSPanel* fps = (IFPSPanel*)&g_FPSPanel;
