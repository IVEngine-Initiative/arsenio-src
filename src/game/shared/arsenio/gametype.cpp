#include "cbase.h"           
#include "gametype.h"
#include "tier1/strtools.h" 

GameType CGameType::m_gameType = GameType::ARSENIO_2044;

void CGameType::SetGameType(GameType gameType)
{

    if (gameType == GameType::ARSENIO_2044)
    {
        Msg("Setting game type to: Arsenio 2044\n");
    }
    else if (gameType == GameType::DATA_REDACTED)
    {
        Msg("Setting game type to: Data Redacted\n");
    }
    else
    {
        Msg("Setting game type to: Unknown\n");
    }

    m_gameType = gameType;

    Msg("Current game type is now: %s\n", GetGameTypeAsString(m_gameType).c_str());
}

GameType CGameType::GetGameType()
{

    Msg("Getting current game type: %s\n", GetGameTypeAsString(m_gameType).c_str());

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
        return "Unknown";
    }
}