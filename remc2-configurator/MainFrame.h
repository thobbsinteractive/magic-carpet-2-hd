#pragma once
#include <wx/wx.h>

enum
{
	// menu items
	Minimal_Quit = wxID_EXIT,

	// it is important for the id corresponding to the "About" command to have
	// this standard value as otherwise it won't be handled properly under Mac
	// (where it is special and put into the "Apple" menu)
	Minimal_About = wxID_ABOUT
};

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

private:
	void OnExit(wxCommandEvent& event);
};