#include "cbase.h"
#include "hud_dynamic_crosshair.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "vgui/ISurface.h"
#include "vgui_controls/Panel.h"
#include "hl2/basehlcombatweapon_shared.h"

using namespace vgui;

DECLARE_HUDELEMENT(CHudDynamicCrosshair);

CHudDynamicCrosshair::CHudDynamicCrosshair(const char* pElementName)
    : CHudElement(pElementName), Panel(NULL, "HudDynamicCrosshair")
{
    SetParent(g_pClientMode->GetViewport());
    m_flCrosshairGap = 10.0f;
    m_flMaxGap = 40.0f;
    m_flMinGap = 10.0f;
    m_flExpansionRate = 50.0f;
    m_flContractionRate = 30.0f;
    m_bPlayerMoving = false;
    m_bPlayerShooting = false;

    SetHiddenBits(HIDEHUD_CROSSHAIR | HIDEHUD_PLAYERDEAD);
}

void CHudDynamicCrosshair::Init()
{
    m_flCrosshairGap = m_flMinGap;
}

void CHudDynamicCrosshair::VidInit()
{
    Init();
}

void CHudDynamicCrosshair::Paint()
{
    int screenWidth, screenHeight;
    surface()->GetScreenSize(screenWidth, screenHeight);
    SetBounds(0, 0, screenWidth, screenHeight);

    int xCenter = screenWidth / 2;
    int yCenter = screenHeight / 2;

    UpdateCrosshair(gpGlobals->frametime);

    DrawCrosshairLines(xCenter, yCenter);
}

void CHudDynamicCrosshair::UpdateCrosshair(float flFrameTime)
{
    C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
    if (!pPlayer)
        return;

    m_bPlayerMoving = pPlayer->GetAbsVelocity().Length2D() > 10.0f;

    if (pPlayer->HasFiredWeapon())
    {
        m_bPlayerShooting = true;
    }
    else
    {
        m_bPlayerShooting = false;
    }

    if (m_bPlayerMoving || m_bPlayerShooting)
    {
        m_flCrosshairGap = min(m_flCrosshairGap + m_flExpansionRate * flFrameTime, m_flMaxGap);
    }
    else
    {
        m_flCrosshairGap = max(m_flCrosshairGap - m_flContractionRate * flFrameTime, m_flMinGap);
    }
}

void CHudDynamicCrosshair::DrawCrosshairLines(int centerX, int centerY)
{
    int lineLength = 10;

    Color crosshairColor(255, 255, 255, 200);

    surface()->DrawSetColor(crosshairColor);
    surface()->DrawLine(centerX - m_flCrosshairGap - lineLength, centerY, centerX - m_flCrosshairGap, centerY);
    surface()->DrawLine(centerX + m_flCrosshairGap, centerY, centerX + m_flCrosshairGap + lineLength, centerY);

    surface()->DrawLine(centerX, centerY - m_flCrosshairGap - lineLength, centerX, centerY - m_flCrosshairGap);
    surface()->DrawLine(centerX, centerY + m_flCrosshairGap, centerX, centerY + m_flCrosshairGap + lineLength);
}