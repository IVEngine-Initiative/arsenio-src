#include "gametype.h"
#include "tier1/strtools.h"  
#include "cbase.h"            
#include <fstream>            
#include <string>             

GameType CGameType::m_gameType = GameType::ARSENIO_2044;

void CGameType::LoadGameTypeFromFile()
{
    std::ifstream file("gametype.txt");
    std::string gameType;

    if (file.is_open())
    {
        std::getline(file, gameType);
        file.close();

        Msg("Loaded game type from file: %s\n", gameType.c_str());

        if (gameType == "ARSENIO 2044")
        {
            m_gameType = GameType::ARSENIO_2044;
        }
        else if (gameType == "DATA REDACTED")
        {
            m_gameType = GameType::DATA_REDACTED;
        }
        else
        {

            Error("ERROR: Invalid Game Type");
        }
    }
    else
    {

        Error("ERROR: No GameType.txt file found.");
    }
}

void CGameType::SetGameType(GameType gameType)
{
    m_gameType = gameType;
}

GameType CGameType::GetGameType()
{
    return m_gameType;
}

std::string CGameType::GetGameTypeAsString(GameType gameType)
{
    switch (gameType)
    {
    case GameType::ARSENIO_2044:
        return "Arsenio 2044";
    case GameType::DATA_REDACTED:
        return "Data Redacted";
    default:
        return "Invalid Game Type";
    }
}

void CGameType::Init()
{
    LoadGameTypeFromFile();
}