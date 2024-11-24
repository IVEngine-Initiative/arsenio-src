#include "cbase.h"
#include "vgui_controls/Panel.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ProgressBar.h"
#include "vgui_controls/ComboBox.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui/IScheme.h"
#include "vgui/IVgui.h"
#include "arsenio/level_system.h"
#include "filesystem.h"
#include "arsenio/CoinManager.cpp"
#include "tier0/memdbgon.h"

using namespace vgui;

struct ShopItem {
    const char* name;
    const char* description;
    int price;
};

class CCustomMenu : public Panel
{
    DECLARE_CLASS(CCustomMenu, Panel);

public:
    CCustomMenu(Panel* parent);
    ~CCustomMenu();

    void ApplySchemeSettings(IScheme* scheme) override;
    void OnThink() override;
    Panel* m_pMapSelectionPanel;
private:
    Button* m_pPlayButton;
    Button* m_pSelectMapButton;
    Button* m_pExitButton;
    Label* m_pLevelText;
    Label* m_pXPText;
    ImagePanel* m_pZombieTitleImage;
    ProgressBar* m_pXPProgressBar;
    const char* currentMap = nullptr;

    Button* m_pPlayButton2;
    Button* m_pShopButton;
    Label* m_pCoinLabel;

    Label* m_pItemTitle[3];
    Label* m_pItemDescription[3];
    Button* m_pPurchaseButton[3];
    int m_Price[3];

    CoinManager m_CoinManager;
    const char* m_pPurchasedItems[3];

    ShopItem m_ShopItems[3];

    Label* m_pZombieTitle;
    Label* m_pZombieDescription;

    int m_Level;
    int m_XP;
    int m_XPRequired;

    void OnPlayPressed();
    void OnSelectMapPressed();
    void OnExitPressed();
    void OpenMapSelectionMenu();
    void ConfirmMapSelection();
    void UpdateLevelDisplay();
    void UpdateShopUI();

    void OnShopPressed();
    void OnPurchasePressed(int itemIndex);

    bool IsShopActive() const { return m_bShopActive; }
    bool m_bShopActive = false;

    virtual void OnCommand(const char* command);

    void SetupButtons();
    void SetupMapSelectionMenu();
};

CCustomMenu::CCustomMenu(Panel* parent) : Panel(parent, "CustomMenu"), m_Level(1), m_XP(0), m_XPRequired(42)
{
    SetSize(1920, 1080);

    for (int i = 0; i < 3; ++i) {
        m_pItemTitle[i] = nullptr;
        m_pItemDescription[i] = nullptr;
        m_pPurchaseButton[i] = nullptr;
        m_Price[i] = 0;
    }

    m_ShopItems[0] = { "RAINING XP", "You get 15% more XP from zombies.", 10 };
    m_ShopItems[1] = { "BOMBS AWAY!", "Call in a drone strike (one per match).", 20 };
    m_ShopItems[2] = { "Parkour", "Allows you to use the exosuit in matches.", 30 };

    m_CoinManager.LoadCoins();

    m_pZombieTitleImage = new ImagePanel(this, "ZombieTitleImage");
    m_pZombieTitleImage->SetImage("logo/logo2");
    m_pZombieTitleImage->SetPos(GetWide() / 2 - 300, 50);
    m_pZombieTitleImage->SetSize(1024, 1024);

    SetupButtons();
    SetupMapSelectionMenu();
    LoadLevelData(m_Level, m_XP);
    UpdateLevelDisplay();
    UpdateShopUI();
}

CCustomMenu::~CCustomMenu()
{
}

