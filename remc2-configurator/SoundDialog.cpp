#include "SoundDialog.h"

SoundDialog::SoundDialog(wxWindow* parent, const SoundSettings& s)
	: wxDialog(parent, wxID_ANY, "Sound Settings",
		wxDefaultPosition, wxSize(420, 340),
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);

	// ── Controls ────────────────────────────────────────────────────────────
	m_hqSound = new wxCheckBox(panel, wxID_ANY, "High Quality Sound");
	m_oggMusic = new wxCheckBox(panel, wxID_ANY, "OGG Music");
	m_oggMusicAlternative = new wxCheckBox(panel, wxID_ANY, "OGG Music Alternative");
	m_fixSpeedSound = new wxCheckBox(panel, wxID_ANY, "Fix Speed Sound");
	m_autoShowObjectives = new wxCheckBox(panel, wxID_ANY,
		"Auto Show Objectives for Foreign Languages");

	m_hqSound->SetValue(s.hqSound);
	m_oggMusic->SetValue(s.oggMusic);
	m_oggMusicAlternative->SetValue(s.oggMusicAlternative);
	m_fixSpeedSound->SetValue(s.fixSpeedSound);
	m_autoShowObjectives->SetValue(s.autoShowObjectivesForForeignLang);

	// OGG Folder row
	auto* oggFolderLabel = new wxStaticText(panel, wxID_ANY, "OGG Folder:");
	m_oggFolder = new wxTextCtrl(panel, wxID_ANY, s.oggFolder);

	wxBoxSizer* oggRow = new wxBoxSizer(wxHORIZONTAL);
	oggRow->Add(oggFolderLabel, wxSizerFlags(0).CentreVertical().Border(wxRIGHT, 6));
	oggRow->Add(m_oggFolder, wxSizerFlags(1).Expand());

	// Max Simultaneous Sounds row
	auto* maxLabel = new wxStaticText(panel, wxID_ANY, "Max Simultaneous Sounds:");
	m_maxSimSounds = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS, 8, 128, s.maxSimultaneousSounds);

	wxBoxSizer* maxRow = new wxBoxSizer(wxHORIZONTAL);
	maxRow->Add(maxLabel, wxSizerFlags(0).CentreVertical().Border(wxRIGHT, 6));
	maxRow->Add(m_maxSimSounds, wxSizerFlags(0));

	// Speech Folder row
	auto* speechLabel = new wxStaticText(panel, wxID_ANY, "Speech Folder:");
	m_speechFolder = new wxTextCtrl(panel, wxID_ANY, s.speechFolder);

	wxBoxSizer* speechRow = new wxBoxSizer(wxHORIZONTAL);
	speechRow->Add(speechLabel, wxSizerFlags(0).CentreVertical().Border(wxRIGHT, 6));
	speechRow->Add(m_speechFolder, wxSizerFlags(1).Expand());

	// ── OK / Cancel ──────────────────────────────────────────────────────────
	wxStdDialogButtonSizer* btnSizer = new wxStdDialogButtonSizer();
	btnSizer->AddButton(new wxButton(panel, wxID_OK));
	btnSizer->AddButton(new wxButton(panel, wxID_CANCEL));
	btnSizer->Realize();

	// ── Main sizer ───────────────────────────────────────────────────────────
	const wxSizerFlags rowFlags = wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT | wxTOP, 12);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(m_hqSound, rowFlags);
	mainSizer->Add(m_oggMusic, rowFlags);
	mainSizer->Add(oggRow, rowFlags);
	mainSizer->Add(m_oggMusicAlternative, rowFlags);
	mainSizer->Add(m_fixSpeedSound, rowFlags);
	mainSizer->Add(maxRow, rowFlags);
	mainSizer->Add(speechRow, rowFlags);
	mainSizer->Add(m_autoShowObjectives, rowFlags);
	mainSizer->AddStretchSpacer(1);
	mainSizer->Add(btnSizer, wxSizerFlags(0).Expand().Border(wxALL, 8));

	panel->SetSizer(mainSizer);

	wxBoxSizer* dialogSizer = new wxBoxSizer(wxVERTICAL);
	dialogSizer->Add(panel, wxSizerFlags(1).Expand());
	SetSizer(dialogSizer);

	Centre();
}

SoundSettings SoundDialog::GetSettings() const
{
	SoundSettings s;
	s.hqSound = m_hqSound->GetValue();
	s.oggMusic = m_oggMusic->GetValue();
	s.oggFolder = m_oggFolder->GetValue();
	s.oggMusicAlternative = m_oggMusicAlternative->GetValue();
	s.fixSpeedSound = m_fixSpeedSound->GetValue();
	s.maxSimultaneousSounds = m_maxSimSounds->GetValue();
	s.speechFolder = m_speechFolder->GetValue();
	s.autoShowObjectivesForForeignLang = m_autoShowObjectives->GetValue();
	return s;
}