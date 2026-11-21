#pragma once
#include "Globals.h"
#include <wx/statline.h>

// VR headset settings of the hardware renderer (OpenXR: SteamVR, Meta, Monado or a simulator): on or off, and the eye
// resolution the game runs at while a headset is found (the menus, the UI and each eye are drawn at that size).
class VrDialog : public wxDialog
{
public:
	VrDialog(wxWindow* parent, const Config::Settings::OpenXR& cfg);

	Config::Settings::OpenXR GetSettings() const;

private:
	struct Preset
	{
		const char* name;
		int eyeWidth;    // 0 x 0: the headset eye's shape at the in-game resolution's height
		int eyeHeight;
	};
	static const Preset s_presets[];

	void OnPreset(wxCommandEvent&);
	void SelectMatchingPreset();   // the preset with the eye size shown, or "Custom"
	void UpdateEnabled();
	void OnOK(wxCommandEvent&);

	wxCheckBox* m_chkEnabled;
	wxChoice* m_choicePreset;
	wxSpinCtrl* m_spinEyeWidth;
	wxSpinCtrl* m_spinEyeHeight;

	Config::Settings::OpenXR m_cfg;
};
