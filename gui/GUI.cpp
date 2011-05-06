///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GUI.h"

///////////////////////////////////////////////////////////////////////////

GUISettings::GUISettings( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Intervall für inkrementelle Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText1->Wrap( -1 );
	bSizer29->Add( m_staticText1, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer29->Add( m_textCtrl1, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Stunden"), wxPoint( -1,-1 ), wxSize( -1,-1 ), 0 );
	m_staticText2->Wrap( -1 );
	bSizer29->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer1->Add( bSizer29, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Intervall für volle Backups:"), wxPoint( -1,-1 ), wxSize( 300,-1 ), 0 );
	m_staticText3->Wrap( -1 );
	bSizer30->Add( m_staticText3, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl2 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer30->Add( m_textCtrl2, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Tage"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer30->Add( m_staticText4, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer1->Add( bSizer30, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Intervall für inkrementelle Image-Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText6->Wrap( -1 );
	bSizer31->Add( m_staticText6, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl21 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_textCtrl21, 0, wxALL, 5 );
	
	m_staticText41 = new wxStaticText( this, wxID_ANY, _("Tage"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText41->Wrap( -1 );
	m_staticText41->SetMinSize( wxSize( 50,-1 ) );
	
	bSizer31->Add( m_staticText41, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer31->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, _("Aktiv"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox1->SetValue(true); 
	bSizer31->Add( m_checkBox1, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer1->Add( bSizer31, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, _("Intervall für volle Image-Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText7->Wrap( -1 );
	bSizer32->Add( m_staticText7, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl22 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer32->Add( m_textCtrl22, 0, wxALL, 5 );
	
	m_staticText42 = new wxStaticText( this, wxID_ANY, _("Tage"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText42->Wrap( -1 );
	bSizer32->Add( m_staticText42, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer1->Add( bSizer32, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText30 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	bSizer33->Add( m_staticText30, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer33, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer341;
	bSizer341 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, _("Minimale Anzahl an inkrementellen Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText9->Wrap( -1 );
	bSizer341->Add( m_staticText9, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl13 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer341->Add( m_textCtrl13, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer341, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, _("Maximale Anzahl an inkrementellen Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText10->Wrap( -1 );
	bSizer35->Add( m_staticText10, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl131 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_textCtrl131, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer35, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, _("Minimale Anzahl an vollen Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText11->Wrap( -1 );
	bSizer36->Add( m_staticText11, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl132 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer36->Add( m_textCtrl132, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer36, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, _("Maximale Anzahl an vollen Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText12->Wrap( -1 );
	bSizer37->Add( m_staticText12, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl133 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer37->Add( m_textCtrl133, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer37, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText29 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	bSizer38->Add( m_staticText29, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer38, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, _("Minimale Anzahl an inkrementellen Image-Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText14->Wrap( -1 );
	bSizer39->Add( m_staticText14, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl134 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer39->Add( m_textCtrl134, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer39, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText15 = new wxStaticText( this, wxID_ANY, _("Maximale Anzahl an inkrementellen Image-Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText15->Wrap( -1 );
	bSizer40->Add( m_staticText15, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl135 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer40->Add( m_textCtrl135, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer40, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText16 = new wxStaticText( this, wxID_ANY, _("Minimale Anzahl an vollen Image-Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText16->Wrap( -1 );
	bSizer41->Add( m_staticText16, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl136 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( m_textCtrl136, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer41, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, _("Maximale Anzahl an vollen Image-Backups:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText17->Wrap( -1 );
	bSizer42->Add( m_staticText17, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl137 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer42->Add( m_textCtrl137, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer42, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText28 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	bSizer43->Add( m_staticText28, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer43, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText25 = new wxStaticText( this, wxID_ANY, _("Computername:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText25->Wrap( -1 );
	bSizer28->Add( m_staticText25, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl15 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	bSizer28->Add( m_textCtrl15, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer28, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText26 = new wxStaticText( this, wxID_ANY, _("Vom Backup auszuschließen (mit Wildcards):"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText26->Wrap( -1 );
	bSizer44->Add( m_staticText26, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl16 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	bSizer44->Add( m_textCtrl16, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer44, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText27 = new wxStaticText( this, wxID_ANY, _("Backup Zeitfenster:"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	m_staticText27->Wrap( -1 );
	bSizer45->Add( m_staticText27, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl17 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	bSizer45->Add( m_textCtrl17, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer45, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer331;
	bSizer331 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText281 = new wxStaticText( this, wxID_ANY, _("Backupverzögerung nach Systemstart:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText281->Wrap( -1 );
	m_staticText281->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer331->Add( m_staticText281, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl19 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer331->Add( m_textCtrl19, 0, wxALL, 5 );
	
	m_staticText291 = new wxStaticText( this, wxID_ANY, _("min"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText291->Wrap( -1 );
	bSizer331->Add( m_staticText291, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer1->Add( bSizer331, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer4->SetMinSize( wxSize( -1,50 ) ); 
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->SetMinSize( wxSize( -1,25 ) );
	
	bSizer4->Add( m_button1, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, _("Abbrechen"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->SetMinSize( wxSize( -1,25 ) );
	
	bSizer4->Add( m_button2, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	bSizer1->Add( bSizer4, 0, wxEXPAND|wxFIXED_MINSIZE, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_checkBox1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( GUISettings::OnDisableImageBackups ), NULL, this );
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
	
	wxString m_choice1Choices[] = { _("Info"), _("Warnings"), _("Errors") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	m_choice1->SetSelection( 2 );
	bSizer11->Add( m_choice1, 0, wxALL, 5 );
	
	bSizer9->Add( bSizer11, 1, wxEXPAND, 5 );
	
	m_textCtrl3 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer9->Add( m_textCtrl3, 15, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer10->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button5 = new wxButton( this, wxID_ANY, _("Verlassen"), wxDefaultPosition, wxDefaultSize, 0 );
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
	
	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( _("logo1.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_bitmap1->SetMinSize( wxSize( 100,91 ) );
	
	bSizer25->Add( m_bitmap1, 0, wxALL, 5 );
	
	
	bSizer25->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, _("Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	bSizer26->Add( m_staticText21, 0, wxALL, 5 );
	
	m_staticText22 = new wxStaticText( this, wxID_ANY, _("    0.38"), wxDefaultPosition, wxDefaultSize, 0 );
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
	
	m_button4 = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
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
	
	m_button6 = new wxButton( this, wxID_ANY, _("Abbrechen"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_button6, 0, wxALL, 5 );
	
	
	bSizer31->Add( 0, 40, 1, wxEXPAND, 5 );
	
	m_button7 = new wxButton( this, wxID_ANY, _("Neuer Pfad"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_button7, 0, wxALL, 5 );
	
	m_button8 = new wxButton( this, wxID_ANY, _("Pfad entfernen"), wxDefaultPosition, wxDefaultSize, 0 );
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
