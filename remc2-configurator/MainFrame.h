#pragma once
#include "Globals.h"
#include "GameFilesDialog.h"
#include "GameDialog.h"
#include "SoundDialog.h"
#include "DisplayDialog.h"
#include "GraphicsDialog.h"
#include "PathHelpers.h"
#include "ControlsDialog.h"
#include "MultiplayerDialog.h"

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title, const std::string fileName);

private:

	Config* m_ptrConfig = nullptr;

	void OnPlay(wxCommandEvent& evt);
	void OnLaunchOptions(wxCommandEvent& evt);
	void OnMultiplayer(wxCommandEvent& evt);
	void OnFile(wxCommandEvent& evt);
	void OnGame(wxCommandEvent& evt);
	void OnControls(wxCommandEvent& evt);
	void OnSound(wxCommandEvent& evt);
	void OnDisplay(wxCommandEvent& evt);
	void OnGraphics(wxCommandEvent& evt);
	void OnExit(wxCommandEvent& evt);
};