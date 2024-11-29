#ifndef HUD_DYNAMIC_CROSSHAIR_H
#define HUD_DYNAMIC_CROSSHAIR_H
// soon this will be moved to its own element instead of quickinfo so i'm preparing.
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
	void UpdateSpread(float weaponSpread);

	int m_flShootingEndTime;
	bool m_bIsShooting;

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

#endif // HUD_DYNAMIC_CROSSHAIR_H
