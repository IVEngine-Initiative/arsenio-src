#include "cbase.h"
#include "CustomConsole.h"
#include "vgui/IInput.h"
#include "vgui/ISurface.h"
#include "vgui/KeyCode.h"
#include "ienginevgui.h"
#include "vgui_controls/Label.h" 

#include "tier0/memdbgon.h"

using namespace vgui;

CGameConsoleDialog::CGameConsoleDialog() : BaseClass(NULL, "GameConsole", false)
{
    AddActionSignalTarget(this);
    SetScheme("TFOConsole");
    SetVisible(false);

    SetMinimizeButtonVisible(false);
    SetMaximizeButtonVisible(false);
    SetCloseButtonVisible(false);
    SetSizeable(false);
    SetMoveable(false);

    SetTitle("ARSENIO 2044 CONSOLE", true);
    bShouldShow = false;
    bShouldClose = false;

    SetSize(ScreenWidth(), 512);
    SetPos(0, -512);

    PerformLayout();

    InvalidateLayout();

    m_flLerp = 0.0f;

    m_pEngineVersionLabel = new Label(this, "EngineVersionLabel", "IVEngine 3.14 (Build 5763)");
    m_pEngineVersionLabel->SetFgColor(Color(255, 255, 255, 255));

    m_pEngineVersionLabel->SetPos(GetWide() - 270, 2);
    m_pEngineVersionLabel->SetSize(500, 80);
}

void CGameConsoleDialog::OnCommandSubmitted(const char* pCommand)
{

    engine->ClientCmd_Unrestricted(pCommand);
}

void CGameConsoleDialog::OnThink()
{
    int x, y;
    GetPos(x, y);

    if (bShouldClose || bShouldShow)
    {
        float flMilli = MAX(0.0f, 1000.0f - m_flLerp);
        float flSec = flMilli * 0.001f;
        if ((flSec > 0.5))
        {
            SetPos(x, bShouldShow ? 0 : -512);

            if (bShouldClose)
            {
                bShouldClose = false;
                engine->ClientCmd_Unrestricted("gameui_allowescapetoshow\n");
                SetVisible(false);
            }

            if (bShouldShow)
                bShouldShow = false;
        }
        else
        {
            float flFrac = SimpleSpline(flSec / 0.5);
            SetPos(x, (bShouldShow ? (1.0f - flFrac) : flFrac) * -512);
        }
    }

    float frame_msec = 1000.0f * gpGlobals->frametime;

    if (m_flLerp > 0)
    {
        m_flLerp -= frame_msec;
        if (m_flLerp < 0)
            m_flLerp = 0;
    }

    BaseClass::OnThink();

}

void CGameConsoleDialog::ToggleConsole(bool bVisibile, bool bForceOff)
{

    if (bForceOff)
    {
        SetSize(ScreenWidth(), 512);
        bShouldShow = false;
        bShouldClose = false;
        SetPos(0, -512);
        engine->ClientCmd_Unrestricted("gameui_allowescapetoshow\n");
        SetVisible(false);

        if (m_pEngineVersionLabel)
        {
            m_pEngineVersionLabel->SetVisible(false);
        }
        return;
    }

    if (bShouldClose || bShouldShow)
        return;

    SetKeyBoardInputEnabled(bVisibile);
    SetMouseInputEnabled(bVisibile);

    SetSize(ScreenWidth(), 512);
    bShouldShow = false;
    bShouldClose = false;

    vgui::surface()->PlaySound("common/console.wav");
    m_flLerp = 1000.0f;
    InvalidateLayout(false, true);

    if (bVisibile)
    {
        SetVisible(true);
        Activate();
        RequestFocus();
        bShouldShow = true;

        if (m_pEngineVersionLabel)
        {
            m_pEngineVersionLabel->SetVisible(true);
        }

        engine->ClientCmd_Unrestricted("gameui_preventescapetoshow\n");
    }
    else
    {
        bShouldClose = true;

        if (m_pEngineVersionLabel)
        {
            m_pEngineVersionLabel->SetVisible(false);
        }
    }
}

void CGameConsoleDialog::ApplySchemeSettings(vgui::IScheme* pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);

    SetBorder(NULL);
    SetPaintBorderEnabled(false);
}

void CGameConsoleDialog::OnKeyCodeTyped(vgui::KeyCode code)
{
    if (code == KEY_ESCAPE)
    {
        ToggleConsole(false);
    }
    else
    {
        BaseClass::OnKeyCodeTyped(code);
    }
}

void CGameConsoleDialog::PaintBackground()
{
    SetFgColor(Color(20, 20, 20, 240));
    SetBgColor(Color(20, 16, 16, 230));
    SetPaintBackgroundType(0);
    SetPaintBackgroundEnabled(true);
    BaseClass::PaintBackground();
}

CGameConsoleDialog* CGameConsoleDialog::s_pInstance = nullptr;

CGameConsoleDialog* CGameConsoleDialog::GetInstance()
{
    if (s_pInstance == nullptr)
    {

        s_pInstance = new CGameConsoleDialog();
    }
    return s_pInstance;
}

void ToggleConsoleCommand()
{
    if (CGameConsoleDialog* pConsole = CGameConsoleDialog::GetInstance())
    {

        pConsole->ToggleConsole(!pConsole->IsVisible(), false);
    }
}

ConCommand toggleconsole("toggleconsole", ToggleConsoleCommand, "Toggles the custom game console.");