#pragma once
#include "Globals.h"

class GameFilesDialog : public wxDialog {
public:
	GameFilesDialog(wxWindow* parent);

	wxString GetGameFolderPath() const { return m_gameFolderCtrl->GetValue(); }
	wxString GetCdFolderPath()   const { return m_cdFolderCtrl->GetValue(); }

private:
	// Helper that builds one label + text + browse row and appends it to mainSizer
	wxBoxSizer* MakeFolderRow(wxPanel* panel, const wxString& label,
		wxTextCtrl*& outCtrl, int browseId);

	void OnBrowseGameFiles(wxCommandEvent& evt);
	void OnBrowseCdFiles(wxCommandEvent& evt);

	wxTextCtrl* m_gameFolderCtrl = nullptr;
	wxTextCtrl* m_cdFolderCtrl = nullptr;
};
