#pragma once
#include "Globals.h"
#include "GameFilesDialog.h"
#include "SoundDialog.h"
#include "GraphicsDialog.h"
#include "PathHelpers.h"

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title, const std::string fileName);

private:

	Config* m_ptrConfig = nullptr;

	void OnPlay(wxCommandEvent& evt);
	void OnFile(wxCommandEvent& evt);
	void OnControls(wxCommandEvent& evt);
	void OnSound(wxCommandEvent& evt);
	void OnGraphics(wxCommandEvent& evt);
	void OnExit(wxCommandEvent& evt);
};