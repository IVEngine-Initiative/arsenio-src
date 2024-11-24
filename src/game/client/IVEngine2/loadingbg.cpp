#include "cbase.h"
#include "loadingbg.h"
#include <vector> 
#include <string> 
#include <vgui/IVGui.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;
using namespace std; 


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMapLoadBG::CMapLoadBG(char const* panelName) : EditablePanel(NULL, panelName)
{
	VPANEL toolParent = enginevgui->GetPanel(PANEL_GAMEUIDLL);
	SetParent(toolParent);

	// Fenix: We load a RES file rather than create the element here for taking advantage of the "F" parameter for wide and tall
	// Is the sole thing that makes fill the background to the entire screen regardless of the texture size
	// Congratulations to Valve for once again give options to only one side and not both
	LoadControlSettings("resource/loadingdialogbackground.res");

	m_pBackground = FindControl<ImagePanel>("LoadingImage", true);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CMapLoadBG::~CMapLoadBG()
{
	// None
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMapLoadBG::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	int iWide, iTall;
	surface()->GetScreenSize(iWide, iTall);
	SetSize(iWide, iTall);
}

//-----------------------------------------------------------------------------
// Purpose: Sets a new background on demand
//-----------------------------------------------------------------------------
void CMapLoadBG::SetNewBackgroundImage(char const* imageName)
{
	m_pBackground->SetImage(imageName);
}