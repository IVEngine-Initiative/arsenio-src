#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "iclientmode.h"
#include "vgui_controls/Panel.h"
#include "vgui/ISurface.h"
#include "view.h"
#include "basecombatweapon_shared.h"
#include "mathlib/mathlib.h"
#include "hud_quickinfo.h"
#include "tier0/memdbgon.h"

using namespace vgui;

static ConVar dyncrosshair_spread("dyncrosshair_spread", "10.0", FCVAR_CLIENTDLL, "Dynamic crosshair spread value during shooting.");

CHUDDynamicCrosshair::CHUDDynamicCrosshair(const char* pElementName)
	: CHudElement(pElementName), BaseClass(nullptr, "HUDDynamicCrosshair")
{
	vgui::Panel* pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	SetHiddenBits(HIDEHUD_CROSSHAIR);

	m_nCrosshairSize = 15;
	m_nMaxSpreadSize = 80;
	m_flSpreadScale = 0.1f;
	m_clrCrosshair = Color(255, 255, 255, 255);
	m_pTexture = nullptr;

	m_flCurrentSpread = 0.0f;
	m_bIsShooting = false;
	m_flShootingEndTime = 0.0f;
}

void CHUDDynamicCrosshair::VidInit()
{
	m_pTexture = gHUD.GetIcon("crosshair");
	m_flCurrentSpread = 0.0f;
	m_vecLastVelocity = Vector(0, 0, 0);
}

bool CHUDDynamicCrosshair::ShouldDraw()
{
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
		return false;

	return CHudElement::ShouldDraw();
}

void CHUDDynamicCrosshair::Paint()
{
	int screenWidth, screenHeight;
	surface()->GetScreenSize(screenWidth, screenHeight);
	SetBounds(0, 0, screenWidth, screenHeight);

	int xCenter = screenWidth / 2;
	int yCenter = screenHeight / 2;

	CalculateCrosshairSpread();

	DrawDynamicCrosshair(xCenter, yCenter);
}

void CHUDDynamicCrosshair::DrawDynamicCrosshair(int xCenter, int yCenter)
{
	int spreadOffset = static_cast<int>(m_flCurrentSpread);

	surface()->DrawSetColor(m_clrCrosshair);


	surface()->DrawLine(xCenter, yCenter - m_nCrosshairSize - spreadOffset, xCenter, yCenter - spreadOffset);

	surface()->DrawLine(xCenter, yCenter + spreadOffset, xCenter, yCenter + m_nCrosshairSize + spreadOffset);

	surface()->DrawLine(xCenter - m_nCrosshairSize - spreadOffset, yCenter, xCenter - spreadOffset, yCenter);

	surface()->DrawLine(xCenter + spreadOffset, yCenter, xCenter + m_nCrosshairSize + spreadOffset, yCenter);
}

void CHUDDynamicCrosshair::CalculateCrosshairSpread()
{
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
		return;

	Vector velocity = pPlayer->GetAbsVelocity();
	float speed = velocity.Length();

	float baseSpread = speed * m_flSpreadScale;

	float weaponSpread = 0.0f;


	if (m_bIsShooting)
	{
		weaponSpread = dyncrosshair_spread.GetFloat();
	}
	else
	{
		C_BaseCombatWeapon* pWeapon = pPlayer->GetActiveWeapon();
		if (pWeapon)
		{
			Vector bulletSpread = pWeapon->GetBulletSpread();
			weaponSpread = bulletSpread.Length();
			weaponSpread *= 10.0f;
		}
	}

	m_flCurrentSpread = clamp(baseSpread + weaponSpread, 0.2f, (float)m_nMaxSpreadSize);


	if (m_bIsShooting && gpGlobals->curtime >= m_flShootingEndTime)
	{
		m_bIsShooting = false;
	}
}

void CHUDDynamicCrosshair::UpdateSpread(float weaponSpread)
{
	m_flCurrentSpread = clamp(weaponSpread, 0.2f, (float)m_nMaxSpreadSize);
}


void DyncrosshairShoot()
{
	CHUDDynamicCrosshair* pCrosshair = dynamic_cast<CHUDDynamicCrosshair*>(gHUD.FindElement("HUDDynamicCrosshair"));
	if (!pCrosshair)
		return;

	pCrosshair->m_bIsShooting = true;
	pCrosshair->m_flShootingEndTime = gpGlobals->curtime + 0.5f;
}

static ConCommand dyncrosshair_shoot("dyncrosshair_shoot", DyncrosshairShoot, "Triggers dynamic crosshair shooting effect for 0.5 seconds.");
