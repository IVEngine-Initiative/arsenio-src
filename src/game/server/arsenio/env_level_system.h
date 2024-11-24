#ifndef ENV_LEVEL_SYSTEM_H
#define ENV_LEVEL_SYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"

class CEnvLevelSystem : public CBaseEntity
{
    DECLARE_CLASS(CEnvLevelSystem, CBaseEntity);

public:
    CEnvLevelSystem();

    virtual void Spawn() override;

    void InputIncreaseXP(inputdata_t& inputdata);
    void InputResetLevel(inputdata_t& inputdata);

    void CheckLevelUp();

    COutputEvent m_OnLevelUp;
    DECLARE_DATADESC();
private:
    int m_Level;
    int m_XP;
    int m_XPRequired;
};

#endif 