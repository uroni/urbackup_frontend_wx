///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUISETUPWIZARD_H__
#define __GUISETUPWIZARD_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/wizard.h>
#include <wx/dynarray.h>
WX_DEFINE_ARRAY_PTR( wxWizardPageSimple*, WizardPages );

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class GUISetupWizard
///////////////////////////////////////////////////////////////////////////////
class GUISetupWizard : public wxWizard 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxRadioButton* m_radioBtn1;
		wxRadioButton* m_radioBtn2;
		wxRadioButton* m_radioBtn3;
		wxStaticText* m_staticText2;
		wxRadioButton* m_radioBtn4;
		wxRadioButton* m_radioBtn5;
		wxRadioButton* m_radioBtn6;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrl1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void wizardCancel( wxWizardEvent& event ) { event.Skip(); }
		virtual void wizardFinished( wxWizardEvent& event ) { event.Skip(); }
		virtual void wizardNext( wxWizardEvent& event ) { event.Skip(); }
		virtual void manualVolumeConfig( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GUISetupWizard( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Select files and volumes to backup"), const wxBitmap& bitmap = wxNullBitmap, const wxPoint& pos = wxDefaultPosition, long style = wxDEFAULT_DIALOG_STYLE );
		WizardPages m_pages;
		~GUISetupWizard();
	
};

#endif //__GUISETUPWIZARD_H__
