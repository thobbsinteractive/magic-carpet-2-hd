#pragma once
#include "Globals.h"
#include <wx/statline.h>
#include <wx/clrpicker.h>

// Anaglyph 3D settings of the hardware renderer (F11 in game turns it on and off): the colours of the glasses' lenses.
class AnaglyphDialog : public wxDialog
{
public:
	AnaglyphDialog(wxWindow* parent, const Config::Settings::Anaglyph& cfg);

	Config::Settings::Anaglyph GetSettings() const;

private:
	void OnDefaultColours(wxCommandEvent&);
	void OnOK(wxCommandEvent&);
	// A lens: its colour picker with its "#RRGGBB" box beside it, each updating the other
	wxColourPickerCtrl* AddLens(wxFlexGridSizer* grid, const wxString& label, const wxColour& colour,
		const wxString& toolTip, wxTextCtrl*& hexBox);

	// A "#RRGGBB" lens colour from the config (the # optional, as the game reads it), or the fallback if it isn't one
	static wxColour ParseLensColour(const std::string& text, const wxColour& fallback);
	static std::string ToHex(const wxColour& colour);

	// The defaults, as Config::Settings::Anaglyph: red left, cyan dimmed to 66% right (like the original stereo mode)
	static const wxColour s_defaultLeftColour;
	static const wxColour s_defaultRightColour;

	wxColourPickerCtrl* m_pickLeftColour;
	wxColourPickerCtrl* m_pickRightColour;
	wxTextCtrl* m_txtLeftColour;    // the pickers' colours as "#RRGGBB"
	wxTextCtrl* m_txtRightColour;

	Config::Settings::Anaglyph m_cfg;
};
