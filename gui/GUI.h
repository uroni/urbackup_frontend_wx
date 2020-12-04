///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUI__
#define __GUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/choice.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/gauge.h>
#include <wx/statline.h>
#include <wx/bmpbuttn.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////

#undef _
#define _(s) wxGetTranslation(wxT(s))

#undef _
#define _(s) wxGetTranslation(wxT(s))

///////////////////////////////////////////////////////////////////////////////
/// Class GUISettings
///////////////////////////////////////////////////////////////////////////////
class GUISettings : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrl1;
		wxStaticText* m_staticText2;
		//wxButton* m_bitmapButton1;
		wxBitmapButton* m_bitmapButton1;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrl2;
		wxBitmapButton* m_bitmapButton2;
		wxStaticText* m_staticText4;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrl21;
		wxBitmapButton* m_bitmapButton21;
		wxStaticText* m_staticText41;
		
		wxCheckBox* m_checkBox1;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textCtrl22;
		wxBitmapButton* m_bitmapButton22;
		wxStaticText* m_staticText42;
		wxStaticText* m_staticText30;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCtrl13;
		wxBitmapButton* m_bitmapButton13;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textCtrl131;
		wxBitmapButton* m_bitmapButton131;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textCtrl132;
		wxBitmapButton* m_bitmapButton132;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_textCtrl133;
		wxBitmapButton* m_bitmapButton133;
		wxStaticText* m_staticText29;
		wxStaticText* m_staticText14;
		wxTextCtrl* m_textCtrl134;
		wxBitmapButton* m_bitmapButton134;
		wxStaticText* m_staticText15;
		wxTextCtrl* m_textCtrl135;
		wxBitmapButton* m_bitmapButton135;
		wxStaticText* m_staticText16;
		wxTextCtrl* m_textCtrl136;
		wxBitmapButton* m_bitmapButton136;
		wxStaticText* m_staticText17;
		wxTextCtrl* m_textCtrl137;
		wxBitmapButton* m_bitmapButton137;
		wxStaticText* m_staticText28;
		wxStaticText* m_staticText25;
		wxTextCtrl* m_textCtrl15;
		wxStaticText* m_staticText26;
		wxTextCtrl* m_textCtrl16;
		wxBitmapButton* m_bitmapButton16;
		wxStaticText* m_staticText261;
		wxTextCtrl* m_textCtrl161;
		wxBitmapButton* m_bitmapButton161;
		wxStaticText* m_staticText27;
		wxTextCtrl* m_textCtrl17;
		wxBitmapButton* m_bitmapButton17;
		wxStaticText* m_staticText281;
		wxTextCtrl* m_textCtrl19;
		wxBitmapButton* m_bitmapButton19;
		wxStaticText* m_staticText291;
		wxTextCtrl* m_textCtrl23;
		wxBitmapButton* m_bitmapButton23;
		wxStaticText* m_staticText301;
		wxStaticText* m_staticTextLocalSpeed;
		wxTextCtrl* m_textCtrlLocalSpeed;
		wxBitmapButton* m_bitmapButtonLocalSpeed;
		wxStaticText* m_staticTextLocalSpeedUnit;

		wxStaticText* m_staticTextInternetEnabled;
		wxCheckBox* m_checkBoxInternetEnabled;
		wxStaticText* m_staticInternetServer;
		wxTextCtrl* m_textCtrlInternetServer;
		wxStaticText* m_staticInternetServerPort;
		wxTextCtrl* m_textCtrlInternetServerPort;
		wxStaticText* m_staticInternetServerProxy;
		wxTextCtrl* m_textCtrlInternetServerProxy;
		wxStaticText* m_staticInternetServerAuthkey;
		wxTextCtrl* m_textCtrlInternetServerAuthkey;
		wxStaticText* m_staticTextInternetFullFile;
		wxCheckBox* m_checkBoxInternetFullFile;
		wxBitmapButton* m_bitmapButtonInternetFullFile;
		wxStaticText* m_staticTextInternetImage;
		wxCheckBox* m_checkBoxInternetImage;
		wxBitmapButton* m_bitmapButtonInternetImage;
		wxStaticText* m_staticTextInternetSpeed;
		wxTextCtrl* m_textCtrlInternetSpeed;
		wxBitmapButton* m_bitmapButtonInternetSpeed;
		wxStaticText* m_staticTextInternetSpeedUnit;
		wxStaticText* m_staticTextInternetCompress;
		wxCheckBox* m_checkBoxInternetCompress;
		wxBitmapButton* m_bitmapButtonInternetCompress;
		wxStaticText* m_staticTextInternetEncrypt;
		wxCheckBox* m_checkBoxInternetEncrypt;
		wxBitmapButton* m_bitmapButtonInternetEncrypt;
		
		wxButton* m_button1;
		wxButton* m_button2;

		wxNotebook *m_notebook;
		wxPanel *m_tab_filebackups;
		wxPanel *m_tab_imagebackups;
		wxPanel *m_tab_client;
		wxPanel *m_tab_internet;

		wxImage fa_lock_img_scaled;
		wxImage fa_home_img_scaled;
		wxImage fa_client_img_scaled;
		wxImage fa_copy_img_scaled;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnDisableImageBackups( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbortClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBitmapBtnClick(wxCommandEvent& event) { event.Skip(); }
		virtual void OnCtlChange(wxCommandEvent& event) { event.Skip(); }

		
		int capa;
	
	public:
		
		GUISettings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		virtual ~GUISettings();
	
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
		virtual void OnLogEntrySelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoglevelChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExitClick( wxCommandEvent& event ) { event.Skip(); }
		
	
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
		wxTextCtrl* m_textCtrl14;
		wxSizer* m_versionSizer;
		
		
		wxButton* m_button4;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOKClick( wxCommandEvent& event ) { event.Skip(); }
		
		virtual void OnClose()=0;

		virtual void OnCloseInt(wxCloseEvent& event) {
			event.Skip();
			OnClose();
		}
	
	public:
		
		GUIInfo( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,265 ), long style = wxDEFAULT_DIALOG_STYLE );
		~GUIInfo();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GUIConfigPath
