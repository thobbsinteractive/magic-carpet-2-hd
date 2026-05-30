#pragma once
#include "Globals.h"
#include <wx/statline.h>
#include <wx/display.h>
#include <wx/notebook.h>

class ControlsDialog : public wxDialog
{
public:
	ControlsDialog(wxWindow* parent, const Config::Settings::Controls& cfg);
	Config::Settings::Controls GetSettings() const;

private:
	// Mouse tab widgets
	wxCheckBox* m_invertYAxis;
	wxCheckBox* m_invertXAxis;
	wxCheckBox* m_disableLRButtonsMenuOpen;

	wxSpinCtrlDouble* m_mouseScaleX;
	wxSpinCtrlDouble* m_mouseScaleY;

	wxSpinCtrl* m_spellLeft;
	wxSpinCtrl* m_spellRight;
	wxSpinCtrl* m_map;
	wxSpinCtrl* m_spellMenu;
	wxSpinCtrl* m_spellMenuMark;

	wxNotebook* m_notebook;

	wxPanel* m_mousePage;
	wxPanel* m_keyboardPage;
	wxPanel* m_joystickPage;

	Config::Settings::Controls m_cfg;

	void CreateMousePage();
	void CreateKeyboardPage();
	void CreateJoystickPage();
};