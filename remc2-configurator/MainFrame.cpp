#include "MainFrame.h"

MainFrame::MainFrame(const wxString& title, const std::string fileName) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(300, 350))
{
	m_ptrConfig = new Config(ToAbsolute(fileName).ToStdString());

	// set the frame icon
	SetIcon(wxICON(sample));

	// ── Panel (gives a native background) ───────────────────────────────────
	wxPanel* panel = new wxPanel(this, wxID_ANY);

	// ── Buttons ─────────────────────────────────────────────────────────────
	wxButton* btnPlay = new wxButton(panel, ID_BTN_PLAY, "Play");
	wxButton* btnFile = new wxButton(panel, ID_BTN_FILE, "Game Files");
	wxButton* btnGame = new wxButton(panel, ID_BTN_GAME, "Speed");
	wxButton* btnControls = new wxButton(panel, ID_BTN_CONTROLS, "Controls");
	wxButton* btnSound = new wxButton(panel, ID_BTN_SOUND, "Sound");
	wxButton* btnDisplay = new wxButton(panel, ID_BTN_DISPLAY, "Display");
	wxButton* btnGraphics = new wxButton(panel, ID_BTN_GRAPHICS, "Graphics");
	wxButton* btnExit = new wxButton(panel, ID_BTN_EXIT, "Exit");

	// ── Vertical sizer ──────────────────────────────────────────────────────
	// wxSizerFlags: Expand fills the full width; border adds padding on all sides.
	wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);
	const wxSizerFlags flags = wxSizerFlags(0)
		.Expand()
		.Border(wxALL, 8);

	vSizer->Add(btnPlay, flags);
	vSizer->Add(btnFile, flags);
	vSizer->Add(btnGame, flags);
	vSizer->Add(btnControls, flags);
	vSizer->Add(btnSound, flags);
	vSizer->Add(btnDisplay, flags);
	vSizer->Add(btnGraphics, flags);
	vSizer->Add(btnExit, flags);

	// ── Wrap in an outer sizer to centre the column ─────────────────────────
	wxBoxSizer* outerSizer = new wxBoxSizer(wxVERTICAL);
	outerSizer->AddStretchSpacer(1);
	outerSizer->Add(vSizer, wxSizerFlags(0).Centre());
	outerSizer->AddStretchSpacer(1);

	panel->SetSizer(outerSizer);

	// ── Event bindings ───────────────────────────────────────────────────────
	Bind(wxEVT_BUTTON, &MainFrame::OnPlay, this, ID_BTN_PLAY);
	Bind(wxEVT_BUTTON, &MainFrame::OnFile, this, ID_BTN_FILE);
	Bind(wxEVT_BUTTON, &MainFrame::OnGame, this, ID_BTN_GAME);
	Bind(wxEVT_BUTTON, &MainFrame::OnControls, this, ID_BTN_CONTROLS);
	Bind(wxEVT_BUTTON, &MainFrame::OnSound, this, ID_BTN_SOUND);
	Bind(wxEVT_BUTTON, &MainFrame::OnDisplay, this, ID_BTN_DISPLAY);
	Bind(wxEVT_BUTTON, &MainFrame::OnGraphics, this, ID_BTN_GRAPHICS);
	Bind(wxEVT_BUTTON, &MainFrame::OnExit, this, ID_BTN_EXIT);

	Centre(); // centre on screen
}

// ── Button handlers ──────────────────────────────────────────────────────────
void MainFrame::OnPlay(wxCommandEvent&)
{
	// Launch remc2.exe detached (wxEXEC_ASYNC = fire-and-forget)
	long pid = wxExecute("remc2.exe", wxEXEC_ASYNC);
	if (pid == 0) {
		wxMessageBox("Failed to launch Magic Carpet 2 HD.\n"
			"Make sure it is in the same directory as this application.",
			"Launch Error", wxOK | wxICON_ERROR, this);
		return;
	}
	Close(true); // close the launcher
}

void MainFrame::OnFile(wxCommandEvent&)
{
	auto settings = m_ptrConfig->GetSettingsFromDoc();
	GameFilesDialog dlg(this, settings.m_Paths);
	dlg.SetMinSize(wxSize(440, 200));
	dlg.SetMaxSize(wxSize(440, 200));
	if (dlg.ShowModal() == wxID_OK) {
		auto pathSettings = dlg.GetPaths();
		m_ptrConfig->SavePathsToDoc(pathSettings);
		m_ptrConfig->SaveToFile();
		m_ptrConfig->GetSettingsFromDoc();
	}
}

void MainFrame::OnGame(wxCommandEvent&)
{
	auto settings = m_ptrConfig->GetSettingsFromDoc();
	GameDialog dlg(this, settings.m_Game);
	dlg.SetMinSize(wxSize(300, 200));
	dlg.SetMaxSize(wxSize(300, 200));
	if (dlg.ShowModal() == wxID_OK) {
		auto gameSettings = dlg.GetSettings();
		m_ptrConfig->SaveGameToDoc(gameSettings);
		m_ptrConfig->SaveToFile();
		m_ptrConfig->GetSettingsFromDoc();
	}
}

void MainFrame::OnControls(wxCommandEvent&) 
{ 
	auto settings = m_ptrConfig->GetSettingsFromDoc();
	ControlsDialog dlg(this, settings.m_Controls);
	dlg.SetMinSize(wxSize(264, 500));
	dlg.SetMaxSize(wxSize(264, 500));
	if (dlg.ShowModal() == wxID_OK)
	{
		auto controlSettings = dlg.GetSettings();
		m_ptrConfig->SaveControlsToDoc(controlSettings);
		m_ptrConfig->SaveToFile();
		m_ptrConfig->GetSettingsFromDoc();
	}
}

void MainFrame::OnSound(wxCommandEvent&)
{
	auto settings = m_ptrConfig->GetSettingsFromDoc();
	SoundDialog dlg(this, settings.m_Sound);
	dlg.SetMinSize(wxSize(420, 340));
	dlg.SetMaxSize(wxSize(420, 340));
	if (dlg.ShowModal() == wxID_OK)
	{
		auto soundSettings = dlg.GetSettings();
		m_ptrConfig->SaveSoundToDoc(soundSettings);
		m_ptrConfig->SaveToFile();
		m_ptrConfig->GetSettingsFromDoc();
	}
}

void MainFrame::OnDisplay(wxCommandEvent&)
{
	auto settings = m_ptrConfig->GetSettingsFromDoc();
	DisplayDialog dlg(this, settings.m_Graphics);
	dlg.SetMinSize(wxSize(256, 256));
	dlg.SetMaxSize(wxSize(256, 256));
	if (dlg.ShowModal() == wxID_OK)
	{
		auto graphicSettings = dlg.GetSettings();
		m_ptrConfig->SaveGraphicsToDoc(graphicSettings);
		m_ptrConfig->SaveToFile();
		m_ptrConfig->GetSettingsFromDoc();
	}
}

void MainFrame::OnGraphics(wxCommandEvent&)
{
	auto settings = m_ptrConfig->GetSettingsFromDoc();
	GraphicsDialog dlg(this, settings.m_Graphics);
	dlg.SetMinSize(wxSize(256, 256));
	dlg.SetMaxSize(wxSize(256, 256));
	if (dlg.ShowModal() == wxID_OK)
	{
		auto graphicSettings = dlg.GetSettings();
		m_ptrConfig->SaveGraphicsToDoc(graphicSettings);
		m_ptrConfig->SaveToFile();
		m_ptrConfig->GetSettingsFromDoc();
	}
}

// Event handlers
void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	// true is to force the frame to close
	Close(true);
}