///////////////////////////////////////////////////////////////////////////////
class GUIConfigPath : public wxDialog 
{
	private:
	
	protected:
		
		wxListBox* listbox;
		wxStaticText* m_staticText27;
		wxTextCtrl* m_textCtrl18;
		
		wxButton* m_button5;
		wxButton* m_button6;
		
		wxButton* m_button7;
		wxButton* m_button8;

		wxBitmapButton* m_bitmapButton1;

		wxImage fa_lock_img_scaled;
		wxImage fa_home_img_scaled;
		wxImage fa_client_img_scaled;
		wxImage fa_copy_img_scaled;

		/*wxStaticText* m_staticTextGroup;
		wxChoice* m_group;*/
		
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnPathSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNameTextChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickOk( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickAbort( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickNew( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickDel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnGroupChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickSourceSwitch(wxCommandEvent& event) { event.Skip(); }
	
	public:
		
		GUIConfigPath( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Add/Remove backup paths"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~GUIConfigPath();
	
};

struct SProcessItem
{
	wxBoxSizer* bSizer36;
	wxStaticText* m_staticText31;
	wxBoxSizer* bSizer362;
	wxStaticText* m_staticText312;
	wxGauge* m_gauge1;
	wxStaticLine* m_staticLine;
};

///////////////////////////////////////////////////////////////////////////////
/// Class GUIStatus
///////////////////////////////////////////////////////////////////////////////
class GUIStatus : public wxDialog 
{
	private:

		
	
	protected:
		void removeCurrentProcesses(size_t new_size);

		void resizeForProcesses(size_t new_size);

		void relayout();

		std::vector<SProcessItem> m_processItem;
		wxStaticText* m_staticText32;
		wxStaticText* m_staticText33;
		wxStaticText* m_staticText36;
		wxStaticText* m_staticText34;
		wxStaticText* m_staticText35;
		wxStaticText* m_staticText37;
		wxStaticText* m_staticText38;
		wxBoxSizer* bSizer34;

		virtual void OnClose()=0;

		virtual void OnCloseInt(wxCloseEvent& event);

		wxLongLong_t follow_only_process_id;
	
	public:
		
		GUIStatus( wxWindow* parent, wxLongLong_t follow_only_process_id, wxWindowID id = wxID_ANY, const wxString& title = _("Status"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~GUIStatus();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GUICbtStatus
///////////////////////////////////////////////////////////////////////////////
class GUICbtStatus : public wxDialog
{
private:

protected:
	wxTextCtrl* m_textCtrl3;

	wxButton* m_button5;

	// Virtual event handlers, overide them in your derived class
	virtual void OnExitClick(wxCommandEvent& event) { event.Skip(); }

public:

	GUICbtStatus(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("CBT status"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 222), long style = wxDEFAULT_DIALOG_STYLE);
	virtual ~GUICbtStatus();

};

#endif //__GUI_H__
