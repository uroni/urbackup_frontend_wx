///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GUI.h"
#include "../main.h"
#include "../capa_bits.h"

extern MyTimer *timer;
extern wxString res_path;

#undef _
#define _(s) wxGetTranslation(wxT(s))

///////////////////////////////////////////////////////////////////////////

GUISettings::GUISettings( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxNB_DEFAULT);

	m_tab_filebackups=new wxPanel(m_notebook, wxID_ANY);
	m_tab_imagebackups=NULL;
	m_tab_client=new wxPanel(m_notebook, wxID_ANY);
	m_tab_internet=new wxPanel(m_notebook, wxID_ANY);;

	wxBoxSizer* bSizer_filebackups=new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* bSizer_imagebackups=NULL;
	wxBoxSizer* bSizer_client=new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* bSizer_internet=new wxBoxSizer( wxVERTICAL );

#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		m_tab_imagebackups=new wxPanel(m_notebook, wxID_ANY);
		bSizer_imagebackups=new wxBoxSizer( wxVERTICAL );
	}
#endif
	

	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Interval for incremental file backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText1->Wrap( -1 );
	bSizer29->Add( m_staticText1, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl1 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer29->Add( m_textCtrl1, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("hours"), wxPoint( -1,-1 ), wxSize( -1,-1 ), 0 );
	m_staticText2->Wrap( -1 );
	bSizer29->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer29, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Interval for full file backups:"), wxPoint( -1,-1 ), wxSize( 300,-1 ), 0 );
	m_staticText3->Wrap( -1 );
	bSizer30->Add( m_staticText3, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl2 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer30->Add( m_textCtrl2, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("days"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer30->Add( m_staticText4, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer30, 0, wxEXPAND, 5 );
	
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		wxBoxSizer* bSizer31;
		bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText6 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Interval for incremental image backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
		m_staticText6->Wrap( -1 );
		bSizer31->Add( m_staticText6, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl21 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer31->Add( m_textCtrl21, 0, wxALL, 5 );
	
		m_staticText41 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("days"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText41->Wrap( -1 );
		m_staticText41->SetMinSize( wxSize( 50,-1 ) );
	
		bSizer31->Add( m_staticText41, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_checkBox1 = new wxCheckBox( m_tab_imagebackups, wxID_ANY, _("Active"), wxDefaultPosition, wxDefaultSize, 0 );
		m_checkBox1->SetValue(true); 
		bSizer31->Add( m_checkBox1, 0, wxALIGN_CENTER|wxALL, 5 );
	
		bSizer_imagebackups->Add( bSizer31, 0, wxEXPAND, 5 );
	
		wxBoxSizer* bSizer32;
		bSizer32 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText7 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Interval for full image backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
		m_staticText7->Wrap( -1 );
		bSizer32->Add( m_staticText7, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl22 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer32->Add( m_textCtrl22, 0, wxALL, 5 );
	
		m_staticText42 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("days"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText42->Wrap( -1 );
		bSizer32->Add( m_staticText42, 0, wxALIGN_CENTER|wxALL, 5 );
	
		bSizer_imagebackups->Add( bSizer32, 0, wxEXPAND, 5 );
	}
#endif
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText30 = new wxStaticText( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	bSizer33->Add( m_staticText30, 0, wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer33, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer341;
	bSizer341 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText9 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Minimal number of incremental file backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText9->Wrap( -1 );
	bSizer341->Add( m_staticText9, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl13 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer341->Add( m_textCtrl13, 0, wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer341, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText10 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Maximal number of incremental file backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText10->Wrap( -1 );
	bSizer35->Add( m_staticText10, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl131 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_textCtrl131, 0, wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer35, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Minimal number of full file backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText11->Wrap( -1 );
	bSizer36->Add( m_staticText11, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl132 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer36->Add( m_textCtrl132, 0, wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer36, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText12 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Maximal number of full file backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText12->Wrap( -1 );
	bSizer37->Add( m_staticText12, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl133 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer37->Add( m_textCtrl133, 0, wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer37, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText29 = new wxStaticText( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	bSizer38->Add( m_staticText29, 0, wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer38, 0, wxEXPAND, 5 );
	
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		wxBoxSizer* bSizer38s;
		bSizer38s = new wxBoxSizer( wxVERTICAL );

		wxStaticText *m_staticText29s = new wxStaticText( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText29s->Wrap( -1 );
		bSizer38s->Add( m_staticText29s, 0, wxALL, 5 );
	
		bSizer_imagebackups->Add( bSizer38s, 0, wxEXPAND, 5 );

		wxBoxSizer* bSizer39;
		bSizer39 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText14 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Minimal number of incremental image backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
		m_staticText14->Wrap( -1 );
		bSizer39->Add( m_staticText14, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl134 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer39->Add( m_textCtrl134, 0, wxALL, 5 );
	
		bSizer_imagebackups->Add( bSizer39, 0, wxEXPAND, 5 );
	
		wxBoxSizer* bSizer40;
		bSizer40 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText15 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Maximal number of incremental image backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
		m_staticText15->Wrap( -1 );
		bSizer40->Add( m_staticText15, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl135 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer40->Add( m_textCtrl135, 0, wxALL, 5 );
	
		bSizer_imagebackups->Add( bSizer40, 0, wxEXPAND, 5 );
	
		wxBoxSizer* bSizer41;
		bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText16 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Minimal number of full image backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
		m_staticText16->Wrap( -1 );
		bSizer41->Add( m_staticText16, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl136 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer41->Add( m_textCtrl136, 0, wxALL, 5 );
	
		bSizer_imagebackups->Add( bSizer41, 0, wxEXPAND, 5 );
	
		wxBoxSizer* bSizer42;
		bSizer42 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText17 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Maximal number of full image backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
		m_staticText17->Wrap( -1 );
		bSizer42->Add( m_staticText17, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl137 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer42->Add( m_textCtrl137, 0, wxALL, 5 );
	
		bSizer_imagebackups->Add( bSizer42, 0, wxEXPAND, 5 );
	
		wxBoxSizer* bSizer43;
		bSizer43 = new wxBoxSizer( wxVERTICAL );
	
		m_staticText28 = new wxStaticText( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText28->Wrap( -1 );
		bSizer43->Add( m_staticText28, 1, wxEXPAND, 5 );
	
		bSizer_imagebackups->Add( bSizer43, 0, wxEXPAND, 5 );
	}
#endif
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText25 = new wxStaticText( m_tab_client, wxID_ANY, _("Computer name:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText25->Wrap( -1 );
	bSizer28->Add( m_staticText25, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl15 = new wxTextCtrl( m_tab_client, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	bSizer28->Add( m_textCtrl15, 0, wxALL, 5 );
	
	bSizer_client->Add( bSizer28, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText26 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Exclude from backup (with wildcards):"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText26->Wrap( -1 );
	bSizer44->Add( m_staticText26, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl16 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	bSizer44->Add( m_textCtrl16, 0, wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer44, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer441;
	bSizer441 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText261 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Include in backup (with wildcards):"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText261->Wrap( -1 );
	bSizer441->Add( m_staticText261, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl161 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	bSizer441->Add( m_textCtrl161, 0, wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer441, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText27 = new wxStaticText( m_tab_client, wxID_ANY, _("Backup window:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText27->Wrap( -1 );
	bSizer45->Add( m_staticText27, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl17 = new wxTextCtrl( m_tab_client, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	bSizer45->Add( m_textCtrl17, 0, wxALL, 5 );
	
	bSizer_client->Add( bSizer45, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer331;
	bSizer331 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText281 = new wxStaticText( m_tab_client, wxID_ANY, _("Backup delay after system start:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText281->Wrap( -1 );
	m_staticText281->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer331->Add( m_staticText281, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl19 = new wxTextCtrl( m_tab_client, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer331->Add( m_textCtrl19, 0, wxALL, 5 );
	
	m_staticText291 = new wxStaticText( m_tab_client, wxID_ANY, _("min"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText291->Wrap( -1 );
	bSizer331->Add( m_staticText291, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer_client->Add( bSizer331, 0, wxEXPAND, 5 );

#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		wxBoxSizer* bSizer441;
		bSizer441 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText301 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Volumes to backup:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
		m_staticText301->Wrap( -1 );
		bSizer441->Add( m_staticText301, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl23 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
		bSizer441->Add( m_textCtrl23, 0, wxALL, 5 );
	
		bSizer_imagebackups->Add( bSizer441, 0, wxEXPAND, 5 );

		wxBoxSizer* bSizer38s;
		bSizer38s = new wxBoxSizer( wxVERTICAL );

		wxStaticText *m_staticText29s = new wxStaticText( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText29s->Wrap( -1 );
		bSizer38s->Add( m_staticText29s, 0, wxALL, 5 );

		bSizer_imagebackups->Add( bSizer38s, 5, wxALL, 5 );
	}
#endif

	wxBoxSizer* bSizerH;

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextLocalSpeed = new wxStaticText( m_tab_client, wxID_ANY, _("Max backup speed for local network:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticTextLocalSpeed->Wrap( -1 );
	bSizerH->Add( m_staticTextLocalSpeed, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlLocalSpeed = new wxTextCtrl( m_tab_client, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizerH->Add( m_textCtrlLocalSpeed, 0, wxALL, 5 );	
	m_staticTextLocalSpeedUnit = new wxStaticText( m_tab_client, wxID_ANY, _("MBit/s"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticTextLocalSpeedUnit->Wrap( -1 );
	bSizerH->Add( m_staticTextLocalSpeedUnit, 0, wxALIGN_CENTER|wxALL, 5 );	
	bSizer_client->Add( bSizerH, 0, wxEXPAND, 5 );


	//Internet tab
	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetEnabled = new wxStaticText( m_tab_internet, wxID_ANY, _("Enable backup via internet:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticTextInternetEnabled->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetEnabled, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_checkBoxInternetEnabled = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerH->Add( m_checkBoxInternetEnabled, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticInternetServer = new wxStaticText( m_tab_internet, wxID_ANY, _("Internet server name/IP:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticInternetServer->Wrap( -1 );
	bSizerH->Add( m_staticInternetServer, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlInternetServer = new wxTextCtrl( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	bSizerH->Add( m_textCtrlInternetServer, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticInternetServerPort = new wxStaticText( m_tab_internet, wxID_ANY, _("Internet server port:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticInternetServerPort->Wrap( -1 );
	bSizerH->Add( m_staticInternetServerPort, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlInternetServerPort = new wxTextCtrl( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizerH->Add( m_textCtrlInternetServerPort, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticInternetServerAuthkey = new wxStaticText( m_tab_internet, wxID_ANY, _("Internet server password:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticInternetServerAuthkey->Wrap( -1 );
	bSizerH->Add( m_staticInternetServerAuthkey, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlInternetServerAuthkey = new wxTextCtrl( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizerH->Add( m_textCtrlInternetServerAuthkey, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		bSizerH = new wxBoxSizer( wxHORIZONTAL );	
		m_staticTextInternetImage = new wxStaticText( m_tab_internet, wxID_ANY, _("Do image backups over internet:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
		m_staticTextInternetImage->Wrap( -1 );
		bSizerH->Add( m_staticTextInternetImage, 0, wxALIGN_CENTER|wxALL, 5 );	
		m_checkBoxInternetImage = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizerH->Add( m_checkBoxInternetImage, 0, wxALL, 5 );	
		bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );
	}
#endif

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetFullFile = new wxStaticText( m_tab_internet, wxID_ANY, _("Do full file backups over internet:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticTextInternetFullFile->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetFullFile, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_checkBoxInternetFullFile = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerH->Add( m_checkBoxInternetFullFile, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetSpeed = new wxStaticText( m_tab_internet, wxID_ANY, _("Max backups speed for internet connection:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticTextInternetSpeed->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetSpeed, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlInternetSpeed = new wxTextCtrl( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizerH->Add( m_textCtrlInternetSpeed, 0, wxALL, 5 );	
	m_staticTextInternetSpeedUnit = new wxStaticText( m_tab_internet, wxID_ANY, _("KBit/s"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticTextInternetSpeedUnit->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetSpeedUnit, 0, wxALIGN_CENTER|wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetCompress = new wxStaticText( m_tab_internet, wxID_ANY, _("Compressed transfer:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticTextInternetCompress->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetCompress, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_checkBoxInternetCompress = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerH->Add( m_checkBoxInternetCompress, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetEncrypt = new wxStaticText( m_tab_internet, wxID_ANY, _("Encrypted transfer:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticTextInternetEncrypt->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetEncrypt, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_checkBoxInternetEncrypt = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerH->Add( m_checkBoxInternetEncrypt, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );



	{
		bSizer_client->Add( 0, 0, 10, wxEXPAND, 5 );
		bSizer_filebackups->Add( 0, 0, 5, wxEXPAND, 5 );
		bSizer_internet->Add( 0, 0, 10, wxEXPAND, 5 );
	}

	m_tab_filebackups->SetSizerAndFit( bSizer_filebackups);
	m_tab_client->SetSizerAndFit( bSizer_client );
	m_tab_internet->SetSizerAndFit( bSizer_internet );

	m_notebook->AddPage(m_tab_filebackups, _("File backups"));
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		m_tab_imagebackups->SetSizerAndFit( bSizer_imagebackups );
		m_notebook->AddPage(m_tab_imagebackups, _("Image backups"));
	}
#endif
	m_notebook->AddPage(m_tab_client, _("Client") );
	m_notebook->AddPage(m_tab_internet, _("Internet") );

	bSizer1->Add(m_notebook);
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer4->SetMinSize( wxSize( -1,50 ) ); 
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->SetMinSize( wxSize( -1,25 ) );
	
	bSizer4->Add( m_button1, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, _("Abort"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->SetMinSize( wxSize( -1,25 ) );
	
	bSizer4->Add( m_button2, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	bSizer1->Add( bSizer4, 0, wxEXPAND|wxFIXED_MINSIZE, 5 );
	
	this->SetSizerAndFit( bSizer1 );
	this->Layout();	
	this->Centre( wxBOTH );
	
	// Connect Events
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		m_checkBox1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( GUISettings::OnDisableImageBackups ), NULL, this );
	}
#endif
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISettings::OnOkClick ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISettings::OnAbortClick ), NULL, this );
}

GUISettings::~GUISettings()
{
	// Disconnect Events
	m_checkBox1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( GUISettings::OnDisableImageBackups ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISettings::OnOkClick ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISettings::OnAbortClick ), NULL, this );
	
}

GUILogfiles::GUILogfiles( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer8->Add( m_listBox1, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer11->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Filter:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer11->Add( m_staticText5, 0, wxALL, 5 );
	
	wxString m_choice1Choices[] = { _("Infos"), _("Warnings"), _("Errors") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	bSizer11->Add( m_choice1, 0, wxALL, 5 );
	
	bSizer9->Add( bSizer11, 1, wxEXPAND, 5 );
	
	m_textCtrl3 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer9->Add( m_textCtrl3, 15, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer10->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button5 = new wxButton( this, wxID_ANY, _("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_button5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer9->Add( bSizer10, 1, wxEXPAND|wxALIGN_RIGHT, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer8 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GUILogfiles::OnLogEntrySelect ), NULL, this );
	m_choice1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( GUILogfiles::OnLoglevelChange ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUILogfiles::OnExitClick ), NULL, this );
}

GUILogfiles::~GUILogfiles()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GUILogfiles::OnLogEntrySelect ), NULL, this );
	m_choice1->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( GUILogfiles::OnLoglevelChange ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUILogfiles::OnExitClick ), NULL, this );
	
}

GUIInfo::GUIInfo( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer24->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer25->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( res_path+_("logo1.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_bitmap1->SetMinSize( wxSize( 100,91 ) );
	
	bSizer25->Add( m_bitmap1, 0, wxALL, 5 );
	
	
	bSizer25->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, _("Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	bSizer26->Add( m_staticText21, 0, wxALL, 5 );
	
	m_staticText22 = new wxStaticText( this, wxID_ANY, _("    1.3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	bSizer26->Add( m_staticText22, 0, wxALL, 5 );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	bSizer26->Add( m_staticText23, 0, wxALL, 5 );
	
	m_textCtrl14 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 170,100 ), wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL );
	bSizer26->Add( m_textCtrl14, 0, wxALL, 5 );
	
	bSizer25->Add( bSizer26, 3, wxEXPAND, 5 );
	
	bSizer24->Add( bSizer25, 6, wxEXPAND, 5 );
	
	
	bSizer24->Add( 0, 0, 4, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer27->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button4 = new wxButton( this, wxID_ANY, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer27->Add( m_button4, 0, wxALL, 5 );
	
	bSizer24->Add( bSizer27, 2, wxEXPAND, 5 );
	
	this->SetSizer( bSizer24 );
	this->Layout();
	
	// Connect Events
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIInfo::OnOKClick ), NULL, this );
}

GUIInfo::~GUIInfo()
{
	// Disconnect Events
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIInfo::OnOKClick ), NULL, this );
	
}

GUIConfigPath::GUIConfigPath( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer29->Add( 10, 0, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxVERTICAL );
	
	listbox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	listbox->SetMinSize( wxSize( 450,400 ) );
	
	bSizer30->Add( listbox, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText27 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	m_staticText27->SetMinSize( wxSize( 40,-1 ) );
	
	bSizer32->Add( m_staticText27, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl18 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl18->Enable( false );
	m_textCtrl18->SetMinSize( wxSize( 200,-1 ) );
	
	bSizer32->Add( m_textCtrl18, 0, wxALL, 5 );
	
	bSizer30->Add( bSizer32, 0, wxEXPAND, 5 );
	
	
	bSizer30->Add( 0, 20, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button5 = new wxButton( this, wxID_ANY, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_button5, 0, wxALL, 5 );
	
	m_button6 = new wxButton( this, wxID_ANY, _("Abort"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_button6, 0, wxALL, 5 );
	
	
	bSizer31->Add( 0, 40, 1, wxEXPAND, 5 );
	
	m_button7 = new wxButton( this, wxID_ANY, _("Add path"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_button7, 0, wxALL, 5 );
	
	m_button8 = new wxButton( this, wxID_ANY, _("Remove path"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_button8, 0, wxALL, 5 );
	
	bSizer30->Add( bSizer31, 1, wxEXPAND, 5 );
	
	bSizer29->Add( bSizer30, 1, wxEXPAND, 5 );
	
	
	bSizer29->Add( 10, 0, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer29 );
	this->Layout();
	bSizer29->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	listbox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GUIConfigPath::OnPathSelected ), NULL, this );
	m_textCtrl18->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GUIConfigPath::OnNameTextChange ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIConfigPath::OnClickOk ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIConfigPath::OnClickAbort ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIConfigPath::OnClickNew ), NULL, this );
	m_button8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIConfigPath::OnClickDel ), NULL, this );
}

GUIConfigPath::~GUIConfigPath()
{
	// Disconnect Events
	listbox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GUIConfigPath::OnPathSelected ), NULL, this );
	m_textCtrl18->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GUIConfigPath::OnNameTextChange ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIConfigPath::OnClickOk ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIConfigPath::OnClickAbort ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIConfigPath::OnClickNew ), NULL, this );
	m_button8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIConfigPath::OnClickDel ), NULL, this );
	
}

