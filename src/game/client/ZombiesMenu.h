#pragma once
#include "vgui_controls/Frame.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/Panel.h"

class CCustomMenuPanel : public vgui::Frame
{
    DECLARE_CLASS_SIMPLE(CCustomMenuPanel, vgui::Frame);

public:
    CCustomMenuPanel(vgui::VPANEL parent);
    ~CCustomMenuPanel();

    void OnCommand(const char* command) override;

private:
    void OnPlayButtonPressed();
    void OnSelectMapButtonPressed();
    void OnMapSelected(const char* mapName);

    vgui::Button* m_pPlayButton;
    vgui::Button* m_pSelectMapButton;
    vgui::Button* m_pExtraButton1; // Placeholder for future buttons
    vgui::Button* m_pExtraButton2; // Placeholder for future buttons
    vgui::Label* m_pLevelTextLabel; // Placeholder for future text
    vgui::Panel* m_pMapSelectionPanel; // Panel for map selection
    char m_szSelectedMap[256]; // Holds the selected map name
};
