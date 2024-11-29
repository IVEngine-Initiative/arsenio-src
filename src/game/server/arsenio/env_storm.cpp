#include "cbase.h"
#include "baseentity.h"
#include "player.h"
#include "util.h"
#include "mathlib/mathlib.h"
#include "tier0/memdbgon.h"


class CEnvStorm : public CBaseEntity
{
    DECLARE_CLASS(CEnvStorm, CBaseEntity);
    DECLARE_DATADESC();

public:
    void Spawn() override;
    void Think() override;

    void StartStorm();
    void StopStorm();
    void UpdateStorm(float deltaTime);
    void DamagePlayersOutsideZone();
    void RenderStormWall();

    void InputStartStorm(inputdata_t& inputData);
    void InputStopStorm(inputdata_t& inputData);
    void InputSetRadius(inputdata_t& inputData);
    void InputSetShrinkRate(inputdata_t& inputData);
    void InputSetShrinkInterval(inputdata_t& inputData);
    void InputSetDamagePerSecond(inputdata_t& inputData);

    void SetRadius(float radius) { m_flTargetRadius = radius; }
    void SetShrinkRate(float rate) { m_flShrinkRate = rate; }
    void SetShrinkInterval(float interval) { m_flShrinkInterval = interval; }
    void SetDamagePerSecond(float damage) { m_flDamagePerSecond = damage; }

private:
    Vector m_vecCenter;
    float m_flCurrentRadius;
    float m_flTargetRadius;
    float m_flShrinkRate;
    float m_flShrinkInterval;
    float m_flDamagePerSecond;
    float m_flShrinkSpeed;
    bool m_bIsStormActive;
};

LINK_ENTITY_TO_CLASS(env_storm, CEnvStorm);

BEGIN_DATADESC(CEnvStorm)
DEFINE_FIELD(m_vecCenter, FIELD_VECTOR),
DEFINE_FIELD(m_flCurrentRadius, FIELD_FLOAT),
DEFINE_FIELD(m_flTargetRadius, FIELD_FLOAT),
DEFINE_FIELD(m_flShrinkRate, FIELD_FLOAT),
DEFINE_FIELD(m_flShrinkInterval, FIELD_FLOAT),
DEFINE_FIELD(m_flDamagePerSecond, FIELD_FLOAT),
DEFINE_FIELD(m_bIsStormActive, FIELD_BOOLEAN),

DEFINE_INPUTFUNC(FIELD_VOID, "StartStorm", InputStartStorm),
DEFINE_INPUTFUNC(FIELD_VOID, "StopStorm", InputStopStorm),
DEFINE_INPUTFUNC(FIELD_FLOAT, "SetRadius", InputSetRadius),
DEFINE_INPUTFUNC(FIELD_FLOAT, "SetShrinkRate", InputSetShrinkRate),
DEFINE_INPUTFUNC(FIELD_FLOAT, "SetShrinkInterval", InputSetShrinkInterval),
DEFINE_INPUTFUNC(FIELD_FLOAT, "SetDamagePerSecond", InputSetDamagePerSecond),
END_DATADESC()

void CEnvStorm::Spawn()
{
    BaseClass::Spawn();
    m_vecCenter = GetAbsOrigin();
    m_flCurrentRadius = 1000.0f;
    m_flTargetRadius = m_flCurrentRadius;
    m_flShrinkRate = 50.0f;
    m_flShrinkInterval = 10.0f;
    m_flDamagePerSecond = 10.0f;
    m_flShrinkSpeed = m_flShrinkRate / m_flShrinkInterval;
    m_bIsStormActive = false;
}

void CEnvStorm::StartStorm()
{
    if (!m_bIsStormActive)
    {
        m_bIsStormActive = true;
        SetThink(&CEnvStorm::Think);
        SetNextThink(gpGlobals->curtime);
    }
}

void CEnvStorm::StopStorm()
{
    if (m_bIsStormActive)
    {
        m_bIsStormActive = false;
        SetThink(nullptr);
    }
}

void CEnvStorm::Think()
{
    if (!m_bIsStormActive)
        return;

    float deltaTime = gpGlobals->frametime;

    UpdateStorm(deltaTime);
    DamagePlayersOutsideZone();
    RenderStormWall();

    SetNextThink(gpGlobals->curtime + deltaTime);
}

void CEnvStorm::UpdateStorm(float deltaTime)
{
    if (m_flCurrentRadius > m_flTargetRadius)
    {

        m_flCurrentRadius -= m_flShrinkSpeed * deltaTime;
        if (m_flCurrentRadius < m_flTargetRadius)
            m_flCurrentRadius = m_flTargetRadius;
    }
    else if (m_flCurrentRadius == m_flTargetRadius)
    {

        m_flTargetRadius = MAX(0.0f, m_flCurrentRadius - m_flShrinkRate);
    }
}

void CEnvStorm::DamagePlayersOutsideZone()
{
    for (int i = 1; i <= gpGlobals->maxClients; i++)
    {
        CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
        if (!pPlayer || !pPlayer->IsAlive())
            continue;

        float distance = (pPlayer->GetAbsOrigin() - m_vecCenter).Length();
        if (distance > m_flCurrentRadius)
        {
            CTakeDamageInfo info(this, this, m_flDamagePerSecond * gpGlobals->frametime, DMG_GENERIC);
            pPlayer->TakeDamage(info);
        }
    }
}

void CEnvStorm::RenderStormWall()
{
    float height = 3000.0f;
    int numSegments = 72;

    Vector center = m_vecCenter + Vector(0, 0, height / 2);
    float angleStep = 2 * M_PI / numSegments;
    float radius = m_flCurrentRadius;



    for (int i = 0; i < numSegments; i++)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;


        Vector point1Bottom = center + Vector(radius * cos(angle1), radius * sin(angle1), -height / 2);
        Vector point2Bottom = center + Vector(radius * cos(angle2), radius * sin(angle2), -height / 2);


        Vector point1Top = center + Vector(radius * cos(angle1), radius * sin(angle1), height / 2);
        Vector point2Top = center + Vector(radius * cos(angle2), radius * sin(angle2), height / 2);


        debugoverlay->AddLineOverlay(point1Bottom, point1Top, 0, 0, 255, true, 0.1f);
        debugoverlay->AddLineOverlay(point2Bottom, point2Top, 0, 0, 255, true, 0.1f);


        debugoverlay->AddLineOverlay(point1Bottom, point2Bottom, 0, 0, 255, true, 0.1f);
        debugoverlay->AddLineOverlay(point1Top, point2Top, 0, 0, 255, true, 0.1f);
    }
}



void CEnvStorm::InputStartStorm(inputdata_t& inputData)
{
    StartStorm();
}

void CEnvStorm::InputStopStorm(inputdata_t& inputData)
{
    StopStorm();
}

void CEnvStorm::InputSetRadius(inputdata_t& inputData)
{
    SetRadius(inputData.value.Float());
}

void CEnvStorm::InputSetShrinkRate(inputdata_t& inputData)
{
    SetShrinkRate(inputData.value.Float());
    m_flShrinkSpeed = m_flShrinkRate / m_flShrinkInterval;
}

void CEnvStorm::InputSetShrinkInterval(inputdata_t& inputData)
{
    SetShrinkInterval(inputData.value.Float());
    m_flShrinkSpeed = m_flShrinkRate / m_flShrinkInterval;
}

void CEnvStorm::InputSetDamagePerSecond(inputdata_t& inputData)
{
    SetDamagePerSecond(inputData.value.Float());
}
