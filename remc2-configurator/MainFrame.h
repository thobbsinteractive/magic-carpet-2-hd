#pragma once
#include <wx/wx.h>

// ─── IDs ────────────────────────────────────────────────────────────────────
enum {
	ID_BTN_PLAY = wxID_HIGHEST + 1,
	ID_BTN_FILE,
	ID_BTN_CONTROLS,
	ID_BTN_SOUND,
	ID_BTN_GRAPHICS,
	ID_BTN_EXIT
};

enum
{
	// menu items
	Minimal_Quit = wxID_EXIT,

	// it is important for the id corresponding to the "About" command to have
	// this standard value as otherwise it won't be handled properly under Mac
	// (where it is special and put into the "Apple" menu)
	Minimal_About = wxID_ABOUT
};

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

private:
	void OnPlay(wxCommandEvent& evt);
	void OnFile(wxCommandEvent& evt);
	void OnControls(wxCommandEvent& evt);
	void OnSound(wxCommandEvent& evt);
	void OnGraphics(wxCommandEvent& evt);
	void OnExit(wxCommandEvent& evt);
};