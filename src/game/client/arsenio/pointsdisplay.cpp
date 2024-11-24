#include "cbase.h"
#include "vgui/IVGui.h"
#include "vgui_controls/Panel.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/Frame.h"
#include "vgui/ISurface.h"
#include "tier1/KeyValues.h"
#include "vgui/IScheme.h"
#include "arsenio/level_system.h"
#include "filesystem.h"
#include "arsenio/CoinManager.cpp"
#include "vgui/IInput.h"
#include "tier0/memdbgon.h"

using namespace vgui;

class TitleCardScreen : public Frame
{
public:
    TitleCardScreen() : Frame(nullptr, "TitleCardScreen")
    {
        SetTitle("Choose Your Adventure", true);
        SetSize(1920, 1090);
        SetDeleteSelfOnClose(false);
        SetSizeable(false);
        SetVisible(true);

        m_pCard1 = new Panel(this, "Card1");
        m_pCard1->SetBounds(960 - 75, 545 - 100, 150, 200);
        m_pCard1->SetBgColor(Color(80, 80, 80, 255));

        m_pCard1Label = new Label(m_pCard1, "Card1Label", "Arsenio 2044");
        m_pCard1Label->SetBounds(10, 10, 130, 20);

        m_pCard1Description = new Label(this, "Card1Description", "Play in the strange year of 2044.");
        m_pCard1Description->SetBounds(960 - 150, 745, 300, 20);
        m_pCard1Description->SetVisible(false);

        m_pCard1Button = new Button(this, "Card1Button", "PLAY", this, "play_card1");
        m_pCard1Button->SetBounds(960 - 50, 775, 100, 30);
        m_pCard1Button->SetVisible(false);

        m_pCard2 = new Panel(this, "Card2");
        m_pCard2->SetBounds(960 + 75, 545 - 100, 150, 200);
        m_pCard2->SetBgColor(Color(80, 80, 80, 255));

        m_pCard2Label = new Label(m_pCard2, "Card2Label", "Gamemodes");
        m_pCard2Label->SetBounds(10, 10, 130, 20);

        m_pCard2Description = new Label(this, "Card2Description", "Play fun gamemodes!");
        m_pCard2Description->SetBounds(960 + 50, 745, 300, 20);
        m_pCard2Description->SetVisible(false);

        m_pCard2Button = new Button(this, "Card2Button", "PLAY", this, "play_card2");
        m_pCard2Button->SetBounds(960 + 100, 775, 100, 30);
        m_pCard2Button->SetVisible(false);

        ivgui()->AddTickSignal(GetVPanel());
    }

    void OnThink() override
    {
        int mx, my;
        input()->GetCursorPos(mx, my);
        ScreenToLocal(mx, my);

        HandleCardHover(mx, my, m_pCard1, m_pCard1Description, m_pCard1Button);
        HandleCardHover(mx, my, m_pCard2, m_pCard2Description, m_pCard2Button);
    }

    void OnCommand(const char* command) override
    {
        if (FStrEq(command, "play_card1"))
        {
            engine->ClientCmd("disconnect");
        }
        else if (FStrEq(command, "play_card2"))
        {
            engine->ClientCmd("map gamemodes");
        }
        else
        {
            Frame::OnCommand(command);
        }
    }

private:
    Panel* m_pCard1;
    Label* m_pCard1Label;
    Label* m_pCard1Description;
    Button* m_pCard1Button;

    Panel* m_pCard2;
    Label* m_pCard2Label;
    Label* m_pCard2Description;
    Button* m_pCard2Button;

    void HandleCardHover(int mx, int my, Panel* card, Label* description, Button* button)
    {
        bool isHovered = card->IsWithin(mx, my);

        card->SetSize(isHovered ? 200 : 150, isHovered ? 250 : 200);
        description->SetVisible(isHovered);
        button->SetVisible(isHovered);
    }
};

static TitleCardScreen* g_pTitleCardScreen = nullptr;

void ToggleTitleCardScreen()
{
    if (g_pTitleCardScreen)
    {
        if (g_pTitleCardScreen->IsVisible())
        {
            g_pTitleCardScreen->SetVisible(false);
        }
        else
        {
            g_pTitleCardScreen->SetVisible(true);
        }
    }
    else
    {
        g_pTitleCardScreen = new TitleCardScreen();
        g_pTitleCardScreen->MakePopup();
    }
}

CON_COMMAND(toggle_title_card, "Toggles the title card screen.")
{
    ToggleTitleCardScreen();
}

#ifdef ARSENIO_OLD_POINTS

class CPointsDisplayPanel : public vgui::Panel
{
public:
    CPointsDisplayPanel(vgui::Panel* pParent) : vgui::Panel(pParent, "PointsDisplay")
    {
        m_iPoints = 0;
        SetSize(200, 50);
        SetBgColor(Color(0, 0, 0, 200));
    }

    void Paint() override
    {

        wchar_t szPoints[64];
        swprintf(szPoints, ARRAYSIZE(szPoints), L"Points: %d", m_iPoints);

        surface()->DrawSetTextFont(m_hFont);
        surface()->DrawSetTextColor(255, 255, 255, 255);
        surface()->DrawSetTextPos(10, 10);
        surface()->DrawPrintText(szPoints, wcslen(szPoints));
    }

    void SetPoints(int points)
    {
        m_iPoints = points;
        InvalidateLayout();
    }

private:
    int m_iPoints;
    vgui::HFont m_hFont;

    void ApplySchemeSettings(vgui::IScheme* pScheme) override
    {
        BaseClass::ApplySchemeSettings(pScheme);
        m_hFont = pScheme->GetFont("Default", true);
    }
};

class CPointsDisplay : public CAutoGameSystemPerFrame
{
public:
    CPointsDisplay() : m_pPointsDisplayPanel(nullptr)
    {
        gameeventmanager->AddListener(this, "update_points_display", false);
    }

    ~CPointsDisplay()
    {
        gameeventmanager->RemoveListener(this);
    }

    void FireGameEvent(IGameEvent* event) override
    {
        if (FStrEq(event->GetName(), "update_points_display"))
        {
            int points = event->GetInt("points");
            if (m_pPointsDisplayPanel)
            {
                m_pPointsDisplayPanel->SetPoints(points);
            }
        }
    }

    void CreatePointsDisplayPanel()
    {
        if (!m_pPointsDisplayPanel)
        {
            m_pPointsDisplayPanel = new CPointsDisplayPanel(g_pClientMode->GetViewport(), "PointsDisplay");
        }
    }

private:
    CPointsDisplayPanel* m_pPointsDisplayPanel;
};

static CPointsDisplay g_PointsDisplay;

CON_COMMAND(point_display_init, "Initialize Points Display")
{
    g_PointsDisplay.CreatePointsDisplayPanel();
}
#endif