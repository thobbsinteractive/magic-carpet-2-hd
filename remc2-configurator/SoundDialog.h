#pragma once
#include "Globals.h"

struct SoundSettings {
	wxString oggFolder = "music-ogg";
	bool     oggMusicAlternative = false;
	bool     fixSpeedSound = false;
	int      maxSimultaneousSounds = 20;
	wxString speechFolder = "CD_Files/SPEECH";
	bool     autoShowObjectivesForForeignLang = true;
};

class SoundDialog : public wxDialog {
public:
	SoundDialog(wxWindow* parent, const SoundSettings& s);
	SoundSettings GetSettings() const;

private:
	wxTextCtrl* m_oggFolder = nullptr;
	wxCheckBox* m_oggMusicAlternative = nullptr;
	wxCheckBox* m_fixSpeedSound = nullptr;
	wxSpinCtrl* m_maxSimSounds = nullptr;
	wxTextCtrl* m_speechFolder = nullptr;
	wxCheckBox* m_autoShowObjectives = nullptr;
};