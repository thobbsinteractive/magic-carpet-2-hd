#include "GraphicsDialog.h"

// ── Resolution table ─────────────────────────────────────────────────────────
const GraphicsDialog::ResEntry GraphicsDialog::s_resolutions[] =
{
	{ "800 x 600",   800,  600  },
	{ "1024 x 768",  1024, 768  },
	{ "1280 x 720",  1280, 720  },
	{ "1280 x 1024", 1280, 1024 },
	{ "1366 x 768",  1366, 768  },
	{ "1600 x 900",  1600, 900  },
	{ "1920 x 1080", 1920, 1080 },
	{ "2560 x 1440", 2560, 1440 },
	{ "3840 x 2160", 3840, 2160 },
};

int GraphicsDialog::FindResIndex(int w, int h)
{
	for (int i = 0; i < (int)std::size(s_resolutions); ++i)
		if (s_resolutions[i].w == w && s_resolutions[i].h == h)
			return i;
	return (int)std::size(s_resolutions); // points to the "Custom..." entry
}

// ── GraphicsDialog ────────────────────────────────────────────────────────────
GraphicsDialog::GraphicsDialog(wxWindow* parent, const GraphicsSettings& cfg)
	: wxDialog(parent, wxID_ANY, "Graphics Settings",
		wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	BuildUI(cfg);
	Fit();
	Centre();
}

void GraphicsDialog::BuildUI(const GraphicsSettings& cfg)
{
	// Seed custom resolution from config in case it's not in the preset list
	m_customWidth = cfg.windowResWidth;
	m_customHeight = cfg.windowResHeight;

	auto* main = new wxBoxSizer(wxVERTICAL);
	auto* grid = new wxFlexGridSizer(/*rows*/4, /*cols*/2, /*vgap*/10, /*hgap*/12);
	grid->AddGrowableCol(1, 1);

	// ── Display index ────────────────────────────────────────────────────────
	grid->Add(new wxStaticText(this, wxID_ANY, "Display index:"),
		0, wxALIGN_CENTER_VERTICAL);
	m_spinDisplay = new wxSpinCtrl(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS, 0, 7, cfg.displayIndex);
	m_spinDisplay->SetToolTip("Decides which display to use, if it cannot find a display at the index, it will find the first one big enough.");
	grid->Add(m_spinDisplay, 0, wxEXPAND);

	// ── Resolution ───────────────────────────────────────────────────────────
	grid->Add(new wxStaticText(this, wxID_ANY, "Window Resolution:"),
		0, wxALIGN_CENTER_VERTICAL);
	wxArrayString choices;
	for (const auto& r : s_resolutions)
		choices.Add(r.label);
	choices.Add("Custom...");
	m_choiceRes = new wxChoice(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, choices);
	m_choiceRes->SetSelection(FindResIndex(cfg.windowResWidth, cfg.windowResHeight));
	m_choiceRes->Bind(wxEVT_CHOICE, &GraphicsDialog::OnResolutionChanged, this);
	m_choiceRes->SetToolTip("Window resolution, cannot be greater than resolution of chosen display.");
	grid->Add(m_choiceRes, 0, wxEXPAND);

	// ── Maintain aspect ratio ─────────────────────────────────────────────────
	grid->Add(new wxStaticText(this, wxID_ANY, "Maintain aspect ratio:"),
		0, wxALIGN_CENTER_VERTICAL);
	m_chkAspect = new wxCheckBox(this, wxID_ANY, wxEmptyString);
	m_chkAspect->SetValue(cfg.maintainAspectRatio);
	m_chkAspect->SetToolTip("If set to false, whole window will be used for menu screen etc... stretching content.");
	grid->Add(m_chkAspect, 0);

	// ── Start windowed ───────────────────────────────────────────────────────
	grid->Add(new wxStaticText(this, wxID_ANY, "Start windowed:"),
		0, wxALIGN_CENTER_VERTICAL);
	m_chkWindowed = new wxCheckBox(this, wxID_ANY, wxEmptyString);
	m_chkWindowed->SetValue(cfg.startWindowed);
	m_chkWindowed->SetToolTip("Set to true to start windowed. Use Alt-Enter to change in game to windowed mode.");
	grid->Add(m_chkWindowed, 0);

	main->Add(grid, 1, wxEXPAND | wxALL, 14);
	main->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 14);

	// ── Advanced Settings button ─────────────────────────────────────────────
	auto* btnAdvanced = new wxButton(this, wxID_ANY, "Advanced Settings...");
	main->Add(btnAdvanced, 0, wxALIGN_CENTER | wxLEFT | wxTOP | wxBOTTOM, 10);
	btnAdvanced->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
		wxMessageBox("Advanced settings coming soon.",
			"Advanced Settings", wxOK | wxICON_INFORMATION, this);
		});

	main->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 14);

	// ── OK / Cancel ──────────────────────────────────────────────────────────
	auto* btnSizer = new wxStdDialogButtonSizer();
	auto* btnOK = new wxButton(this, wxID_OK, "OK");
	auto* btnCancel = new wxButton(this, wxID_CANCEL, "Cancel");
	btnOK->SetDefault();
	btnSizer->AddButton(btnOK);
	btnSizer->AddButton(btnCancel);
	btnSizer->Realize();
	main->Add(btnSizer, 0, wxEXPAND | wxALL, 10);

	SetSizer(main);

	btnOK->Bind(wxEVT_BUTTON, &GraphicsDialog::OnOK, this);
}