void CCustomMenu::ApplySchemeSettings(IScheme* scheme)
{
    BaseClass::ApplySchemeSettings(scheme);

    DevMsg("CCustomMenu::ApplySchemeSettings called\n");

    SetBgColor(Color(0, 0, 0, 0));

    HFont titleFont = scheme->GetFont("DefaultLarge");
    HFont buttonFont = scheme->GetFont("DefaultSmall");

    if (titleFont)
    {
        DevMsg("Successfully retrieved 'CustomFont'\n");
    }
    else
    {
        DevMsg("Failed to retrieve 'CustomFont'\n");
    }

    if (buttonFont)
    {
        DevMsg("Successfully retrieved 'ClientTitleFont'\n");
    }
    else
    {
        DevMsg("Failed to retrieve 'ClientTitleFont'\n");
    }

    if (m_pCoinLabel)
    {
        m_pCoinLabel->SetFont(titleFont);
        m_pCoinLabel->SetFgColor(Color(255, 255, 0, 255));
        DevMsg("Applied titleFont to m_pCoinLabel\n");
    }

    if (m_pLevelText)
    {
        m_pLevelText->SetFont(titleFont);
        m_pLevelText->SetFgColor(Color(255, 255, 255, 255));
        DevMsg("Applied titleFont to m_pLevelText\n");
    }

    if (m_pXPText)
    {
        m_pXPText->SetFont(titleFont);
        m_pXPText->SetFgColor(Color(255, 255, 255, 255));
        DevMsg("Applied titleFont to m_pXPText\n");
    }

    if (m_pPlayButton)
    {
        m_pPlayButton->SetFont(buttonFont);
        DevMsg("Applied buttonFont to m_pPlayButton\n");
    }

    if (m_pShopButton)
    {
        m_pShopButton->SetFont(buttonFont);
        DevMsg("Applied buttonFont to m_pShopButton\n");
    }

    if (m_pSelectMapButton)
    {
        m_pSelectMapButton->SetFont(buttonFont);
        DevMsg("Applied buttonFont to m_pSelectMapButton\n");
    }

    if (m_pExitButton)
    {
        m_pExitButton->SetFont(buttonFont);
        DevMsg("Applied buttonFont to m_pExitButton\n");
    }
}

void CCustomMenu::OnThink()
{
    BaseClass::OnThink();
}

void CCustomMenu::SetupButtons()
{

    m_pPlayButton2 = new Button(this, "PlayButton2", "PLAY");
    m_pPlayButton2->SetPos(100, GetTall() - 100);
    m_pPlayButton2->SetSize(200, 40);
    m_pPlayButton2->AddActionSignalTarget(this);
    m_pPlayButton2->SetCommand("Play2");

    m_pShopButton = new Button(this, "ShopButton", "SHOP");
    m_pShopButton->SetPos(300, GetTall() - 100);
    m_pShopButton->SetSize(200, 40);
    m_pShopButton->AddActionSignalTarget(this);
    m_pShopButton->SetCommand("Shop");

    m_pCoinLabel = new Label(this, "CoinLabel", VarArgs("Coins: %d", m_CoinManager.GetCoins()));
    m_pCoinLabel->SetPos(GetWide() - 200, 20);
    m_pCoinLabel->SetFgColor(Color(255, 255, 0, 255));

    m_pPlayButton = new Button(this, "PlayButton", "PLAY");
    m_pPlayButton->SetPos(GetWide() / 2 - 100, GetTall() - 100);
    m_pPlayButton->SetSize(200, 40);
    m_pPlayButton->AddActionSignalTarget(this);
    m_pPlayButton->SetCommand("Play");

    m_pSelectMapButton = new Button(this, "SelectMapButton", "SELECT MAP");
    m_pSelectMapButton->SetPos(GetWide() / 2 - 100, GetTall() - 150);
    m_pSelectMapButton->SetSize(200, 40);
    m_pSelectMapButton->AddActionSignalTarget(this);
    m_pSelectMapButton->SetCommand("SelectMap");

    m_pExitButton = new Button(this, "ExitButton", "EXIT");
    m_pExitButton->SetPos(10, GetTall() - 50);
    m_pExitButton->SetSize(100, 30);
    m_pExitButton->AddActionSignalTarget(this);
    m_pExitButton->SetCommand("Exit");

    m_pLevelText = new Label(this, "LevelText", "Level: 1");
    m_pLevelText->SetPos(20, GetTall() / 2 - 50);
    m_pLevelText->SetFgColor(Color(255, 255, 255, 255));
    m_pLevelText->SetSize(200, 50);
    m_pLevelText->SetBgColor(Color(20, 20, 20, 255));

    m_pXPText = new Label(this, "XPText", "XP: 0/100");
    m_pXPText->SetPos(20, GetTall() / 2);
    m_pXPText->SetFgColor(Color(255, 255, 255, 255));
    m_pXPText->SetSize(250, 50);
    m_pXPText->SetBgColor(Color(20, 20, 20, 255));

    m_pXPProgressBar = new ProgressBar(this, "XPProgressBar");
    m_pXPProgressBar->SetPos(20, GetTall() / 2 + 50);
    m_pXPProgressBar->SetSize(400, 40);
    m_pXPProgressBar->SetBgColor(Color(20, 20, 20, 255));
}

