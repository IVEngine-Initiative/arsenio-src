#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "iclientmode.h"
#include "vgui_controls/Panel.h"
#include "vgui/ISurface.h"
#include "view.h"
#include "basecombatweapon_shared.h"
#include "mathlib/mathlib.h"

#include "tier0/memdbgon.h"

using namespace vgui;

class CHUDDynamicCrosshair : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CHUDDynamicCrosshair, vgui::Panel);

public:
	CHUDDynamicCrosshair(const char* pElementName);

	virtual void VidInit();
	virtual bool ShouldDraw();
	virtual void Paint();

private:
	void CalculateCrosshairSpread();
	void DrawDynamicCrosshair(int xCenter, int yCenter);

	float m_flCurrentSpread;
	Vector m_vecLastVelocity;

	Color m_clrCrosshair;

	int m_nCrosshairSize;
	int m_nMaxSpreadSize;
	float m_flSpreadScale;

	CHudTexture* m_pTexture;
};

DECLARE_HUDELEMENT(CHUDDynamicCrosshair);

CHUDDynamicCrosshair::CHUDDynamicCrosshair(const char* pElementName)
	: CHudElement(pElementName), BaseClass(nullptr, "HUDDynamicCrosshair")
{
	vgui::Panel* pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	SetHiddenBits(HIDEHUD_CROSSHAIR);

	m_nCrosshairSize = 5;
	m_nMaxSpreadSize = 30;
	m_flSpreadScale = 0.1f;
	m_clrCrosshair = Color(255, 255, 255, 255);
	m_pTexture = nullptr;
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
	C_BaseCombatWeapon* pWeapon = pPlayer->GetActiveWeapon();
	if (pWeapon)
	{

		Vector bulletSpread = pWeapon->GetBulletSpread();

		weaponSpread = bulletSpread.Length();
		weaponSpread *= 10.0f;
	}

	m_flCurrentSpread = clamp(baseSpread + weaponSpread, 0.2f, (float)m_nMaxSpreadSize);
}