/*************************************************************************
*    UrBackup - Client/Server backup system
*    Copyright (C) 2011  Martin Raiber
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef __GUI__
#define __GUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/choice.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class GUISettings
///////////////////////////////////////////////////////////////////////////////
class GUISettings : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText7;
		wxStaticText* m_staticText8;
		wxStaticText* m_staticText9;
		wxStaticText* m_staticText10;
		wxStaticText* m_staticText11;
		wxStaticText* m_staticText12;
		wxStaticText* m_staticText13;
		wxStaticText* m_staticText14;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText16;
		wxStaticText* m_staticText17;
		wxTextCtrl* m_textCtrl1;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrl2;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrl21;
		wxStaticText* m_staticText41;
		
		wxCheckBox* m_checkBox1;
		wxTextCtrl* m_textCtrl22;
		wxStaticText* m_staticText42;
		wxTextCtrl* m_textCtrl13;
		wxTextCtrl* m_textCtrl131;
		wxTextCtrl* m_textCtrl132;
		wxTextCtrl* m_textCtrl133;
		
		wxTextCtrl* m_textCtrl134;
		wxTextCtrl* m_textCtrl135;
		wxTextCtrl* m_textCtrl136;
		wxTextCtrl* m_textCtrl137;
		
		
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnDisableImageBackups( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOkClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAbortClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		GUISettings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Einstellungen"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 541,543 ), long style = wxDEFAULT_DIALOG_STYLE );
		~GUISettings();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GUILogfiles
///////////////////////////////////////////////////////////////////////////////
class GUILogfiles : public wxDialog 
{
	private:
	
	protected:
		wxListBox* m_listBox1;
		
		wxStaticText* m_staticText5;
		wxChoice* m_choice1;
		wxTextCtrl* m_textCtrl3;
		
		wxButton* m_button5;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnLogEntrySelect( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnLoglevelChange( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExitClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		GUILogfiles( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Logs"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 703,522 ), long style = wxDEFAULT_DIALOG_STYLE );
		~GUILogfiles();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GUIInfo
///////////////////////////////////////////////////////////////////////////////
class GUIInfo : public wxDialog 
{
	private:
	
	protected:
		
		
		wxStaticBitmap* m_bitmap1;
		
		wxStaticText* m_staticText21;
		wxStaticText* m_staticText22;
		wxStaticText* m_staticText23;
		wxStaticText* m_staticText24;
		wxStaticText* m_staticText25;
		
		
		wxButton* m_button4;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOKClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		GUIInfo( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Informationen"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 313,215 ), long style = wxDEFAULT_DIALOG_STYLE );
		~GUIInfo();
	
};

#endif //__GUI__
