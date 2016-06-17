#pragma once

#include "gui/GUI.h"
#include <wx/process.h>
#include <wx/timer.h>

class CbtStatus : public GUICbtStatus, wxTimer, wxProcess
{
public:
	CbtStatus(wxWindow* parent);
	~CbtStatus();
protected:
	virtual void OnExitClick(wxCommandEvent& event);

	virtual void Notify(void);

	virtual void OnTerminate(int pid, int status);

	void addStream(wxInputStream* input_stream);

	bool first_val;
};