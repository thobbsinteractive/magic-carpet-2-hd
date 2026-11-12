#include "AnaglyphDialog.h"

const wxColour AnaglyphDialog::s_defaultLeftColour(0xFF, 0x00, 0x00);
const wxColour AnaglyphDialog::s_defaultRightColour(0x00, 0xA8, 0xA8);

// ── AnaglyphDialog ────────────────────────────────────────────────────────────
AnaglyphDialog::AnaglyphDialog(wxWindow* parent, const Config::Settings::Anaglyph& cfg)
	: wxDialog(parent, wxID_ANY, "Anaglyph 3D Settings",
		wxDefaultPosition, wxSize(400, 250),
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	m_cfg = cfg;

	auto* main = new wxBoxSizer(wxVERTICAL);

	auto* intro = new wxStaticText(this, wxID_ANY,
		"3D for red / cyan glasses. F11 in game turns it on and off.");
	main->Add(intro, 0, wxLEFT | wxRIGHT | wxTOP, 14);

	auto* grid = new wxFlexGridSizer(/*rows*/2, /*cols*/3, /*vgap*/10, /*hgap*/12);
	grid->AddGrowableCol(1, 1);

	// ── Lenses ────────────────────────────────────────────────────────────────
	m_pickLeftColour = AddLens(grid, "Left lens colour:", ParseLensColour(cfg.m_LeftColour, s_defaultLeftColour),
		"The colour of the glasses' left lens. Default red (#FF0000).", m_txtLeftColour);
	m_pickRightColour = AddLens(grid, "Right lens colour:", ParseLensColour(cfg.m_RightColour, s_defaultRightColour),
		"The colour of the glasses' right lens. Default cyan dimmed to 66% (#00A8A8), like the original stereo mode. "
		"#00FFFF for full strength cyan, #0000FF for glasses with a blue right lens.", m_txtRightColour);

	main->Add(grid, 1, wxEXPAND | wxALL, 14);

	auto* btnDefaults = new wxButton(this, wxID_ANY, "Default Colours");
	btnDefaults->SetToolTip("Red left, cyan dimmed to 66% right.");
	main->Add(btnDefaults, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, 14);
	btnDefaults->Bind(wxEVT_BUTTON, &AnaglyphDialog::OnDefaultColours, this);

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
	btnOK->Bind(wxEVT_BUTTON, &AnaglyphDialog::OnOK, this);

	Fit();
	Centre();
}

wxColourPickerCtrl* AnaglyphDialog::AddLens(wxFlexGridSizer* grid, const wxString& label, const wxColour& colour,
	const wxString& toolTip, wxTextCtrl*& hexBox)
{
	grid->Add(new wxStaticText(this, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);

	auto* picker = new wxColourPickerCtrl(this, wxID_ANY, colour);
	picker->SetToolTip(toolTip);
	grid->Add(picker, 0, wxEXPAND);

	// Wide enough for "#RRGGBB"; at most 7 characters
	auto* hex = new wxTextCtrl(this, wxID_ANY, ToHex(colour), wxDefaultPosition,
		wxSize(GetTextExtent("#WWWWWW").GetWidth() + 12, -1));
	hex->SetMaxLength(7);
	hex->SetToolTip("The colour as #RRGGBB. Type a value to set it.");
	grid->Add(hex, 0, wxALIGN_CENTER_VERTICAL);
	hexBox = hex;

	// The picker shows in the box; a whole "#RRGGBB" typed in the box sets the picker (anything else waits for more)
	picker->Bind(wxEVT_COLOURPICKER_CHANGED, [hex](wxColourPickerEvent& evt) {
		hex->ChangeValue(ToHex(evt.GetColour()));
		});
	hex->Bind(wxEVT_TEXT, [hex, picker](wxCommandEvent&) {
		const wxColour typed = ParseLensColour(hex->GetValue().ToStdString(), wxNullColour);
		if (typed.IsOk())
			picker->SetColour(typed);
		});
	// Leaving the box puts back the picker's colour, so a part typed value doesn't stay shown
	hex->Bind(wxEVT_KILL_FOCUS, [hex, picker](wxFocusEvent& evt) {
		hex->ChangeValue(ToHex(picker->GetColour()));
		evt.Skip();
		});

	return picker;
}

void AnaglyphDialog::OnDefaultColours(wxCommandEvent&)
{
	m_pickLeftColour->SetColour(s_defaultLeftColour);
	m_pickRightColour->SetColour(s_defaultRightColour);
	m_txtLeftColour->ChangeValue(ToHex(s_defaultLeftColour));
	m_txtRightColour->ChangeValue(ToHex(s_defaultRightColour));
}

void AnaglyphDialog::OnOK(wxCommandEvent&)
{
	EndModal(wxID_OK);
}

wxColour AnaglyphDialog::ParseLensColour(const std::string& text, const wxColour& fallback)
{
	const size_t start = (!text.empty() && text[0] == '#') ? 1 : 0;
	if (text.size() != start + 6 || text.find_first_not_of("0123456789abcdefABCDEF", start) != std::string::npos)
		return fallback;
	const unsigned long rgb = std::stoul(text.substr(start), nullptr, 16);
	return wxColour((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

// "#RRGGBB", as the game reads it
std::string AnaglyphDialog::ToHex(const wxColour& colour)
{
	return wxString::Format("#%02X%02X%02X", colour.Red(), colour.Green(), colour.Blue()).ToStdString();
}

Config::Settings::Anaglyph AnaglyphDialog::GetSettings() const
{
	Config::Settings::Anaglyph cfg = m_cfg;   // keeps the ghost reduction

	cfg.m_LeftColour = ToHex(m_pickLeftColour->GetColour());
	cfg.m_RightColour = ToHex(m_pickRightColour->GetColour());

	return cfg;
}
