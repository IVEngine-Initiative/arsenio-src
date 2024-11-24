#include "cbase.h"
#include "baseentity.h"
#include "env_level_system.h"
#include "tier0/memdbgon.h"
#include "arsenio/level_system.h" 

LINK_ENTITY_TO_CLASS(env_level_system, CEnvLevelSystem);

BEGIN_DATADESC(CEnvLevelSystem)

DEFINE_INPUTFUNC(FIELD_VOID, "IncreaseXP", InputIncreaseXP),
DEFINE_INPUTFUNC(FIELD_VOID, "ResetLevel", InputResetLevel),

DEFINE_OUTPUT(m_OnLevelUp, "OnLevelUp"),
END_DATADESC()

CEnvLevelSystem::CEnvLevelSystem()
    : m_Level(1), m_XP(0), m_XPRequired(42)
{
    DevMsg("CEnvLevelSystem constructor called. Initializing level system.\n");
}

void CEnvLevelSystem::Spawn()
{
    BaseClass::Spawn();

    if (LoadLevelData(m_Level, m_XP))
    {
        DevMsg("Level data loaded successfully. Level: %d, XP: %d\n", m_Level, m_XP);
    }
    else
    {
        DevMsg("No level data file found. Using default values. Level: %d, XP: %d\n", m_Level, m_XP);
    }

    m_XPRequired = 42;
    DevMsg("CEnvLevelSystem spawned. Level: %d, XP: %d, XP Required: %d\n", m_Level, m_XP, m_XPRequired);
}

void CEnvLevelSystem::InputIncreaseXP(inputdata_t& inputdata)
{
    DevMsg("InputIncreaseXP called. Current XP: %d\n", m_XP);
    m_XP += 10;
    DevMsg("XP increased. New XP: %d\n", m_XP);

    SaveLevelData(m_Level, m_XP);

    CheckLevelUp();
}

void CEnvLevelSystem::InputResetLevel(inputdata_t& inputdata)
{
    DevMsg("InputResetLevel called. Resetting level system.\n");
    m_Level = 1;
    m_XP = 0;
    m_XPRequired = 42;
    DevMsg("Level system reset. Level: %d, XP: %d, XP Required: %d\n", m_Level, m_XP, m_XPRequired);

    SaveLevelData(m_Level, m_XP);

    CheckLevelUp();
}

void CEnvLevelSystem::CheckLevelUp()
{
    DevMsg("Checking level up. Current XP: %d, Required XP: %d\n", m_XP, m_XPRequired);
    if (m_XP >= m_XPRequired)
    {
        DevMsg("Level up! Current Level: %d\n", m_Level);
        m_Level++;
        m_XP = 0;
        m_XPRequired += 42;
        m_OnLevelUp.FireOutput(this, this);
        DevMsg("New Level: %d, New XP Required: %d\n", m_Level, m_XPRequired);

        SaveLevelData(m_Level, m_XP);
    }
    else
    {
        DevMsg("No level up. XP not enough.\n");
    }
}