#pragma once
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/artprov.h>

// ── HostDialog ───────────────────────────────────────────────────────────────
class HostDialog : public wxDialog
{
public:
	HostDialog(wxWindow* parent);

	int      GetPort() const { return m_port; }

private:
	int      m_port = 3030;
	wxSpinCtrl* m_ctrlPort = nullptr;

	void OnOK(wxCommandEvent&);
};

// ── JoinDialog ───────────────────────────────────────────────────────────────
class JoinDialog : public wxDialog
{
public:
	JoinDialog(wxWindow* parent);

	wxString GetIP()   const { return m_ip; }
	int      GetPort() const { return m_port; }

private:
	wxString m_ip;
	int      m_port = 3030;

	wxTextCtrl* m_ctrlIP = nullptr;
	wxSpinCtrl* m_ctrlPort = nullptr;

	void OnOK(wxCommandEvent&);
};

// ── MultiplayerDialog ────────────────────────────────────────────────────────
class MultiplayerDialog : public wxDialog
{
public:
	MultiplayerDialog(wxWindow* parent);

private:
	void OnHost(wxCommandEvent&);
	void OnJoin(wxCommandEvent&);

	enum { ID_BTN_HOST = wxID_HIGHEST + 1, ID_BTN_JOIN };
};