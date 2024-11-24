//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: spawn and think functions for editor-placed lights
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "lights.h"
#include "world.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(light, CLight);

BEGIN_DATADESC(CLight)

DEFINE_FIELD(m_iCurrentFade, FIELD_CHARACTER),
DEFINE_FIELD(m_iTargetFade, FIELD_CHARACTER),

DEFINE_KEYFIELD(m_iStyle, FIELD_INTEGER, "style"),
DEFINE_KEYFIELD(m_iDefaultStyle, FIELD_INTEGER, "defaultstyle"),
DEFINE_KEYFIELD(m_iszPattern, FIELD_STRING, "pattern"),

// New fields for Godray system
DEFINE_KEYFIELD(m_bGodraysEnabled, FIELD_BOOLEAN, "godrays"),  // Keyvalue to toggle Godrays
DEFINE_KEYFIELD(m_fGodrayIntensity, FIELD_FLOAT, "godray_intensity"), // Intensity of the Godrays
DEFINE_KEYFIELD(m_pGodrayEntity, FIELD_EHANDLE, "godray_entity"),  // Handle for the Godray entity
DEFINE_KEYFIELD(m_fGodrayScale, FIELD_FLOAT, "godray_scale"),  // Scale of the Godray effect


// Fuctions
DEFINE_FUNCTION(FadeThink),

// Inputs
DEFINE_INPUTFUNC(FIELD_STRING, "SetPattern", InputSetPattern),
DEFINE_INPUTFUNC(FIELD_STRING, "FadeToPattern", InputFadeToPattern),
DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),
DEFINE_INPUTFUNC(FIELD_VOID, "TurnOn", InputTurnOn),
DEFINE_INPUTFUNC(FIELD_VOID, "TurnOff", InputTurnOff),


END_DATADESC()



//
// Cache user-entity-field values until spawn is called.
//
bool CLight::KeyValue(const char* szKeyName, const char* szValue)
{
	if (FStrEq(szKeyName, "godrays"))
	{
		m_bGodraysEnabled = atoi(szValue) > 0;
	}
	else if (FStrEq(szKeyName, "godray_intensity"))
	{
		m_fGodrayIntensity = atof(szValue);
	}
	else if (FStrEq(szKeyName, "godray_scale"))
	{
		m_fGodrayScale = atof(szValue);
	}
	else if (FStrEq(szKeyName, "pitch"))
	{
		QAngle angles = GetAbsAngles();
		angles.x = atof(szValue);
		SetAbsAngles(angles);
	}
	else
	{
		return BaseClass::KeyValue(szKeyName, szValue);
	}

	return true;
}


// Light entity
// If targeted, it will toggle between on or off.
void CLight::Spawn(void)
{
	if (!GetEntityName())
	{       // inert light
		UTIL_Remove(this);
		return;
	}

	if (m_iStyle >= 32)
	{
		if (m_iszPattern == NULL_STRING && m_iDefaultStyle > 0)
		{
			m_iszPattern = MAKE_STRING(GetDefaultLightstyleString(m_iDefaultStyle));
		}

		if (FBitSet(m_spawnflags, SF_LIGHT_START_OFF))
			engine->LightStyle(m_iStyle, "a");
		else if (m_iszPattern != NULL_STRING)
			engine->LightStyle(m_iStyle, (char*)STRING(m_iszPattern));
		else
			engine->LightStyle(m_iStyle, "m");
	}
	// Check if Godrays are enabled, and spawn the godray entity if so
	if (m_bGodraysEnabled)
	{
		SpawnGodrays();
	}

	// Set the default intensity and scale
	if (m_fGodrayIntensity <= 0.0f) m_fGodrayIntensity = 1.0f;  // Default intensity if not set
	if (m_fGodrayScale <= 0.0f) m_fGodrayScale = 1.0f;  // Default scale if not set
}

void CLight::SpawnGodrays()
{
	// Spawn the Godray model (ep2_pak.vpk\models\effects\vol_light128x128.mdl)
	if (!m_pGodrayEntity)
	{
		m_pGodrayEntity = CreateEntityByName("env_model");
		if (m_pGodrayEntity)
		{
			// Set model and other properties
			m_pGodrayEntity->SetModel("models/effects/vol_light128x128.mdl");
			m_pGodrayEntity->SetAbsOrigin(GetAbsOrigin());  // Position it at the light's location
			m_pGodrayEntity->SetAbsAngles(GetAbsAngles());  // Align with the light's angle
			m_pGodrayEntity->SetParent(this);  // Attach it to the light

			// Scale the godray model based on the intensity and scale
		//	Vector scale = Vector(m_fGodrayScale, m_fGodrayScale, m_fGodrayScale);
		//	m_pGodrayEntity->SetModelScale(scale);
		}
	}
}