void CCustomMenu::SetupMapSelectionMenu()
{

    m_pMapSelectionPanel = new Panel(this, "MapSelectionPanel");
    m_pMapSelectionPanel->SetSize(500, 350);
    m_pMapSelectionPanel->SetPos(GetWide() / 2 - 250, GetTall() / 2 - 175);
    m_pMapSelectionPanel->SetVisible(false);

    m_pMapSelectionPanel->SetBgColor(Color(30, 30, 30, 255));

    Label* mapSelectionTitle = new Label(m_pMapSelectionPanel, "MapSelectionTitle", "Select a Map");
    mapSelectionTitle->SetPos(20, 20);
    mapSelectionTitle->SetSize(460, 40);
    mapSelectionTitle->SetFgColor(Color(255, 255, 255, 255));

    ComboBox* mapComboBox = new ComboBox(m_pMapSelectionPanel, "MapComboBox", 5, false);
    mapComboBox->AddItem("Farm", NULL);
    mapComboBox->AddItem("Map 2", NULL);
    mapComboBox->AddItem("Map 3", NULL);
    mapComboBox->SetPos(20, 80);
    mapComboBox->SetSize(460, 40);
    mapComboBox->SetBgColor(Color(50, 50, 50, 255));
    mapComboBox->SetFgColor(Color(255, 255, 255, 255));

    Button* selectMapButton = new Button(m_pMapSelectionPanel, "SelectMapButton", "Select Map");
    selectMapButton->SetPos(20, 150);
    selectMapButton->SetSize(460, 50);
    selectMapButton->AddActionSignalTarget(this);
    selectMapButton->SetCommand("ConfirmMapSelection");

}

void CCustomMenu::ConfirmMapSelection()
{
    ComboBox* mapComboBox = dynamic_cast<ComboBox*>(m_pMapSelectionPanel->FindChildByName("MapComboBox"));
    if (mapComboBox)
    {

        int selectedIndex = mapComboBox->GetActiveItem();

        char selectedMap[256];
        mapComboBox->GetItemText(selectedIndex, selectedMap, sizeof(selectedMap));

        currentMap = selectedMap;

        DevMsg("Selected map: %s\n", currentMap);

        m_pMapSelectionPanel->SetVisible(false);
    }
}

void CCustomMenu::OnPlayPressed()
{

    ComboBox* mapComboBox = dynamic_cast<ComboBox*>(m_pMapSelectionPanel->FindChildByName("MapComboBox"));
    if (mapComboBox)
    {

        int selectedIndex = mapComboBox->GetActiveItem();

        char selectedMap[256];
        mapComboBox->GetItemText(selectedIndex, selectedMap, sizeof(selectedMap));

        currentMap = selectedMap;

        DevMsg("Selected map: %s\n", currentMap);

        SetVisible(false);

        engine->ClientCmd(VarArgs("map %s", currentMap));
    }

}

void CCustomMenu::OnSelectMapPressed()
{
    OpenMapSelectionMenu();
}

void CCustomMenu::OnExitPressed()
{
    SetVisible(false);
    engine->ClientCmd("disconnect");
}

void CCustomMenu::OpenMapSelectionMenu()
{
    m_pMapSelectionPanel->SetVisible(true);
}

void CCustomMenu::UpdateLevelDisplay()
{

    m_pLevelText->SetText(VarArgs("Level: %d", m_Level));

    m_XPRequired = 42 + (m_Level - 1) * 42;

    m_pXPText->SetText(VarArgs("XP: %d/%d", m_XP, m_XPRequired));
    float progress = static_cast<float>(m_XP) / static_cast<float>(m_XPRequired);
    m_pXPProgressBar->SetProgress(progress);
}

void CCustomMenu::UpdateShopUI()
{

    if (IsShopActive())
    {

        for (int i = 0; i < 3; ++i)
        {

            m_pItemTitle[i] = new Label(this, VarArgs("ItemTitle%d", i), m_ShopItems[i].name);
            m_pItemTitle[i]->SetPos(100, 200 + (i * 150));
            m_pItemTitle[i]->SetSize(400, 40);
            m_pItemTitle[i]->SetFgColor(Color(255, 255, 255, 255));
            m_pItemTitle[i]->SetContentAlignment(Label::a_northwest);

            m_pItemDescription[i] = new Label(this, VarArgs("ItemDescription%d", i), m_ShopItems[i].description);
            m_pItemDescription[i]->SetPos(100, 250 + (i * 150));
            m_pItemDescription[i]->SetSize(500, 80);
            m_pItemDescription[i]->SetFgColor(Color(200, 200, 200, 255));
            m_pItemDescription[i]->SetContentAlignment(Label::a_northwest);
            m_pItemDescription[i]->SetWrap(true);

            m_pPurchaseButton[i] = new Button(this, VarArgs("PurchaseButton%d", i), "Purchase");
            m_pPurchaseButton[i]->SetPos(400, 200 + (i * 150));
            m_pPurchaseButton[i]->SetSize(200, 40);
            m_pPurchaseButton[i]->SetCommand(VarArgs("Purchase%d", i));
            m_pPurchaseButton[i]->AddActionSignalTarget(this);
        }
    }
}