void GraphicsDialog::OnResolutionChanged(wxCommandEvent&)
{
	int sel = m_choiceRes->GetSelection();
	int customIdx = (int)std::size(s_resolutions); // index of "Custom..."

	if (sel != customIdx)
		return; // nothing extra to do for preset entries

	// Ask the user for a custom resolution
	wxDialog dlg(this, wxID_ANY, "Custom Resolution",
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	auto* sizer = new wxBoxSizer(wxVERTICAL);
	auto* row = new wxBoxSizer(wxHORIZONTAL);

	auto* spinW = new wxSpinCtrl(&dlg, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(90, -1),
		wxSP_ARROW_KEYS, 320, 7680, m_customWidth);
	auto* spinH = new wxSpinCtrl(&dlg, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(90, -1),
		wxSP_ARROW_KEYS, 240, 4320, m_customHeight);

	row->Add(new wxStaticText(&dlg, wxID_ANY, "Width:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
	row->Add(spinW, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
	row->Add(new wxStaticText(&dlg, wxID_ANY, "Height:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
	row->Add(spinH, 0, wxALIGN_CENTER_VERTICAL);

	auto* btnSizer = new wxStdDialogButtonSizer();
	auto* btnOK = new wxButton(&dlg, wxID_OK, "OK");
	btnOK->SetDefault();
	btnSizer->AddButton(btnOK);
	btnSizer->AddButton(new wxButton(&dlg, wxID_CANCEL, "Cancel"));
	btnSizer->Realize();

	sizer->Add(row, 0, wxEXPAND | wxALL, 14);
	sizer->Add(new wxStaticLine(&dlg), 0, wxEXPAND | wxLEFT | wxRIGHT, 14);
	sizer->Add(btnSizer, 0, wxEXPAND | wxALL, 10);
	dlg.SetSizerAndFit(sizer);
	dlg.Centre();

	if (dlg.ShowModal() == wxID_OK) {
		m_customWidth = spinW->GetValue();
		m_customHeight = spinH->GetValue();
		// Update the "Custom..." label to show the chosen size
		m_choiceRes->SetString(customIdx,
			wxString::Format("%d x %d (custom)", m_customWidth, m_customHeight));
	}
	else {
		// User cancelled — revert to the previous preset (or keep Custom if it was already set)
		m_choiceRes->SetSelection(FindResIndex(m_customWidth, m_customHeight));
	}
}

void GraphicsDialog::OnOK(wxCommandEvent&)
{
	EndModal(wxID_OK);
}

GraphicsSettings GraphicsDialog::GetSettings() const
{
	GraphicsSettings cfg;
	cfg.displayIndex = m_spinDisplay->GetValue();

	int sel = m_choiceRes->GetSelection();
	int customIdx = (int)std::size(s_resolutions);

	if (sel == customIdx) {
		cfg.windowResWidth = m_customWidth;
		cfg.windowResHeight = m_customHeight;
	}
	else if (sel >= 0 && sel < customIdx) {
		cfg.windowResWidth = s_resolutions[sel].w;
		cfg.windowResHeight = s_resolutions[sel].h;
	}
	cfg.maintainAspectRatio = m_chkAspect->GetValue();
	cfg.startWindowed = m_chkWindowed->GetValue();
	return cfg;
}