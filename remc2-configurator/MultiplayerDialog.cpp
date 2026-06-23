#include "MultiplayerDialog.h"

// ── HostDialog ───────────────────────────────────────────────────────────────
HostDialog::HostDialog(wxWindow* parent, const Config::Settings::Multiplayer& cfg)
	: wxDialog(parent, wxID_ANY, "Host Game",
		wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE)
{
	m_cfg = cfg;
	auto* sizer = new wxBoxSizer(wxVERTICAL);
	auto* grid = new wxFlexGridSizer(1, 2, 8, 8);
	grid->AddGrowableCol(1, 1);

	// Port
	grid->Add(new wxStaticText(this, wxID_ANY, "Port:"),
		0, wxALIGN_CENTER_VERTICAL);
	m_ctrlPort = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(180, -1),
		wxSP_ARROW_KEYS, 1024, 65535, 3030);
	m_ctrlPort->SetValue(cfg.m_ServerPort);
	grid->Add(m_ctrlPort, 1, wxEXPAND);

	sizer->Add(grid, 0, wxEXPAND | wxALL, 12);
	sizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 8);

	auto* btnSizer = new wxStdDialogButtonSizer();
	auto* btnOK = new wxButton(this, wxID_OK, "Host");
	btnSizer->AddButton(btnOK);
	btnSizer->AddButton(new wxButton(this, wxID_CANCEL));
	btnSizer->Realize();
	sizer->Add(btnSizer, 0, wxEXPAND | wxALL, 8);

	SetSizerAndFit(sizer);
	Centre();

	btnOK->Bind(wxEVT_BUTTON, &HostDialog::OnOK, this);
}

void HostDialog::OnOK(wxCommandEvent&)
{
	m_cfg.m_ServerPort = m_ctrlPort->GetValue();
	EndModal(wxID_OK);
}

// ── JoinDialog ───────────────────────────────────────────────────────────────
JoinDialog::JoinDialog(wxWindow* parent, const Config::Settings::Multiplayer& cfg)
	: wxDialog(parent, wxID_ANY, "Join Game",
		wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE)
{
	m_cfg = cfg;
	auto* sizer = new wxBoxSizer(wxVERTICAL);
	auto* grid = new wxFlexGridSizer(2, 2, 8, 8);
	grid->AddGrowableCol(1, 1);

	// Server IP
	grid->Add(new wxStaticText(this, wxID_ANY, "Server IP:"),
		0, wxALIGN_CENTER_VERTICAL);
	m_ctrlIP = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(180, -1));
	m_ctrlIP->SetValue(cfg.m_ClientServerIp);
	grid->Add(m_ctrlIP, 1, wxEXPAND);

	// Server Port
	grid->Add(new wxStaticText(this, wxID_ANY, "Port:"),
		0, wxALIGN_CENTER_VERTICAL);
	m_ctrlPort = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(180, -1),
		wxSP_ARROW_KEYS, 1024, 65535, 3030);
	m_ctrlPort->SetValue(cfg.m_ClientServerPort);
	grid->Add(m_ctrlPort, 1, wxEXPAND);

	sizer->Add(grid, 0, wxEXPAND | wxALL, 12);
	sizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 8);

	auto* btnSizer = new wxStdDialogButtonSizer();
	auto* btnOK = new wxButton(this, wxID_OK, "Join");
	btnSizer->AddButton(btnOK);
	btnSizer->AddButton(new wxButton(this, wxID_CANCEL));
	btnSizer->Realize();
	sizer->Add(btnSizer, 0, wxEXPAND | wxALL, 8);

	SetSizerAndFit(sizer);
	Centre();

	btnOK->Bind(wxEVT_BUTTON, &JoinDialog::OnOK, this);
}

void JoinDialog::OnOK(wxCommandEvent& event)
{
	m_cfg.m_ClientServerIp = m_ctrlIP->GetValue().Trim();
	m_cfg.m_ClientServerPort = m_ctrlPort->GetValue();

	if (m_ctrlIP->GetValue().Trim().IsEmpty())
	{
		wxMessageBox("Please enter the server IP address.",
			"Validation Error", wxOK | wxICON_WARNING, this);
		return;
	}
	EndModal(wxID_OK);
}

// ── MultiplayerDialog ────────────────────────────────────────────────────────
MultiplayerDialog::MultiplayerDialog(wxWindow* parent, const Config::Settings::Multiplayer& cfg)
	: wxDialog(parent, wxID_ANY, "Multiplayer",
		wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE)
{
	m_cfg = cfg;
	auto* sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(new wxStaticText(this, wxID_ANY, "Choose a multiplayer mode:"),
		0, wxALL, 12);

	auto* btnSizer = new wxBoxSizer(wxHORIZONTAL);

	auto* btnHost = new wxButton(this, ID_BTN_HOST, "Host");
	btnHost->SetMinSize(wxSize(100, 40));
	auto* btnJoin = new wxButton(this, ID_BTN_JOIN, "Join");
	btnJoin->SetMinSize(wxSize(100, 40));

	btnSizer->Add(btnHost, 0, wxRIGHT, 8);
	btnSizer->Add(btnJoin, 0);

	sizer->Add(btnSizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 12);

	sizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 8);

	auto* closeSizer = new wxStdDialogButtonSizer();
	closeSizer->AddButton(new wxButton(this, wxID_CANCEL, "Close"));
	closeSizer->Realize();
	sizer->Add(closeSizer, 0, wxEXPAND | wxALL, 8);

	SetSizerAndFit(sizer);
	Centre();

	Bind(wxEVT_BUTTON, &MultiplayerDialog::OnHost, this, ID_BTN_HOST);
	Bind(wxEVT_BUTTON, &MultiplayerDialog::OnJoin, this, ID_BTN_JOIN);
}

void MultiplayerDialog::OnHost(wxCommandEvent&)
{
	HostDialog dlg(this, m_cfg);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_cfg.m_ServerPort = dlg.GetPort();
		EndModal(wxID_NETWORK);
	}
}

void MultiplayerDialog::OnJoin(wxCommandEvent&)
{
	JoinDialog dlg(this, m_cfg);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_cfg.m_ClientServerPort = dlg.GetPort();
		m_cfg.m_ClientServerIp = dlg.GetIP();
		EndModal(wxID_ADD);
	}
}

Config::Settings::Multiplayer MultiplayerDialog::GetMultiplayer() const
{
	return m_cfg;
}
