#include "cbase.h"
#include "level_system.h"
#include "filesystem.h"
#include <algorithm>
#include "tier0/memdbgon.h"

#define LEVEL_DATA_FILE "levels/player_level.txt"
#define XP_DATA_FILE "levels/player_xp.txt"

void SaveLevel(int level)
{

    IFileSystem* filesystem = g_pFullFileSystem;
    filesystem->CreateDirHierarchy("levels", "MOD");

    FileHandle_t file = filesystem->Open(LEVEL_DATA_FILE, "w");
    if (file)
    {

        DevMsg("Opened file for writing: %s\n", LEVEL_DATA_FILE);

        char levelStr[64];
        Q_snprintf(levelStr, sizeof(levelStr), "%d", level);
        filesystem->Write(levelStr, Q_strlen(levelStr), file);
        filesystem->Write("\n", 1, file);

        filesystem->Close(file);

        DevMsg("Successfully wrote level data to file: %s\n", LEVEL_DATA_FILE);
    }
    else
    {

        DevWarning("Failed to open file for writing: %s\n", LEVEL_DATA_FILE);
    }
}

void SaveXP(int xp)
{

    IFileSystem* filesystem = g_pFullFileSystem;
    filesystem->CreateDirHierarchy("levels", "MOD");

    FileHandle_t file = filesystem->Open(XP_DATA_FILE, "w");
    if (file)
    {

        DevMsg("Opened file for writing: %s\n", XP_DATA_FILE);

        char xpStr[64];
        Q_snprintf(xpStr, sizeof(xpStr), "%d", xp);
        filesystem->Write(xpStr, Q_strlen(xpStr), file);
        filesystem->Write("\n", 1, file);

        filesystem->Close(file);

        DevMsg("Successfully wrote XP data to file: %s\n", XP_DATA_FILE);
    }
    else
    {

        DevWarning("Failed to open file for writing: %s\n", XP_DATA_FILE);
    }
}

bool LoadLevel(int& level)
{

    IFileSystem* filesystem = g_pFullFileSystem;

    if (filesystem->FileExists(LEVEL_DATA_FILE))
    {
        FileHandle_t file = filesystem->Open(LEVEL_DATA_FILE, "r");
        if (file)
        {

            DevMsg("Opened file for reading: %s\n", LEVEL_DATA_FILE);

            char levelStr[64];
            int bytesRead = filesystem->Read(levelStr, sizeof(levelStr) - 1, file);
            levelStr[bytesRead] = '\0';

            level = atoi(levelStr);

            filesystem->Close(file);

            DevMsg("Successfully loaded level data: Level %d\n", level);
            return true;
        }
        else
        {

            DevWarning("Failed to open file for reading: %s\n", LEVEL_DATA_FILE);
        }
    }
    else
    {

        DevWarning("Level data file does not exist: %s\n", LEVEL_DATA_FILE);

        level = 1;
    }

    return false;
}

bool LoadXP(int& xp)
{

    IFileSystem* filesystem = g_pFullFileSystem;

    if (filesystem->FileExists(XP_DATA_FILE))
    {
        FileHandle_t file = filesystem->Open(XP_DATA_FILE, "r");
        if (file)
        {

            DevMsg("Opened file for reading: %s\n", XP_DATA_FILE);

            char xpStr[64];
            int bytesRead = filesystem->Read(xpStr, sizeof(xpStr) - 1, file);
            xpStr[bytesRead] = '\0';

            xp = atoi(xpStr);

            filesystem->Close(file);

            DevMsg("Successfully loaded XP data: XP %d\n", xp);
            return true;
        }
        else
        {

            DevWarning("Failed to open file for reading: %s\n", XP_DATA_FILE);
        }
    }
    else
    {

        DevWarning("XP data file does not exist: %s\n", XP_DATA_FILE);

        xp = 0;
    }

    return false;
}

void SaveLevelData(int level, int xp)
{
    SaveLevel(level);
    SaveXP(xp);
}

bool LoadLevelData(int& level, int& xp)
{
    bool levelLoaded = LoadLevel(level);
    bool xpLoaded = LoadXP(xp);

    return levelLoaded && xpLoaded;
}