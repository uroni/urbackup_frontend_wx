#include "CbtStatus.h"
#include <wx/process.h>

extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;

CbtStatus::CbtStatus(wxWindow* parent)
	: GUICbtStatus(parent), wxProcess(wxPROCESS_REDIRECT), first_val(true)
{
	SetIcon(wxIcon(res_path + wxT("backup-ok.") + ico_ext, ico_type));

	long pid = wxExecute(wxT("\"") + res_path + wxT("urbctctl.exe\" status all"), wxEXEC_ASYNC, this);
	if (pid != 0)
	{
		Redirect();
		CloseOutput();
	}

	Start(100);

	m_textCtrl3->SetValue("Retrieving CBT status...");

	Show(true);
}

CbtStatus::~CbtStatus()
{
	Stop();
}

void CbtStatus::OnExitClick(wxCommandEvent & event)
{
	Stop();
	Close();
}

void CbtStatus::Notify(void)
{
	if (first_val)
	{
		first_val = false;
		m_textCtrl3->SetValue("");
	}

	addStream(GetInputStream());
	addStream(GetErrorStream());

	if ((GetErrorStream() == NULL || GetErrorStream()->Eof())
		&& (GetInputStream()==NULL || GetInputStream()->Eof()) )
	{
		Stop();
	}
}

void CbtStatus::OnTerminate(int pid, int status)
{
	Notify();
	Stop();
}

void CbtStatus::addStream(wxInputStream * input_stream)
{
	if (input_stream == NULL)
	{
		return;
	}

	while (input_stream->CanRead())
	{
		char buffer[1024];
		input_stream->Read(buffer, 1024);
		size_t read = input_stream->LastRead();

		if (read > 0)
		{
			wxString read_str = wxString::FromUTF8(buffer, read);
			m_textCtrl3->AppendText(read_str);
		}
	}
}
