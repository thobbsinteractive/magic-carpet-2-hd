#pragma once
#include "Globals.h"
#include <wx/statline.h>

// ── Dialog ───────────────────────────────────────────────────────────────────
class GraphicsDialog : public wxDialog
{
public:
	GraphicsDialog(wxWindow* parent, const Config::Graphics& cfg);
	Config::Graphics GetSettings() const;

private:
	// Controls
	wxSpinCtrl* m_spinDisplay;
	wxChoice* m_choiceRes;
	wxCheckBox* m_chkAspect;
	wxCheckBox* m_chkWindowed;

	// Tracks the custom resolution entered by the user
	int m_customWidth = 1920;
	int m_customHeight = 1080;

	// Helpers
	void OnOK(wxCommandEvent& evt);
	void OnResolutionChanged(wxCommandEvent& evt);

	// Common resolutions list  { label, width, height }
	struct ResEntry { const char* label; int w; int h; };
	static const ResEntry s_resolutions[];
	static constexpr int CUSTOM_INDEX = -1; // sentinel
	static int FindResIndex(int w, int h);
};