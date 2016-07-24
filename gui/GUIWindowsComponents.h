///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUIWINDOWSCOMPONENTS_H__
#define __GUIWINDOWSCOMPONENTS_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class GUIWindowsComponents
///////////////////////////////////////////////////////////////////////////////
class GUIWindowsComponents : public wxDialog 
{
	private:
	
	protected:
		wxTreeCtrl* m_treeCtrl1;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrl1;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void evtOnTreeItemGetTooltip( wxTreeEvent& event ) { event.Skip(); }
		virtual void evtOnTreeStateImageClick( wxTreeEvent& event ) { event.Skip(); }
		virtual void onOkClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GUIWindowsComponents( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Select Windows components to backup"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 746,672 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~GUIWindowsComponents();
	
};

#endif //__GUIWINDOWSCOMPONENTS_H__
