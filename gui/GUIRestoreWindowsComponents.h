///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUIRESTOREWINDOWSCOMPONENTS_H__
#define __GUIRESTOREWINDOWSCOMPONENTS_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class GUISelectRestoreComponents
///////////////////////////////////////////////////////////////////////////////
class GUISelectRestoreComponents : public wxDialog 
{
	private:
	
	protected:
		wxListBox* m_listBox1;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onListBoxSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void evtOnTreeStateImageClick( wxTreeEvent& event ) { event.Skip(); }
		virtual void onStartRestore( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxTreeCtrl* m_treeCtrl1;
		
		GUISelectRestoreComponents( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Select components to restore"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 962,572 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~GUISelectRestoreComponents();
	
};

#endif //__GUIRESTOREWINDOWSCOMPONENTS_H__
