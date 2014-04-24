#include "TranslationHelper.h"

wxString trans_1( wxString str, const wxString& ins1 )
{
	str.Replace(wxT("_1_"), ins1);
	return str;
}

wxString trans_2( wxString str, const wxString& ins1, const wxString& ins2 )
{
	str.Replace(wxT("_1_"), ins1);
	str.Replace(wxT("_2_"), ins2);
	return str;
}

