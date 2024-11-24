#include "cbase.h"
#include "entityinput.h"
#include "entityoutput.h"
#include <fstream>
#include <string>
#include <vector>

// Declare the Upgrade Manager entity class
class CUpgradeManager : public CBaseEntity
{
public:
    DECLARE_CLASS(CUpgradeManager, CBaseEntity);
    DECLARE_DATADESC();

    void Spawn() override;
    void Think() override;

    void LoadPurchases();
    void FindItems(const char* fileContent);
    void CheckPurchasedItems();
    void FireUpgradeOutput(const char* itemName);

private:
    std::vector<std::string> m_PurchasedItems; // Store purchased items from file
    bool m_bCheckedPurchases; // Flag to check if purchases have been checked already

    // Outputs for firing events
    COutputEvent m_OnUpgradeItem1;
    COutputEvent m_OnUpgradeItem2;
    COutputEvent m_OnUpgradeItem3;
};

BEGIN_DATADESC(CUpgradeManager)
// Outputs for each predefined item can be fired here
DEFINE_OUTPUT(m_OnUpgradeItem1, "OnUpgradeItem1"),
DEFINE_OUTPUT(m_OnUpgradeItem2, "OnUpgradeItem2"),
DEFINE_OUTPUT(m_OnUpgradeItem3, "OnUpgradeItem3"),
// Add more as needed
END_DATADESC()

LINK_ENTITY_TO_CLASS(ent_upgrademanager, CUpgradeManager);

// Spawn the entity
void CUpgradeManager::Spawn()
{
    // Initialize the entity
    Precache();
    SetThink(&CUpgradeManager::Think);
    SetNextThink(gpGlobals->curtime + 1.0f); // Run the think function every second

    m_bCheckedPurchases = false;
}

// The think function will periodically check the purchases
void CUpgradeManager::Think()
{
    if (!m_bCheckedPurchases)
    {
        LoadPurchases(); // Read the file when first triggered
        CheckPurchasedItems(); // Check the purchased items
        m_bCheckedPurchases = true;
    }

    SetNextThink(gpGlobals->curtime + 5.0f); // Check every 5 seconds or based on need
}

// Read the purchases file (purchases.txt)
void CUpgradeManager::LoadPurchases()
{
    DevMsg("Attempting to open purchases.txt...\n");

    FileHandle_t file = filesystem->Open("purchases.txt", "r");
    if (file != FILESYSTEM_INVALID_HANDLE)
    {
        DevMsg("Successfully opened purchases.txt\n");

        char buffer[2048]; // Larger buffer for file content
        int bytesRead = filesystem->Read(buffer, sizeof(buffer) - 1, file); // Read file content
        buffer[bytesRead] = '\0'; // Ensure null-termination

        DevMsg("Read %d bytes from purchases.txt\n", bytesRead);
        DevMsg("File content: %s\n", buffer);

        filesystem->Close(file);
        DevMsg("Closed purchases.txt\n");

        FindItems(buffer); // Pass the file content to the FindItems function
    }
    else
    {
        DevMsg("Error: Could not open purchases.txt\n");
    }
}



void CUpgradeManager::FindItems(const char* fileContent)
{
    // Items to look for
    const char* items[] = { "Item1", "Item2", "Item3" };

    for (const char* item : items)
    {
        if (strstr(fileContent, item)) // Check if the item exists in the file content
        {
            m_PurchasedItems.push_back(item); // Add found item to the list
            FireUpgradeOutput(item);
            DevMsg("Found %s in purchases file\n", item);
        }
    }
}

// Check if any purchased item matches predefined items
void CUpgradeManager::CheckPurchasedItems()
{
    for (const auto& item : m_PurchasedItems)
    {
        FireUpgradeOutput(item.c_str()); // Fire the output for each matched item
    }
}

// Fire the corresponding output for each matched item
void CUpgradeManager::FireUpgradeOutput(const char* itemName)
{
    DevMsg("FireUpgradeOutput: Attempting to fire output for %s\n", itemName);

    if (FStrEq(itemName, "Item1"))
    {
        m_OnUpgradeItem1.FireOutput(this, this);
        DevMsg("FireUpgradeOutput: Fired OnUpgradeItem1 output\n");
    }
    else if (FStrEq(itemName, "Item2"))
    {
        m_OnUpgradeItem2.FireOutput(this, this);
        DevMsg("FireUpgradeOutput: Fired OnUpgradeItem2 output\n");
    }
    else if (FStrEq(itemName, "Item3"))
    {
        m_OnUpgradeItem3.FireOutput(this, this);
        DevMsg("FireUpgradeOutput: Fired OnUpgradeItem3 output\n");
    }
    else
    {
        DevMsg("FireUpgradeOutput: No matching output found for %s\n", itemName);
    }
}