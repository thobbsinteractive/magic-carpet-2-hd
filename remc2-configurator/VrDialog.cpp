#include "VrDialog.h"
#include <iterator>

// Headsets' eye sizes (the shape is what matters: the game is drawn at it, the headset scales it to its own). First the
// runtimes' 100% default render sizes, then the native panels. Headsets with the same size share a preset (the dialog
// shows the first preset matching the size). The Quest 3's native 2064 x 2208 is taller than the game's 2160 lines
const VrDialog::Preset VrDialog::s_presets[] = {
	{ "Meta Quest 3 / 3S (1680 x 1760, default render)", 1680, 1760 },
	{ "Meta Quest 2 / Pro (1440 x 1584, default render)", 1440, 1584 },
	{ "Meta Quest 1 (1216 x 1344, default render)", 1216, 1344 },
	{ "Meta Quest 3S / 2 (1832 x 1920, native)", 1832, 1920 },
	{ "Meta Quest Pro (1800 x 1920, native)", 1800, 1920 },
	{ "Meta Quest 1 / Valve Index (1440 x 1600, native)", 1440, 1600 },
	{ "Valve Steam Frame (1728 x 1728)", 1728, 1728 },
	{ "Phone through ALVR / PhoneVR (1080 x 1080)", 1080, 1080 },
	{ "Match the headset (its eye shape at the in-game resolution's height)", 0, 0 },
};

static constexpr int kMaxEyeWidth = 3840;    // the game's largest resolution
static constexpr int kMaxEyeHeight = 2160;

VrDialog::VrDialog(wxWindow* parent, const Config::Settings::OpenXR& cfg)
	: wxDialog(parent, wxID_ANY, "VR Headset Settings",
		wxDefaultPosition, wxSize(460, 300),
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	m_cfg = cfg;

	auto* main = new wxBoxSizer(wxVERTICAL);

	auto* intro = new wxStaticText(this, wxID_ANY,
		"A VR headset through OpenXR (SteamVR, Meta, Monado or a simulator).\n"
		"The head looks around and the game draws the two eyes for the headset.\n"
		"Without a headset the game plays normally.");
	main->Add(intro, 0, wxLEFT | wxRIGHT | wxTOP, 14);

	// ── On / off ──────────────────────────────────────────────────────────────
	m_chkEnabled = new wxCheckBox(this, wxID_ANY, "VR headset on");
	m_chkEnabled->SetValue(cfg.m_Enabled);
	m_chkEnabled->SetToolTip("Draws the game for a VR headset when one is found.");
	main->Add(m_chkEnabled, 0, wxLEFT | wxRIGHT | wxTOP, 14);
	m_chkEnabled->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) { UpdateEnabled(); });

	// ── Eye resolution ────────────────────────────────────────────────────────
	auto* grid = new wxFlexGridSizer(/*rows*/3, /*cols*/2, /*vgap*/10, /*hgap*/12);
	grid->AddGrowableCol(1, 1);

	grid->Add(new wxStaticText(this, wxID_ANY, "Headset:"), 0, wxALIGN_CENTER_VERTICAL);
	m_choicePreset = new wxChoice(this, wxID_ANY);
	for (const Preset& preset : s_presets)
		m_choicePreset->Append(preset.name);
	m_choicePreset->Append("Custom");
	m_choicePreset->SetToolTip("Sets the eye resolution for a headset.");
	grid->Add(m_choicePreset, 0, wxEXPAND);
	m_choicePreset->Bind(wxEVT_CHOICE, &VrDialog::OnPreset, this);

	grid->Add(new wxStaticText(this, wxID_ANY, "Eye width:"), 0, wxALIGN_CENTER_VERTICAL);
	m_spinEyeWidth = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, -1),
		wxSP_ARROW_KEYS, 0, kMaxEyeWidth, (std::max)(cfg.m_EyeWidth, 0));
	m_spinEyeWidth->SetToolTip("The resolution of each eye: the game resolution while a headset is found (the menus, "
		"the UI and the 3D view are drawn at this size). Keep it the headset eye's shape or the view is stretched. "
		"0 x 0 = the headset eye's shape at the in-game resolution's height.");
	grid->Add(m_spinEyeWidth, 0, wxEXPAND);

	grid->Add(new wxStaticText(this, wxID_ANY, "Eye height:"), 0, wxALIGN_CENTER_VERTICAL);
	m_spinEyeHeight = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, -1),
		wxSP_ARROW_KEYS, 0, kMaxEyeHeight, (std::max)(cfg.m_EyeHeight, 0));
	m_spinEyeHeight->SetToolTip(m_spinEyeWidth->GetToolTipText());
	grid->Add(m_spinEyeHeight, 0, wxEXPAND);

	for (wxSpinCtrl* spin : { m_spinEyeWidth, m_spinEyeHeight })
		spin->Bind(wxEVT_SPINCTRL, [this](wxSpinEvent&) { SelectMatchingPreset(); });

	main->Add(grid, 0, wxEXPAND | wxALL, 14);
	SelectMatchingPreset();
	UpdateEnabled();

	main->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 14);

	// ── OK / Cancel ───────────────────────────────────────────────────────────
	auto* btnSizer = new wxStdDialogButtonSizer();
	auto* btnOK = new wxButton(this, wxID_OK, "OK");
	auto* btnCancel = new wxButton(this, wxID_CANCEL, "Cancel");
	btnOK->SetDefault();
	btnSizer->AddButton(btnOK);
	btnSizer->AddButton(btnCancel);
	btnSizer->Realize();
	main->Add(btnSizer, 0, wxEXPAND | wxALL, 10);

	SetSizer(main);
	btnOK->Bind(wxEVT_BUTTON, &VrDialog::OnOK, this);

	Fit();
	SetMinSize(GetSize());
	Centre();
}

void VrDialog::OnPreset(wxCommandEvent&)
{
	const int selection = m_choicePreset->GetSelection();
	if (selection < 0 || selection >= (int)std::size(s_presets))
		return;   // "Custom": the sizes as they are
	m_spinEyeWidth->SetValue(s_presets[selection].eyeWidth);
	m_spinEyeHeight->SetValue(s_presets[selection].eyeHeight);
}

void VrDialog::SelectMatchingPreset()
{
	const int width = m_spinEyeWidth->GetValue();
	const int height = m_spinEyeHeight->GetValue();
	for (size_t i = 0; i < std::size(s_presets); i++)
	{
		if (s_presets[i].eyeWidth == width && s_presets[i].eyeHeight == height)
		{
			m_choicePreset->SetSelection((int)i);
			return;
		}
	}
	m_choicePreset->SetSelection((int)std::size(s_presets));   // "Custom"
}

void VrDialog::UpdateEnabled()
{
	const bool enabled = m_chkEnabled->GetValue();
	for (wxWindow* control : std::initializer_list<wxWindow*>{ m_choicePreset, m_spinEyeWidth, m_spinEyeHeight })
		control->Enable(enabled);
}

void VrDialog::OnOK(wxCommandEvent&)
{
	EndModal(wxID_OK);
}

Config::Settings::OpenXR VrDialog::GetSettings() const
{
	Config::Settings::OpenXR cfg = m_cfg;
	cfg.m_Enabled = m_chkEnabled->GetValue();
	cfg.m_EyeWidth = m_spinEyeWidth->GetValue();
	cfg.m_EyeHeight = m_spinEyeHeight->GetValue();
	// Both or neither: one of them 0 means the headset's shape
	if (cfg.m_EyeWidth == 0 || cfg.m_EyeHeight == 0)
		cfg.m_EyeWidth = cfg.m_EyeHeight = 0;
	return cfg;
}
