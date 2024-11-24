#ifndef GAME_TYPE_H
#define GAME_TYPE_H

#include <string>  

enum class GameType
{
    ARSENIO_2044,
    DATA_REDACTED,
    GAME_TYPE_INVALID
};

class CGameType
{
public:

    static void SetGameType(GameType gameType);

    static GameType GetGameType();

    static std::string GetGameTypeAsString(GameType gameType);

private:

    static GameType m_gameType;
};

#endif 