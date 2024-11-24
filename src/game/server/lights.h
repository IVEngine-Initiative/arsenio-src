//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef LIGHTS_H
#define LIGHTS_H
#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CLight : public CPointEntity
{
public:
	DECLARE_CLASS(CLight, CPointEntity);

	bool	KeyValue(const char* szKeyName, const char* szValue);
	void	Spawn(void);
	void	FadeThink(void);
	void	Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);

	void	TurnOn(void);
	void	TurnOff(void);
	void	Toggle(void);

	// Input handlers
	void	InputSetPattern(inputdata_t& inputdata);
	void	InputFadeToPattern(inputdata_t& inputdata);

	void	InputToggle(inputdata_t& inputdata);
	void	InputTurnOn(inputdata_t& inputdata);
	void	InputTurnOff(inputdata_t& inputdata);

	// New function to enable or disable Godrays
	void	EnableGodrays(bool bEnable);

	// New function to update Godray settings (intensity, scale)
	void	UpdateGodraySettings();
	// Called to spawn the Godray entity
	void	SpawnGodrays();

	// Think function for the light entity
	void	Think(void);


	DECLARE_DATADESC();

private:
	int		m_iStyle;
	int		m_iDefaultStyle;
	string_t m_iszPattern;
	char	m_iCurrentFade;
	char	m_iTargetFade;

	// New members for the Godray system
	bool	m_bGodraysEnabled;        // Whether Godrays are enabled
	float	m_fGodrayIntensity;       // Intensity of the Godrays
	QAngle	m_vecGodrayDirection;     // Direction of the Godrays
	CBaseEntity* m_pGodrayEntity;   // The Godrays model entity
	float	m_fGodrayScale;           // Scale of the Godray effect
};

class CEnvLight : public CLight
{
public:
	DECLARE_CLASS(CEnvLight, CLight);
	DECLARE_DATADESC();



	bool	KeyValue(const char* szKeyName, const char* szValue);
	void	Spawn(void);

	void	FadeThink(void);

	void	TurnOn(void);
	void	TurnOff(void);
	void	Toggle(void);

	void	InputToggle(inputdata_t& inputdata);
	void	InputTurnOn(inputdata_t& inputdata);
	void	InputTurnOff(inputdata_t& inputdata);


	int		m_iPitch;

private:
	int		m_iStyle;
	int		m_iDefaultStyle;
	string_t m_iszPattern;
	char	m_iCurrentFade;
	char	m_iTargetFade;
};

#endif // LIGHTS_H