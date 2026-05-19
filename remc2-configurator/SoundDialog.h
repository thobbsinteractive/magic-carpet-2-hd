#pragma once
#include "Globals.h"
#include "../remc2/portability/Config.h"

class SoundDialog : public wxDialog {
public:
	SoundDialog(wxWindow* parent, const Config::Sound& s);
	Config::Sound GetSettings() const;

private:
	wxTextCtrl* m_oggFolder = nullptr;
	wxCheckBox* m_oggMusicAlternative = nullptr;
	wxCheckBox* m_fixSpeedSound = nullptr;
	wxSpinCtrl* m_maxSimSounds = nullptr;
	wxTextCtrl* m_speechFolder = nullptr;
	wxCheckBox* m_autoShowObjectives = nullptr;
};