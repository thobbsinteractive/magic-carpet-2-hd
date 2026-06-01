#include "ControlsDialog.h"

ConfigToSdlScancode ControlsDialog::m_configToSdlScancode;

ControlsDialog::ControlsDialog(wxWindow* parent, const Config::Settings::Controls& cfg)
	: wxDialog(parent, wxID_ANY, "Controls Settings",
		wxDefaultPosition, wxSize(264, 500))
{
	m_cfg = cfg;
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	m_notebook = new wxNotebook(this, wxID_ANY);

	m_mousePage = new wxPanel(m_notebook, wxID_ANY);
	m_keyboardPage = new wxPanel(m_notebook, wxID_ANY);
	m_joystickPage = new wxPanel(m_notebook, wxID_ANY);

	m_notebook->AddPage(m_mousePage, "Mouse");
	m_notebook->AddPage(m_keyboardPage, "Keyboard");
	m_notebook->AddPage(m_joystickPage, "Joystick");

	CreateMousePage();
	CreateKeyboardPage();
	CreateJoystickPage();

	mainSizer->Add(m_notebook, 1, wxEXPAND | wxALL, 5);
	SetSizerAndFit(mainSizer);
	Centre(wxBOTH);
}

void ControlsDialog::CreateMousePage()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	// -- Invert Axes --
	wxStaticBoxSizer* axesSizer = new wxStaticBoxSizer(wxVERTICAL, m_mousePage, "Axes");
	m_invertYAxis = new wxCheckBox(m_mousePage, wxID_ANY, "Invert Y Axis");
	m_invertYAxis->SetToolTip("True By default, the Y axis is inverted in the original game");
	m_invertXAxis = new wxCheckBox(m_mousePage, wxID_ANY, "Invert X Axis");
	m_invertXAxis->SetToolTip("By default, the X axis is not inverted");
	m_invertYAxis->SetValue(m_cfg.m_Mouse.m_InvertYAxis);
	m_invertXAxis->SetValue(m_cfg.m_Mouse.m_InvertXAxis);
	axesSizer->Add(m_invertYAxis, 0, wxALL, 5);
	axesSizer->Add(m_invertXAxis, 0, wxALL, 5);
	sizer->Add(axesSizer, 0, wxEXPAND | wxALL, 5);

	// -- Mouse Scale --
	wxStaticBoxSizer* scaleSizer = new wxStaticBoxSizer(wxVERTICAL, m_mousePage, "Sensitivity (1.0 = normal, higher = slower)");

	wxBoxSizer* scaleXRow = new wxBoxSizer(wxHORIZONTAL);
	scaleXRow->Add(new wxStaticText(m_mousePage, wxID_ANY, "Scale X:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
	m_mouseScaleX = new wxSpinCtrlDouble(m_mousePage, wxID_ANY);
	m_mouseScaleX->SetRange(0.1, 5.0);
	m_mouseScaleX->SetIncrement(0.1);
	m_mouseScaleX->SetValue(m_cfg.m_Mouse.m_MouseScaleX);
	m_mouseScaleX->SetToolTip("1.0 = normal, 1.5 = slower, 0.5 faster etc... (Note: Effects joystick steering too)");
	scaleXRow->Add(m_mouseScaleX, 1, wxEXPAND);

	wxBoxSizer* scaleYRow = new wxBoxSizer(wxHORIZONTAL);
	scaleYRow->Add(new wxStaticText(m_mousePage, wxID_ANY, "Scale Y:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
	m_mouseScaleY = new wxSpinCtrlDouble(m_mousePage, wxID_ANY);
	m_mouseScaleY->SetRange(0.1, 5.0);
	m_mouseScaleY->SetIncrement(0.1);
	m_mouseScaleY->SetValue(m_cfg.m_Mouse.m_MouseScaleY);
	m_mouseScaleY->SetToolTip("1.0 = normal, 1.5 = slower, 0.5 faster etc... (Note: Effects joystick steering too)");
	scaleYRow->Add(m_mouseScaleY, 1, wxEXPAND);

	scaleSizer->Add(scaleXRow, 0, wxEXPAND | wxALL, 5);
	scaleSizer->Add(scaleYRow, 0, wxEXPAND | wxALL, 5);
	sizer->Add(scaleSizer, 0, wxEXPAND | wxALL, 5);

	// -- Mouse Buttons --
	wxStaticBoxSizer* btnSizer = new wxStaticBoxSizer(wxVERTICAL, m_mousePage, "Mouse Buttons");

	const wxString buttonChoices[] = { "0 - Undefined", "1 - Left", "2 - Middle", "3 - Right", "4 - Mouse 4", "5 - Mouse 5" };
	const int numChoices = WXSIZEOF(buttonChoices);

	auto addButtonRow = [&](const wxString& label, wxChoice*& ctrl, int value)
		{
			wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
			row->Add(new wxStaticText(m_mousePage, wxID_ANY, label), 1, wxALIGN_CENTER_VERTICAL);
			ctrl = new wxChoice(m_mousePage, wxID_ANY, wxDefaultPosition, wxDefaultSize, numChoices, buttonChoices);
			ctrl->SetSelection(value); // value maps directly to index (0=Undefined, 1=Left, etc.)
			row->Add(ctrl, 1, wxEXPAND);
			btnSizer->Add(row, 0, wxEXPAND | wxALL, 5);
		};

	addButtonRow("Fire Spell Left:", m_spellLeft, m_cfg.m_Mouse.m_SpellLeft);
	addButtonRow("Fire Spell Right:", m_spellRight, m_cfg.m_Mouse.m_SpellRight);
	addButtonRow("Open Map:", m_map, m_cfg.m_Mouse.m_Map);
	addButtonRow("Open Spell Menu:", m_spellMenu, m_cfg.m_Mouse.m_SpellMenu);
	addButtonRow("Spell Menu Mark:", m_spellMenuMark, m_cfg.m_Mouse.m_SpellMenuMark);

	sizer->Add(btnSizer, 0, wxEXPAND | wxALL, 5);

	// -- Misc --
	wxStaticBoxSizer* miscSizer = new wxStaticBoxSizer(wxVERTICAL, m_mousePage, "Misc");
	m_disableLRButtonsMenuOpen = new wxCheckBox(m_mousePage, wxID_ANY, "Disable L+R Buttons Opening Menu");
	m_disableLRButtonsMenuOpen->SetValue(m_cfg.m_Mouse.m_DisableLRButtonsMenuOpen);
	m_disableLRButtonsMenuOpen->SetToolTip("When enabled, stops the two fire buttons opening the map screen");
	miscSizer->Add(m_disableLRButtonsMenuOpen, 0, wxALL, 5);
	sizer->Add(miscSizer, 0, wxEXPAND | wxALL, 5);

	m_mousePage->SetSizer(sizer);
}

void ControlsDialog::CreateKeyboardPage()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	//
	// Layout presets
	//
	wxStaticBoxSizer* presetSizer =
		new wxStaticBoxSizer(
			wxVERTICAL,
			m_keyboardPage,
			"Keyboard Layout");

	wxBoxSizer* buttonRow = new wxBoxSizer(wxHORIZONTAL);

	m_modernButton =
		new wxButton(
			m_keyboardPage,
			wxID_ANY,
			"Modern");

	m_classicButton =
		new wxButton(
			m_keyboardPage,
			wxID_ANY,
			"Classic");

	buttonRow->Add(
		m_modernButton,
		1,
		wxEXPAND | wxRIGHT,
		5);

	buttonRow->Add(
		m_classicButton,
		1,
		wxEXPAND);

	presetSizer->Add(
		buttonRow,
		0,
		wxEXPAND | wxALL,
		5);

	m_modernButton->Bind(
		wxEVT_BUTTON,
		&ControlsDialog::OnModernLayout,
		this);

	m_classicButton->Bind(
		wxEVT_BUTTON,
		&ControlsDialog::OnClassicLayout,
		this);

	sizer->Add(
		presetSizer,
		0,
		wxEXPAND | wxALL,
		5);

	//
	// Key bindings
	//
	wxStaticBoxSizer* bindingsSizer =
		new wxStaticBoxSizer(
			wxVERTICAL,
			m_keyboardPage,
			"Key Bindings");

	const wxArrayString keyChoices = GetAvailableKeys();

	auto addKeyRow =
		[&](const wxString& label,
			wxChoice*& ctrl)
		{
			wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);

			row->Add(
				new wxStaticText(
					m_keyboardPage,
					wxID_ANY,
					label),
				1,
				wxALIGN_CENTER_VERTICAL);

			ctrl = new wxChoice(
				m_keyboardPage,
				wxID_ANY,
				wxDefaultPosition,
				wxDefaultSize,
				keyChoices);

			row->Add(
				ctrl,
				1,
				wxEXPAND);

			bindingsSizer->Add(
				row,
				0,
				wxEXPAND | wxALL,
				5);
		};

	addKeyRow("Forward:", m_forwardKey);
	addKeyRow("Backwards:", m_backwardsKey);
	addKeyRow("Left:", m_leftKey);
	addKeyRow("Right:", m_rightKey);
	addKeyRow("Map:", m_mapKey);
	addKeyRow("Spell Menu:", m_spellMenuKey);
	addKeyRow("Mark:", m_spellMenuMarkKey);

	sizer->Add(
		bindingsSizer,
		1,
		wxEXPAND | wxALL,
		5);

	m_keyboardPage->SetSizer(sizer);
}

void ControlsDialog::OnModernLayout(wxCommandEvent&)
{
	LoadKeyboardLayout("Modern");
}

void ControlsDialog::OnClassicLayout(wxCommandEvent&)
{
	LoadKeyboardLayout("Classic");
}

void ControlsDialog::CreateJoystickPage()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	// Add joystick controls here
	m_joystickPage->SetSizer(sizer);
}

