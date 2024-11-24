#include "cbase.h"
#include "point_template.h"
#include "player.h"
#include "entityoutput.h"
#include "tier0/memdbgon.h"

#ifdef ARSENIO_OLD_POINTS

class CPlayerHealth : public CPointEntity
{
public:
    DECLARE_CLASS(CPlayerHealth, CPointEntity);
    DECLARE_DATADESC();

    CPlayerHealth()
    {
        m_flHealthThreshold = 1.0f;
    }

    void InputSetHealthThreshold(inputdata_t& inputdata)
    {
        m_flHealthThreshold = inputdata.value.Float();
    }

    void Think()
    {

        CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

        if (pPlayer)
        {

            if (pPlayer->GetHealth() <= m_flHealthThreshold)
            {

                FireOutput("OnHealthLow", this, this);
            }
        }

        SetNextThink(gpGlobals->curtime + 1.0f);
    }

    void Spawn()
    {
        BaseClass::Spawn();
        SetThink(&CPlayerHealth::Think);
        SetNextThink(gpGlobals->curtime + 1.0f);
    }

private:
    float m_flHealthThreshold;
};

BEGIN_DATADESC(CPlayerHealth)
DEFINE_KEYFIELD(m_flHealthThreshold, FIELD_FLOAT, "healthThreshold"),
DEFINE_INPUTFUNC(FIELD_FLOAT, "SetHealthThreshold", InputSetHealthThreshold),
DEFINE_OUTPUT(m_OnHealthLow, "OnHealthLow"),
END_DATADESC()

LINK_ENTITY_TO_CLASS(player_health, CPlayerHealth);

#ifdef ARSENIO_OLD_POINTS

class CPointsCounter : public CBaseEntity
{
public:
    DECLARE_CLASS(CPointsCounter, CBaseEntity);
    DECLARE_DATADESC();

    CPointsCounter() : m_iPoints(0) {}

    void Spawn() override
    {
        BaseClass::Spawn();
        m_iPoints = 0;
    }

    void InputAddPoints(inputdata_t& inputData)
    {
        int pointsToAdd = inputData.value.Int();
        m_iPoints = MAX(0, m_iPoints + pointsToAdd);
        UpdateClient();
    }

    void InputRemovePoints(inputdata_t& inputData)
    {
        int pointsToRemove = inputData.value.Int();
        m_iPoints = MAX(0, m_iPoints - pointsToRemove);
        UpdateClient();
    }

    void UpdateClient()
    {
        IGameEvent* event = gameeventmanager->CreateEvent("update_points_display");
        if (event)
        {
            event->SetInt("points", m_iPoints);
            gameeventmanager->FireEvent(event);
        }
    }

private:
    int m_iPoints;
};

LINK_ENTITY_TO_CLASS(points_display, CPointsCounter);

BEGIN_DATADESC(CPointsCounter)
DEFINE_KEYFIELD(m_iPoints, FIELD_INTEGER, "InitialPoints"),
DEFINE_INPUTFUNC(FIELD_INTEGER, "AddPoints", InputAddPoints),
DEFINE_INPUTFUNC(FIELD_INTEGER, "RemovePoints", InputRemovePoints),
END_DATADESC()
#endif
#endif