#include "MainFrame.h"

MainFrame::MainFrame(const wxString& title, const std::string fileName) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(416, 600), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	wxInitAllImageHandlers();

	m_ptrConfig = new Config(ToAbsolute(fileName).ToStdString());


	// ── Panel (gives a native background) ───────────────────────────────────
	wxPanel* panel = new wxPanel(this, wxID_ANY);

	wxString path = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + "mc2_hd_logo.png";

	wxBitmap logo(path, wxBITMAP_TYPE_PNG);

	if (!logo.IsOk())
	{
		wxMessageBox("Failed to load mc2_hd_logo.png");
	}

	wxStaticBitmap* image = new wxStaticBitmap(panel, wxID_ANY, logo);

	// ── Buttons ─────────────────────────────────────────────────────────────
	wxButton* btnPlay = new wxButton(panel, ID_BTN_PLAY, "Play");
	btnPlay->SetMinSize(wxSize(-1, 40));
	wxButton* btnFile = new wxButton(panel, ID_BTN_FILE, "Game Files");
	btnFile->SetMinSize(wxSize(-1, 40));
	wxButton* btnGame = new wxButton(panel, ID_BTN_GAME, "Speed");
	btnGame->SetMinSize(wxSize(-1, 40));
	wxButton* btnControls = new wxButton(panel, ID_BTN_CONTROLS, "Controls");
	btnControls->SetMinSize(wxSize(-1, 40));
	wxButton* btnSound = new wxButton(panel, ID_BTN_SOUND, "Sound");
	btnSound->SetMinSize(wxSize(-1, 40));
	wxButton* btnDisplay = new wxButton(panel, ID_BTN_DISPLAY, "Display");
	btnDisplay->SetMinSize(wxSize(-1, 40));
	wxButton* btnGraphics = new wxButton(panel, ID_BTN_GRAPHICS, "Graphics");
	btnGraphics->SetMinSize(wxSize(-1, 40));
	wxButton* btnExit = new wxButton(panel, ID_BTN_EXIT, "Exit");
	btnExit->SetMinSize(wxSize(-1, 40));

	// ── Vertical sizer ──────────────────────────────────────────────────────

	wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
	const wxSizerFlags flags = wxSizerFlags(0)
		.Expand()
		.Border(wxTOP | wxBOTTOM, 4);

	wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);

	buttonSizer->Add(image, flags);
	buttonSizer->Add(btnPlay, flags);
	buttonSizer->Add(btnFile, flags);
	buttonSizer->Add(btnGame, flags);
	buttonSizer->Add(btnControls, flags);
	buttonSizer->Add(btnSound, flags);
	buttonSizer->Add(btnDisplay, flags);
	buttonSizer->Add(btnGraphics, flags);
	buttonSizer->Add(btnExit, flags);

	vSizer->Add(
		buttonSizer,
		wxSizerFlags()
		.Expand()
		.Border(wxLEFT | wxRIGHT, 8)
	);

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
	dlg.SetMinSize(wxSize(430, 195));
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
	dlg.SetMinSize(wxSize(286, 194));
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
	dlg.SetMinSize(wxSize(666, 950));
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
	dlg.SetMinSize(wxSize(404, 332));
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
	dlg.SetMinSize(wxSize(280, 300));
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
	dlg.SetMinSize(wxSize(220, 310));
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