void ControlsDialog::LoadKeyboardLayout(const wxString& layoutName)
{
	if (layoutName.ToStdString() == "Modern")
	{
		SetChoiceValue(m_forwardKey, "W");
		SetChoiceValue(m_backwardsKey, "S");
		SetChoiceValue(m_leftKey, "A");
		SetChoiceValue(m_rightKey, "D");
		SetChoiceValue(m_mapKey, "TAB");
		SetChoiceValue(m_spellMenuKey, "LCTRL");
		SetChoiceValue(m_spellMenuMarkKey, "LSHIFT");
		return;
	}
	
	if (layoutName.ToStdString() == "Classic")
	{
		SetChoiceValue(m_forwardKey, "UP");
		SetChoiceValue(m_backwardsKey, "DOWN");
		SetChoiceValue(m_leftKey, "LEFT");
		SetChoiceValue(m_rightKey, "RIGHT");
		SetChoiceValue(m_mapKey, "RETURN");
		SetChoiceValue(m_spellMenuKey, "RCTRL");
		SetChoiceValue(m_spellMenuMarkKey, "RSHIFT");
		return;
	}
}

void ControlsDialog::SetChoiceValue(wxChoice* choice, const wxString& value)
{
	int idx = choice->FindString(value);

	if (idx != wxNOT_FOUND)
		choice->SetSelection(idx);
}


