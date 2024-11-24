#include "cbase.h"
#include "weapon_phaseshifter.h"

LINK_ENTITY_TO_CLASS(weapon_phaseshifter, CWeaponPhaseShifter);

BEGIN_DATADESC(CWeaponPhaseShifter)
DEFINE_FIELD(m_flPhaseOutDuration, FIELD_FLOAT),
DEFINE_FIELD(m_flProjectileSpeed, FIELD_FLOAT),
DEFINE_FIELD(m_CurrentMode, FIELD_INTEGER),
END_DATADESC()

CWeaponPhaseShifter::CWeaponPhaseShifter()
{
    m_flPhaseOutDuration = 15.0f;
    m_flProjectileSpeed = 500.0f;
    m_CurrentMode = PHASE_MODE;
}

void CWeaponPhaseShifter::Precache()
{
    BaseClass::Precache();

}

void CWeaponPhaseShifter::PrimaryAttack()
{
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer || m_flNextPrimaryAttack > gpGlobals->curtime)
        return;

    switch (m_CurrentMode)
    {
    case PHASE_MODE:
    {

        Vector vecShootOrigin = pPlayer->Weapon_ShootPosition();
        Vector vecShootDir;
        AngleVectors(pPlayer->EyeAngles(), &vecShootDir);
        Vector vecEnd = vecShootOrigin + vecShootDir * MAX_TRACE_LENGTH;

        trace_t tr;
        UTIL_TraceLine(vecShootOrigin, vecEnd, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

        if (tr.DidHit() && tr.m_pEnt && tr.m_pEnt->IsNPC())
        {
            PhaseOutEnemy(tr.m_pEnt);
        }
        break;
    }
    case JUMP_MODE:
    {

        Vector vecShootDir;
        AngleVectors(pPlayer->EyeAngles(), &vecShootDir);
        Vector vecVelocity = vecShootDir * m_flProjectileSpeed;
        pPlayer->SetAbsVelocity(vecVelocity);
        break;
    }
    case TELEPORT_MODE:
    {

        Vector vecShootOrigin = pPlayer->Weapon_ShootPosition();
        Vector vecShootDir;
        AngleVectors(pPlayer->EyeAngles(), &vecShootDir);
        Vector vecEnd = vecShootOrigin + vecShootDir * MAX_TRACE_LENGTH;

        trace_t tr;
        UTIL_TraceLine(vecShootOrigin, vecEnd, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

        if (!tr.startsolid && !tr.allsolid)
        {
            Vector newPosition = tr.endpos;

            if (IsPositionWithinBounds(newPosition))
            {
                pPlayer->Teleport(&newPosition, &pPlayer->GetLocalAngles(), &vec3_origin);
            }
            else
            {

                DevMsg("Teleportation out of bounds prevented.\n");
            }
        }
        break;
    }
    default:
        break;
    }

    m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
}

void CWeaponPhaseShifter::SecondaryAttack()
{

    m_CurrentMode = static_cast<Mode>((m_CurrentMode + 1) % MODE_COUNT);
    m_flNextSecondaryAttack = gpGlobals->curtime + 0.5f;

    DevMsg("Switched to mode: %d\n", m_CurrentMode);
}

void CWeaponPhaseShifter::ItemPostFrame()
{
    BaseClass::ItemPostFrame();

}

void CWeaponPhaseShifter::PhaseOutEnemy(CBaseEntity* pEnemy)
{
    if (!pEnemy || !pEnemy->IsNPC())
        return;

    CBaseCombatCharacter* pNPC = pEnemy->MyCombatCharacterPointer();
    if (!pNPC)
        return;

    pNPC->AddEffects(EF_NODRAW);
    pNPC->AddFlag(FL_NOTARGET);
    pNPC->SetSolid(SOLID_NONE);

    pEnemy->SetContextThink(&CWeaponPhaseShifter::RestoreEnemy, gpGlobals->curtime + GetPhaseOutDuration(), "RestoreEnemyThink");
}

void CWeaponPhaseShifter::RestoreEnemy()
{
    CBaseEntity* pEnemy = this;

    if (!pEnemy || !pEnemy->IsNPC())
        return;

    CBaseCombatCharacter* pNPC = pEnemy->MyCombatCharacterPointer();
    if (!pNPC)
        return;

    pNPC->RemoveEffects(EF_NODRAW);
    pNPC->RemoveFlag(FL_NOTARGET);
    pNPC->SetSolid(SOLID_BBOX);

}

bool CWeaponPhaseShifter::IsPositionWithinBounds(const Vector& position)
{

    const float MIN_X = -10000.0f;
    const float MAX_X = 10000.0f;
    const float MIN_Y = -10000.0f;
    const float MAX_Y = 10000.0f;
    const float MIN_Z = 0.0f;
    const float MAX_Z = 5000.0f;

    if (position.x < MIN_X || position.x > MAX_X ||
        position.y < MIN_Y || position.y > MAX_Y ||
        position.z < MIN_Z || position.z > MAX_Z)
    {
        return false;
    }
    return true;
}

void CWeaponPhaseShifter::PerformJumpMode()
{
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

    Vector vecShootOrigin, vecShootDir;
    vecShootOrigin = pPlayer->Weapon_ShootPosition();
    AngleVectors(pPlayer->EyeAngles(), &vecShootDir);

    Vector vecEnd = vecShootOrigin + vecShootDir * MAX_TRACE_LENGTH;

    Vector vecVelocity = vecShootDir * m_flProjectileSpeed;
    pPlayer->SetAbsVelocity(vecVelocity);

}

void CWeaponPhaseShifter::PerformTeleportMode()
{
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

    Vector vecShootOrigin, vecShootDir;
    vecShootOrigin = pPlayer->Weapon_ShootPosition();
    AngleVectors(pPlayer->EyeAngles(), &vecShootDir);

    Vector vecEnd = vecShootOrigin + vecShootDir * MAX_TRACE_LENGTH;

    trace_t tr;
    UTIL_TraceLine(vecShootOrigin, vecEnd, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr);

    if (tr.fraction < 1.0f)
    {
        Vector newPosition = tr.endpos - vecShootDir * 10;

        if (IsPositionWithinBounds(newPosition))
        {
            pPlayer->SetAbsOrigin(newPosition);
        }
        else
        {

            DevMsg("Teleportation out of bounds prevented.\n");
        }
    }
    else
    {
        Vector newPosition = vecEnd;

        if (IsPositionWithinBounds(newPosition))
        {
            pPlayer->SetAbsOrigin(newPosition);
        }
        else
        {

            DevMsg("Teleportation out of bounds prevented.\n");
        }
    }

}