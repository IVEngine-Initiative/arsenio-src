#include "cbase.h"
#include "filesystem.h"
#include "tier0/memdbgon.h"

class CoinManager
{
public:
    CoinManager() : m_Coins(0) {}

    void LoadCoins()
    {
        FileHandle_t file = filesystem->Open("coins.txt", "r");
        if (file != FILESYSTEM_INVALID_HANDLE)
        {
            char buffer[128];
            filesystem->Read(buffer, sizeof(buffer), file);
            m_Coins = atoi(buffer);
            filesystem->Close(file);
        }
    }

    void SaveCoins()
    {
        FileHandle_t file = filesystem->Open("coins.txt", "w");
        if (file != FILESYSTEM_INVALID_HANDLE)
        {
            filesystem->Write(VarArgs("%d", m_Coins), strlen(VarArgs("%d", m_Coins)), file);
            filesystem->Close(file);
        }
    }

    void AddCoins(int amount)
    {
        m_Coins += amount;
        SaveCoins();
    }

    void SpendCoins(int amount)
    {
        if (m_Coins >= amount)
        {
            m_Coins -= amount;
            SaveCoins();
        }
    }

    int GetCoins() const { return m_Coins; }

private:
    int m_Coins;
};
