#ifndef GAMECONSOLEDIALOG_H
#define GAMECONSOLEDIALOG_H

#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/consoledialog.h"
#include <Color.h>
#include <utlvector.h>
#include <utllinkedlist.h>
#include "vgui_controls/Frame.h"

class CGameConsoleDialog : public vgui::CConsoleDialog
{
	DECLARE_CLASS_SIMPLE(CGameConsoleDialog, vgui::CConsoleDialog);

public:
	CGameConsoleDialog();

	void ToggleConsole(bool bVisibile, bool bForceOff = false);
	void OnThink();

	static CGameConsoleDialog* GetInstance();

private:
	MESSAGE_FUNC_CHARPTR(OnCommandSubmitted, "CommandSubmitted", command);
	bool bShouldShow;
	bool bShouldClose;
	float m_flLerp;

protected:
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void OnKeyCodeTyped(vgui::KeyCode code);
	virtual void PaintBackground();
	vgui::Label* m_pEngineVersionLabel;

	static CGameConsoleDialog* s_pInstance;
};

#endif 