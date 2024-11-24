//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Revolver that shoots lasers (for DLC pack 1, Data Redacted)
//
// Evolva Revolver? lol
// 
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "basehlcombatweapon.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "gamerules.h"
#include "in_buttons.h"
#include "soundent.h"
#include "game.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "te_effect_dispatch.h"
#include "gamestats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
//#include "weapon_revolver_DR.h"



//-----------------------------------------------------------------------------
// CWeaponRevolverDR
//-----------------------------------------------------------------------------

class CWeaponRevolverDR : public CBaseHLCombatWeapon
{
	DECLARE_CLASS( CWeaponRevolverDR, CBaseHLCombatWeapon );
public:

	CWeaponRevolverDR( void );

	void	PrimaryAttack( void );
	void	SecondaryAttack(void);
	void	ItemPostFrame(void);
	void	Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
	bool	m_bZoomed;
	virtual bool Reload(void);
	bool Holster(CBaseCombatWeapon* pSwitchingTo);


	float	WeaponAutoAimScale()	{ return 0.6f; }

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( weapon_revolver_dr, CWeaponRevolverDR );

PRECACHE_WEAPON_REGISTER( weapon_revolver_dr );

IMPLEMENT_SERVERCLASS_ST( CWeaponRevolverDR, DT_WeaponRevolverDR )
END_SEND_TABLE()

BEGIN_DATADESC( CWeaponRevolverDR )

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponRevolverDR::CWeaponRevolverDR( void )
{
	m_bReloadsSingly	= false;
	m_bFiresUnderwater	= false;
	m_bZoomed = false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponRevolverDR::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	switch( pEvent->event )
	{
		case EVENT_WEAPON_RELOAD:
			{
				CEffectData data;

				// Emit six spent shells
				for ( int i = 0; i < 6; i++ )
				{
					data.m_vOrigin = pOwner->WorldSpaceCenter() + RandomVector( -4, 4 );
					data.m_vAngles = QAngle( 90, random->RandomInt( 0, 360 ), 0 );
					data.m_nEntIndex = entindex();

					DispatchEffect( "ShellEject", data );
				}

				break;
			}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponRevolverDR::PrimaryAttack( void )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( !pPlayer )
	{
		return;
	}

	if ( m_iClip1 <= 0 )
	{
		if ( !m_bFireOnEmpty )
		{
			Reload();
		}
		else
		{
			WeaponSound( EMPTY );
			m_flNextPrimaryAttack = 0.15;
		}

		return;
	}

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired( pPlayer, true, GetClassname() );

	WeaponSound( SINGLE );
	pPlayer->DoMuzzleFlash();

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );
	pPlayer->SetAnimation( PLAYER_ATTACK1 );
	// pPlayer->SetFOV( this, 0 );
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.75;
	m_flNextSecondaryAttack = gpGlobals->curtime + 0.75;

	m_iClip1--;

	Vector vecSrc		= pPlayer->Weapon_ShootPosition();
	Vector vecAiming	= pPlayer->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );	

	pPlayer->FireBullets( 1, vecSrc, vecAiming, vec3_origin, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 0 );

	pPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	//Disorient the player
	QAngle angles = pPlayer->GetLocalAngles();

	angles.x += random->RandomInt( -1, 1 );
	angles.y += random->RandomInt( -1, 1 );
	angles.z = 0;

	pPlayer->SnapEyeAngles( angles );

	pPlayer->ViewPunch( QAngle( -8, random->RandomFloat( -2, 2 ), 0 ) );

	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 600, 0.2, GetOwner() );

	if ( !m_iClip1 && pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 ); 
	}
}

bool CWeaponRevolverDR::Reload(void)
{
	bool fRet;


	fRet = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

	if (m_bZoomed)
	{ 
		pPlayer->SetFOV(this, 0, 0);
		m_bZoomed = false;
	}

	return fRet;
}

bool CWeaponRevolverDR::Holster(CBaseCombatWeapon* pSwitchingTo)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

	if (m_bZoomed)
	{
		pPlayer->SetFOV(this, 0, 0);
		m_bZoomed = false;
	}

	return BaseClass::Holster(pSwitchingTo);
}

void CWeaponRevolverDR::ItemPostFrame(void)
{

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

	CBasePlayer* pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	if (pOwner->m_afButtonPressed & IN_ALT1)
	{
		if (!m_bZoomed)
		{
			pPlayer->SetFOV(this, 25, 0);
			m_bZoomed = true;

		}
		else // fixed error
		{
			pPlayer->SetFOV(this, 0, 0);
			m_bZoomed = false;
		}

	}

	if (pOwner->m_afButtonPressed & IN_ALT1) // idea for a laser sight like op4's desert eagle
	{
		/*if (!m_bZoomed)
		{
			pPlayer->SetFOV(this, 20);
			m_bZoomed = true;

		}
		else if (m_bZoomed = true)
		{
			pPlayer->SetFOV(this, 0);
			m_bZoomed = false;
		}*/

	}
	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponRevolverDR::SecondaryAttack(void)
{
	/*
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (m_bZoomed = false)
	{
		pPlayer->SetFOV(this, -125);
		m_bZoomed = true;

	}
	else if (m_bZoomed = true)
	{
		pPlayer->SetFOV(this, 0);
		m_bZoomed = false;
	}
	*/
}