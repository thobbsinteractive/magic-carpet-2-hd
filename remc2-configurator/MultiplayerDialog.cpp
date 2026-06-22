#include "MultiplayerDialog.h"

// ── HostDialog ───────────────────────────────────────────────────────────────
HostDialog::HostDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, "Host Game",
		wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE)
{
	auto* sizer = new wxBoxSizer(wxVERTICAL);
	auto* grid = new wxFlexGridSizer(1, 2, 8, 8);
	grid->AddGrowableCol(1, 1);

	// Port
	grid->Add(new wxStaticText(this, wxID_ANY, "Port:"),
		0, wxALIGN_CENTER_VERTICAL);
	m_ctrlPort = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(180, -1),
		wxSP_ARROW_KEYS, 1024, 65535, 3030);
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
	m_port = m_ctrlPort->GetValue();
	EndModal(wxID_OK);
}

// ── JoinDialog ───────────────────────────────────────────────────────────────
JoinDialog::JoinDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, "Join Game",
		wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE)
{
	auto* sizer = new wxBoxSizer(wxVERTICAL);
	auto* grid = new wxFlexGridSizer(2, 2, 8, 8);
	grid->AddGrowableCol(1, 1);

	// Server IP
	grid->Add(new wxStaticText(this, wxID_ANY, "Server IP:"),
		0, wxALIGN_CENTER_VERTICAL);
	m_ctrlIP = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(180, -1));
	grid->Add(m_ctrlIP, 1, wxEXPAND);

	// Server Port
	grid->Add(new wxStaticText(this, wxID_ANY, "Port:"),
		0, wxALIGN_CENTER_VERTICAL);
	m_ctrlPort = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(180, -1),
		wxSP_ARROW_KEYS, 1024, 65535, 3030);
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
	m_ip = m_ctrlIP->GetValue().Trim();
	m_port = m_ctrlPort->GetValue();

	if (m_ip.IsEmpty())
	{
		wxMessageBox("Please enter the server IP address.",
			"Validation Error", wxOK | wxICON_WARNING, this);
		return;
	}
	EndModal(wxID_OK);
}

// ── MultiplayerDialog ────────────────────────────────────────────────────────
MultiplayerDialog::MultiplayerDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, "Multiplayer",
		wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE)
{
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
	HostDialog dlg(this);
	if (dlg.ShowModal() == wxID_OK)
	{
		std::string port = std::to_string(dlg.GetPort());
		if (LaunchGame("--mode_test_network server " + port + " client 127.0.0.1 " + port + " " + port))
			EndModal(wxID_OK);
	}
}

void MultiplayerDialog::OnJoin(wxCommandEvent&)
{
	JoinDialog dlg(this);
	if (dlg.ShowModal() == wxID_OK)
	{
		std::string port = std::to_string(dlg.GetPort());
		wxString ip = dlg.GetIP();
		if (LaunchGame("--mode_test_network client " + ip + " " + port + " " + port))
			EndModal(wxID_OK);
	}
}