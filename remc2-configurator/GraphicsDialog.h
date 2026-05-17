#pragma once
#include "Globals.h"
#include <wx/statline.h>

// ── Graphics settings ────────────────────────────────────────────────────────
struct GraphicsSettings
{
	int  displayIndex = 0;
	int  windowResWidth = 1920;
	int  windowResHeight = 1080;
	bool maintainAspectRatio = true;
	bool startWindowed = false;
};

// ── Dialog ───────────────────────────────────────────────────────────────────
class GraphicsDialog : public wxDialog
{
public:
	GraphicsDialog(wxWindow* parent, const GraphicsSettings& cfg);
	GraphicsSettings GetSettings() const;

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
	void BuildUI(const GraphicsSettings& cfg);
	void OnOK(wxCommandEvent& evt);
	void OnResolutionChanged(wxCommandEvent& evt);

	// Common resolutions list  { label, width, height }
	struct ResEntry { const char* label; int w; int h; };
	static const ResEntry s_resolutions[];
	static constexpr int CUSTOM_INDEX = -1; // sentinel
	static int FindResIndex(int w, int h);
};