#include "ControlsDialog.h"

ControlsDialog::ControlsDialog(wxWindow* parent, const Config::Settings::Controls& cfg)
	: wxDialog(parent, wxID_ANY, "Controls Settings",
		wxDefaultPosition, wxSize(256, 256))
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

	// -- Mouse Buttons (1=Left, 2=Middle, 3=Right, 0=Undefined) --
	wxStaticBoxSizer* btnSizer = new wxStaticBoxSizer(wxVERTICAL, m_mousePage, "Mouse Buttons (1=Left, 2=Middle, 3=Right, 0=Undefined)");

	auto addButtonRow = [&](const wxString& label, wxSpinCtrl*& ctrl, int value)
		{
			wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
			row->Add(new wxStaticText(m_mousePage, wxID_ANY, label), 1, wxALIGN_CENTER_VERTICAL);
			ctrl = new wxSpinCtrl(m_mousePage, wxID_ANY);
			ctrl->SetRange(0, 5);
			ctrl->SetValue(value);
			row->Add(ctrl, 0);
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
	// Add keyboard controls here
	m_keyboardPage->SetSizer(sizer);
}

void ControlsDialog::CreateJoystickPage()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	// Add joystick controls here
	m_joystickPage->SetSizer(sizer);
}

Config::Settings::Controls ControlsDialog::GetSettings() const
{
	Config::Settings::Controls cfg = m_cfg;

	cfg.m_Mouse.m_InvertXAxis = m_invertYAxis->GetValue();
	cfg.m_Mouse.m_InvertYAxis = m_invertYAxis->GetValue();
	cfg.m_Mouse.m_MouseScaleX = m_mouseScaleX->GetValue();
	cfg.m_Mouse.m_MouseScaleY = m_mouseScaleY->GetValue();
	cfg.m_Mouse.m_SpellLeft = m_spellLeft->GetValue();
	cfg.m_Mouse.m_SpellRight = m_spellRight->GetValue();
	cfg.m_Mouse.m_Map = m_map->GetValue();
	cfg.m_Mouse.m_SpellMenu = m_spellMenu->GetValue();
	cfg.m_Mouse.m_SpellMenuMark = m_spellMenuMark->GetValue();
	cfg.m_Mouse.m_DisableLRButtonsMenuOpen = m_disableLRButtonsMenuOpen->GetValue();

	return cfg;
}