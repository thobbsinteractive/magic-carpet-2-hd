#pragma once
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/artprov.h>
#include "PathHelpers.h"
#include "../remc2/portability/Config.h"

// ── HostDialog ───────────────────────────────────────────────────────────────
class HostDialog : public wxDialog
{
public:
	HostDialog(wxWindow* parent, const Config::Settings::Multiplayer& cfg);

	int      GetPort() const { return m_cfg.m_ServerPort; }

private:
	Config::Settings::Multiplayer m_cfg;
	wxSpinCtrl* m_ctrlPort = nullptr;

	void OnOK(wxCommandEvent&);
};

// ── JoinDialog ───────────────────────────────────────────────────────────────
class JoinDialog : public wxDialog
{
public:
	JoinDialog(wxWindow* parent, const Config::Settings::Multiplayer& cfg);

	wxString GetIP()   const { return m_cfg.m_ClientServerIp; }
	int      GetPort() const { return m_cfg.m_ClientServerPort; }

private:
	Config::Settings::Multiplayer m_cfg;

	wxTextCtrl* m_ctrlIP = nullptr;
	wxSpinCtrl* m_ctrlPort = nullptr;

	void OnOK(wxCommandEvent&);
};

// ── MultiplayerDialog ────────────────────────────────────────────────────────
class MultiplayerDialog : public wxDialog
{
public:
	MultiplayerDialog(wxWindow* parent, const Config::Settings::Multiplayer& cfg);
	Config::Settings::Multiplayer GetMultiplayer() const;

private:
	Config::Settings::Multiplayer m_cfg;
	
	void OnHost(wxCommandEvent&);
	void OnJoin(wxCommandEvent&);

	enum { ID_BTN_HOST = wxID_HIGHEST + 1, ID_BTN_JOIN };
};