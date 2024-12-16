#ifndef HUD_DYNAMIC_CROSSHAIR_H
#define HUD_DYNAMIC_CROSSHAIR_H

#include "hudelement.h"
#include <vgui_controls/Panel.h>

using namespace vgui;

class CHudDynamicCrosshair : public CHudElement, public Panel
{
    DECLARE_CLASS_SIMPLE(CHudDynamicCrosshair, Panel);

public:
    CHudDynamicCrosshair(const char* pElementName);

    void Init() override;
    void VidInit() override;
    void Paint() override;

    void UpdateCrosshair(float flFrameTime);

private:
    float m_flCrosshairGap;
    float m_flMaxGap;
    float m_flMinGap;
    float m_flExpansionRate;
    float m_flContractionRate;
    bool m_bPlayerMoving;
    bool m_bPlayerShooting;

    void DrawCrosshairLines(int centerX, int centerY);
};

#endif 