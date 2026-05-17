#include "MainFrame.h"

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
	// set the frame icon
	SetIcon(wxICON(sample));

	// ── Panel (gives a native background) ───────────────────────────────────
	wxPanel* panel = new wxPanel(this, wxID_ANY);

	// ── Buttons ─────────────────────────────────────────────────────────────
	wxButton* btnPlay = new wxButton(panel, ID_BTN_PLAY, "Play");
	wxButton* btnFile = new wxButton(panel, ID_BTN_FILE, "Game Files");
	wxButton* btnControls = new wxButton(panel, ID_BTN_CONTROLS, "Controls");
	wxButton* btnSound = new wxButton(panel, ID_BTN_SOUND, "Sound");
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
	vSizer->Add(btnControls, flags);
	vSizer->Add(btnSound, flags);
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
	Bind(wxEVT_BUTTON, &MainFrame::OnControls, this, ID_BTN_CONTROLS);
	Bind(wxEVT_BUTTON, &MainFrame::OnSound, this, ID_BTN_SOUND);
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
	GameFilesDialog dlg(this);
	if (dlg.ShowModal() == wxID_OK) {

	}
}

void MainFrame::OnControls(wxCommandEvent&) { wxLogMessage("Save clicked"); }

void MainFrame::OnSound(wxCommandEvent&)
{
	SoundSettings settings;
	SoundDialog dlg(this, settings);
	if (dlg.ShowModal() == wxID_OK)
		settings = dlg.GetSettings();
}

void MainFrame::OnGraphics(wxCommandEvent&)
{
	GraphicsSettings settings;
	GraphicsDialog dlg(this, settings);
	if (dlg.ShowModal() == wxID_OK)
		settings = dlg.GetSettings();
}

// Event handlers
void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	// true is to force the frame to close
	Close(true);
}