void CLight::EnableGodrays(bool bEnable)
{
	if (bEnable)
	{
		if (m_pGodrayEntity)
		{
			// Enable the Godrays by removing the NODRAW effect
			m_pGodrayEntity->RemoveEffects(EF_NODRAW);
			// Set the intensity and scale for the Godrays
			UpdateGodraySettings();
		}
	}
	else
	{
		if (m_pGodrayEntity)
		{
			// Hide the Godrays by adding the NODRAW effect
			m_pGodrayEntity->AddEffects(EF_NODRAW);
		}
	}
}

void CLight::UpdateGodraySettings()
{
	// Update the intensity (could involve scaling, transparency, or other adjustments)
	if (m_pGodrayEntity)
	{
		// Adjust the model scale or other properties to match the intensity
	//	Vector scale = Vector(m_fGodrayIntensity * m_fGodrayScale, m_fGodrayIntensity * m_fGodrayScale, m_fGodrayIntensity * m_fGodrayScale);
	//	m_pGodrayEntity->SetModelScale(scale);
	}

	// Optionally, adjust transparency or other visual effects based on intensity
	// For example: m_pGodrayEntity->SetRenderColorA(intensity * 255);
}

void CLight::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	if (m_iStyle >= 32)
	{
		if (!ShouldToggle(useType, !FBitSet(m_spawnflags, SF_LIGHT_START_OFF)))
			return;

		Toggle();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Turn the light on
//-----------------------------------------------------------------------------
void CLight::TurnOn(void)
{
	if (m_iszPattern != NULL_STRING)
	{
		engine->LightStyle(m_iStyle, (char*)STRING(m_iszPattern));
	}
	else
	{
		engine->LightStyle(m_iStyle, "m");
	}

	if (m_bGodraysEnabled && m_pGodrayEntity)
	{
		// Adjust the direction of the Godrays
		QAngle angles = GetAbsAngles();
		m_pGodrayEntity->SetAbsAngles(angles);  // Align with light's angle

		// Adjust the intensity of the Godrays, if needed (example logic)
		if (m_fGodrayIntensity > 0.0f)
		{
			// Apply changes to the model to reflect intensity here
			// This could involve scaling the entity or changing its transparency
		}
	}

	CLEARBITS(m_spawnflags, SF_LIGHT_START_OFF);
}

void CLight::Think(void)
{
	BaseClass::Think();

	// If Godrays are enabled, update their position and angle to follow the light
	if (m_bGodraysEnabled && m_pGodrayEntity)
	{
		// Adjust the direction of the Godrays
		QAngle angles = GetAbsAngles();
		m_pGodrayEntity->SetAbsAngles(angles);  // Align with light's angle

		// Update the intensity and scale based on light's settings
		UpdateGodraySettings();
	}

	// Continue with the usual light think logic...
}

//-----------------------------------------------------------------------------
// Purpose: Turn the light off
//-----------------------------------------------------------------------------
void CLight::TurnOff(void)
{
	engine->LightStyle(m_iStyle, "a");
	SETBITS(m_spawnflags, SF_LIGHT_START_OFF);
}

//-----------------------------------------------------------------------------
// Purpose: Toggle the light on/off
//-----------------------------------------------------------------------------
void CLight::Toggle(void)
{
	//Toggle it
	if (FBitSet(m_spawnflags, SF_LIGHT_START_OFF))
	{
		TurnOn();
	}
	else
	{
		TurnOff();
	}
	// Toggle godrays based on light state
	EnableGodrays(!m_bGodraysEnabled);
}

//-----------------------------------------------------------------------------
// Purpose: Handle the "turnon" input handler
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CLight::InputTurnOn(inputdata_t& inputdata)
{
	TurnOn();
}

//-----------------------------------------------------------------------------
// Purpose: Handle the "turnoff" input handler
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CLight::InputTurnOff(inputdata_t& inputdata)
{
	TurnOff();
}

//-----------------------------------------------------------------------------
// Purpose: Handle the "toggle" input handler
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CLight::InputToggle(inputdata_t& inputdata)
{
	Toggle();
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for setting a light pattern
//-----------------------------------------------------------------------------
void CLight::InputSetPattern(inputdata_t& inputdata)
{
	m_iszPattern = inputdata.value.StringID();
	engine->LightStyle(m_iStyle, (char*)STRING(m_iszPattern));

	// Light is on if pattern is set
	CLEARBITS(m_spawnflags, SF_LIGHT_START_OFF);
}


//-----------------------------------------------------------------------------
// Purpose: Input handler for fading from first value in old pattern to 
//			first value in new pattern
//-----------------------------------------------------------------------------
void CLight::InputFadeToPattern(inputdata_t& inputdata)
{
	m_iCurrentFade = (STRING(m_iszPattern))[0];
	m_iTargetFade = inputdata.value.String()[0];
	m_iszPattern = inputdata.value.StringID();
	SetThink(&CLight::FadeThink);
	SetNextThink(gpGlobals->curtime);

	// Light is on if pattern is set
	CLEARBITS(m_spawnflags, SF_LIGHT_START_OFF);
}


//------------------------------------------------------------------------------
// Purpose : Fade light to new starting pattern value then stop thinking
//------------------------------------------------------------------------------
void CLight::FadeThink(void)
{
	if (m_iCurrentFade < m_iTargetFade)
	{
		m_iCurrentFade++;
	}
	else if (m_iCurrentFade > m_iTargetFade)
	{
		m_iCurrentFade--;
	}

	// If we're done fading instantiate our light pattern and stop thinking
	if (m_iCurrentFade == m_iTargetFade)
	{
		engine->LightStyle(m_iStyle, (char*)STRING(m_iszPattern));
		SetNextThink(TICK_NEVER_THINK);
	}
	// Otherwise instantiate our current fade value and keep thinking
	else
	{
		char sCurString[2];
		sCurString[0] = m_iCurrentFade;
		sCurString[1] = 0;
		engine->LightStyle(m_iStyle, sCurString);

		// UNDONE: Consider making this settable war to control fade speed
		SetNextThink(gpGlobals->curtime + 0.1f);
	}
}

//
// shut up spawn functions for new spotlights
//
LINK_ENTITY_TO_CLASS(light_spot, CLight);
LINK_ENTITY_TO_CLASS(light_glspot, CLight);



LINK_ENTITY_TO_CLASS(light_environment, CEnvLight);

BEGIN_DATADESC(CEnvLight)

// Fuctions
DEFINE_FUNCTION(FadeThink),

// Inputs
DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),
DEFINE_INPUTFUNC(FIELD_VOID, "TurnOn", InputTurnOn),
DEFINE_INPUTFUNC(FIELD_VOID, "TurnOff", InputTurnOff),

END_DATADESC()

//------------------------------------------------------------------------------
// Purpose : Fade light to new starting pattern value then stop thinking
//------------------------------------------------------------------------------
void CEnvLight::FadeThink(void)
{
	if (m_iCurrentFade < m_iTargetFade)
	{
		m_iCurrentFade++;
	}
	else if (m_iCurrentFade > m_iTargetFade)
	{
		m_iCurrentFade--;
	}

	// If we're done fading instantiate our light pattern and stop thinking
	if (m_iCurrentFade == m_iTargetFade)
	{
		engine->LightStyle(m_iStyle, (char*)STRING(m_iszPattern));
		SetNextThink(TICK_NEVER_THINK);
	}
	// Otherwise instantiate our current fade value and keep thinking
	else
	{
		char sCurString[2];
		sCurString[0] = m_iCurrentFade;
		sCurString[1] = 0;
		engine->LightStyle(m_iStyle, sCurString);

		// UNDONE: Consider making this settable war to control fade speed
		SetNextThink(gpGlobals->curtime + 0.1f);
	}
}

bool CEnvLight::KeyValue(const char* szKeyName, const char* szValue)
{
	if (FStrEq(szKeyName, "_light"))
	{
		// nothing
	}
	else if (FStrEq(szKeyName, "pitch"))
	{
		m_iPitch = atoi(szValue);
	}
	else
	{
		return BaseClass::KeyValue(szKeyName, szValue);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Turn the light on
//-----------------------------------------------------------------------------
void CEnvLight::TurnOn(void)
{
	if (m_iszPattern != NULL_STRING)
	{
		engine->LightStyle(m_iStyle, (char*)STRING(m_iszPattern));
	}
	else
	{
		engine->LightStyle(m_iStyle, "m");
	}

	CLEARBITS(m_spawnflags, SF_LIGHT_START_OFF);
}

//-----------------------------------------------------------------------------
// Purpose: Turn the light off
//-----------------------------------------------------------------------------
void CEnvLight::TurnOff(void)
{
	engine->LightStyle(m_iStyle, "a");
	SETBITS(m_spawnflags, SF_LIGHT_START_OFF);
}

//-----------------------------------------------------------------------------
// Purpose: Toggle the light on/off
//-----------------------------------------------------------------------------
void CEnvLight::Toggle(void)
{
	//Toggle it
	if (FBitSet(m_spawnflags, SF_LIGHT_START_OFF))
	{
		TurnOn();
	}
	else
	{
		TurnOff();
	}
}


void CEnvLight::Spawn(void)
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Handle the "turnon" input handler
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CEnvLight::InputTurnOn(inputdata_t& inputdata)
{
	TurnOn();
}

//-----------------------------------------------------------------------------
// Purpose: Handle the "turnoff" input handler
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CEnvLight::InputTurnOff(inputdata_t& inputdata)
{
	TurnOff();
}

//-----------------------------------------------------------------------------
// Purpose: Handle the "toggle" input handler
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CEnvLight::InputToggle(inputdata_t& inputdata)
{
	Toggle();
}