wxArrayString ControlsDialog::GetAvailableKeys() const
{
	wxArrayString keys;

	for (const auto& [name, scancode] : m_configToSdlScancode.GetScancodeMap())
	{
		if (!name.empty())
			keys.Add(wxString(name));
	}

	keys.Sort();

	return keys;
}

Config::Settings::Controls ControlsDialog::GetSettings() const
{
	Config::Settings::Controls cfg = m_cfg;

	cfg.m_Mouse.m_InvertXAxis = m_invertXAxis->GetValue();
	cfg.m_Mouse.m_InvertYAxis = m_invertYAxis->GetValue();
	cfg.m_Mouse.m_MouseScaleX = m_mouseScaleX->GetValue();
	cfg.m_Mouse.m_MouseScaleY = m_mouseScaleY->GetValue();
	cfg.m_Mouse.m_SpellLeft = m_spellLeft->GetSelection();
	cfg.m_Mouse.m_SpellRight = m_spellRight->GetSelection();
	cfg.m_Mouse.m_Map = m_map->GetSelection();
	cfg.m_Mouse.m_SpellMenu = m_spellMenu->GetSelection();
	cfg.m_Mouse.m_SpellMenuMark = m_spellMenuMark->GetSelection();
	cfg.m_Mouse.m_DisableLRButtonsMenuOpen = m_disableLRButtonsMenuOpen->GetValue();

	cfg.m_Keyboard.m_Forward = m_forwardKey->GetSelection();
	cfg.m_Keyboard.m_Backwards = m_backwardsKey->GetSelection();
	cfg.m_Keyboard.m_Left = m_leftKey->GetSelection();
	cfg.m_Keyboard.m_Right = m_rightKey->GetSelection();
	cfg.m_Keyboard.m_Map = m_mapKey->GetSelection();
	cfg.m_Keyboard.m_SpellMenu = m_spellMenu->GetSelection();
	cfg.m_Keyboard.m_SpellMenuMark = m_spellMenuMark->GetSelection();

	return cfg;
}