void CCustomMenu::OnShopPressed()
{
    m_bShopActive = !m_bShopActive;

    if (m_bShopActive)
    {
        m_pPlayButton->SetVisible(false);
        m_pSelectMapButton->SetVisible(false);
        m_pExitButton->SetVisible(false);
        m_pLevelText->SetVisible(false);
        m_pXPText->SetVisible(false);
        m_pXPProgressBar->SetVisible(false);
    }
    else
    {

        m_pPlayButton->SetVisible(true);
        m_pSelectMapButton->SetVisible(true);
        m_pExitButton->SetVisible(true);
        m_pLevelText->SetVisible(true);
        m_pXPText->SetVisible(true);
        m_pXPProgressBar->SetVisible(true);
        m_pItemTitle[3]->SetVisible(false);
        m_pItemDescription[3]->SetVisible(false);
        m_pPurchaseButton[1]->SetVisible(false);
    }

    UpdateShopUI();
}

void CCustomMenu::OnPurchasePressed(int itemIndex)
{
    if (m_CoinManager.GetCoins() >= m_Price[itemIndex])
    {
        m_CoinManager.SpendCoins(m_Price[itemIndex]);

        m_pPurchasedItems[itemIndex] = VarArgs("Item%d", itemIndex + 1);

        FileHandle_t file = filesystem->Open("Purchased.txt", "a");
        if (file != FILESYSTEM_INVALID_HANDLE)
        {

            filesystem->Write(m_pPurchasedItems[itemIndex], strlen(m_pPurchasedItems[itemIndex]), file);
            filesystem->Write("\n", 1, file);
            filesystem->Close(file);
        }

        m_pCoinLabel->SetText(VarArgs("Coins: %d", m_CoinManager.GetCoins()));
    }
    else
    {
        DevMsg("Not enough coins to purchase item %d\n", itemIndex + 1);
    }
}

void CCustomMenu::OnCommand(const char* command)
{
    if (FStrEq(command, "Shop"))
    {
        OnShopPressed();
    }
    else if (FStrEq(command, "Play2"))
    {

        m_pPlayButton->SetVisible(true);
        m_pSelectMapButton->SetVisible(true);
        m_pExitButton->SetVisible(true);
        m_pLevelText->SetVisible(true);
        m_pXPText->SetVisible(true);
        m_pXPProgressBar->SetVisible(true);

        for (int i = 0; i < 3; ++i)
        {
            if (m_pItemTitle[i])
                m_pItemTitle[i]->SetVisible(false);

            if (m_pItemDescription[i])
                m_pItemDescription[i]->SetVisible(false);

            if (m_pPurchaseButton[i])
                m_pPurchaseButton[i]->SetVisible(false);
        }
    }
    if (FStrEq(command, "Play"))
    {
        OnPlayPressed();
    }
    else if (FStrEq(command, "SelectMap"))
    {
        OnSelectMapPressed();
    }
    else if (FStrEq(command, "ConfirmMapSelection"))
    {
        ConfirmMapSelection();
    }
    else if (FStrEq(command, "Exit"))
    {
        OnExitPressed();
    }
    if (FStrEq(command, "Purchase0"))
    {
        OnPurchasePressed(0);
    }
    else if (FStrEq(command, "Purchase1"))
    {
        OnPurchasePressed(1);
    }
    else if (FStrEq(command, "Purchase2"))
    {
        OnPurchasePressed(2);
    }
    else
    {
        BaseClass::OnCommand(command);
    }
}

void CreateCustomMenu()
{
    CCustomMenu* pMenu = new CCustomMenu(nullptr);
    pMenu->SetVisible(true);
    pMenu->MakePopup();
}

ConCommand open_custom_menu("open_custom_menu", CreateCustomMenu, "Open the custom game mode menu", FCVAR_NONE);