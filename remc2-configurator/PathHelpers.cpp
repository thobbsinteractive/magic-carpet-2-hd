#include "PathHelpers.h"

wxString GetExeDir()
{
	wxFileName exe(wxStandardPaths::Get().GetExecutablePath());
	return exe.GetPath();
}

wxString ToRelative(const wxString& absPath)
{
	wxFileName fn(absPath);
	fn.MakeRelativeTo(GetExeDir());
	return fn.GetFullPath();
}

wxString ToAbsolute(const wxString& path)
{
	wxFileName fn(path);
	if (fn.IsRelative())
		fn.MakeAbsolute(GetExeDir());
	return fn.GetFullPath();
}