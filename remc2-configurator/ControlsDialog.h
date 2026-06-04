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

	static ConfigToSdlScancode m_configToSdlScancode;

	// Mouse tab widgets
	wxCheckBox* m_invertYAxis;
	wxCheckBox* m_invertXAxis;
	wxCheckBox* m_disableLRButtonsMenuOpen;

	wxSpinCtrlDouble* m_mouseScaleX;
	wxSpinCtrlDouble* m_mouseScaleY;

	wxChoice* m_spellLeft;
	wxChoice* m_spellRight;
	wxChoice* m_map;
	wxChoice* m_spellMenu;
	wxChoice* m_spellMenuMark;

	// Keyboard tab widgets
	wxButton* m_modernButton = nullptr;
	wxButton* m_classicButton = nullptr;

	wxChoice* m_forwardKey = nullptr;
	wxChoice* m_backwardsKey = nullptr;
	wxChoice* m_leftKey = nullptr;
	wxChoice* m_rightKey = nullptr;
	wxChoice* m_mapKey = nullptr;
	wxChoice* m_spellMenuKey = nullptr;
	wxChoice* m_spellMenuMarkKey = nullptr;

	wxButton* m_xboxPresetButton = nullptr;
	wxSpinCtrl* m_controllerId = nullptr;
	wxSpinCtrl* m_buttonMiniMap = nullptr;
	wxSpinCtrl* m_buttonSpell = nullptr;
	wxSpinCtrl* m_buttonPauseMenu = nullptr;
	wxSpinCtrl* m_buttonEsc = nullptr;
	wxSpinCtrl* m_buttonFireL = nullptr;
	wxSpinCtrl* m_buttonFireR = nullptr;
	wxSpinCtrl* m_buttonMenuSelect = nullptr;

	wxNotebook* m_notebook;

	wxPanel* m_mousePage;
	wxPanel* m_keyboardPage;
	wxPanel* m_joystickPage;

	Config::Settings::Controls m_cfg;
	void SetChoiceValue(wxChoice* choice, const wxString& value);
	void OnModernLayout(wxCommandEvent&);
	void OnClassicLayout(wxCommandEvent&);
	void OnXBoxPreset(wxCommandEvent& evt);
	void LoadKeyboardLayout(const wxString& layoutName);

	void CreateMousePage();
	void CreateKeyboardPage();
	void CreateJoystickPage();
	wxArrayString GetAvailableKeys() const;
	SDL_Scancode GetSelectedScancode(wxChoice* choice) const;
	void OnOK(wxCommandEvent&);
};