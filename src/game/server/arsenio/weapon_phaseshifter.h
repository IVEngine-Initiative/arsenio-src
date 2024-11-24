#ifndef WEAPON_PHASESHIFTER_H
#define WEAPON_PHASESHIFTER_H

#include "basehlcombatweapon.h"

class CWeaponPhaseShifter : public CBaseHLCombatWeapon
{
public:
    DECLARE_CLASS(CWeaponPhaseShifter, CBaseHLCombatWeapon);
    DECLARE_DATADESC();

    CWeaponPhaseShifter();

    virtual void Precache();
    virtual void PrimaryAttack();
    virtual void SecondaryAttack();
    virtual void ItemPostFrame();

    float GetPhaseOutDuration() const { return m_flPhaseOutDuration; }
    float GetProjectileSpeed() const { return m_flProjectileSpeed; }

private:
    enum Mode {
        PHASE_MODE = 0,
        JUMP_MODE,
        TELEPORT_MODE,
        // Add more modes if needed
        MODE_COUNT // Keeps track of the number of modes
    };

    void PhaseOutEnemy(CBaseEntity* pEnemy);
    void RestoreEnemy();
    void PerformJumpMode();
    void PerformTeleportMode();
    bool IsPositionWithinBounds(const Vector& position);

    float m_flPhaseOutDuration; // Duration of phase-out effect on enemies
    float m_flProjectileSpeed;  // Speed of the Phase Rounds
    Mode m_CurrentMode;         // Current mode of the weapon
};

#endif // WEAPON_PHASESHIFTER_H
