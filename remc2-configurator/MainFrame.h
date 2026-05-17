#pragma once
#include "Globals.h"
#include "GameFilesDialog.h"
#include "SoundDialog.h"

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