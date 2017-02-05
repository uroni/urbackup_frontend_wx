///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GUISetupWizard.h"

///////////////////////////////////////////////////////////////////////////

GUISetupWizard::GUISetupWizard( wxWindow* parent, wxWindowID id, const wxString& title, const wxBitmap& bitmap, const wxPoint& pos, long style ) 
{
	this->Create( parent, id, title, bitmap, pos, style );
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxWizardPageSimple* setupBackupPaths = new wxWizardPageSimple( this );
	m_pages.Add( setupBackupPaths );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( setupBackupPaths, wxID_ANY, _("Please select the file set you want to backup:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetFont( wxFont( 13, 70, 90, 90, false, wxEmptyString ) );
	
	bSizer2->Add( m_staticText1, 0, wxALL, 5 );
	
	m_radioBtn1 = new wxRadioButton( setupBackupPaths, wxID_ANY, _("Backup all files except temporary, cache, system and program files (recommended)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_radioBtn1->SetFont( wxFont( 10, 70, 90, 90, false, wxEmptyString ) );
	
	bSizer2->Add( m_radioBtn1, 0, wxALL, 5 );
	
	m_radioBtn2 = new wxRadioButton( setupBackupPaths, wxID_ANY, _("Backup only documents, music, pictures, movies and the desktop of all users"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtn2->SetFont( wxFont( 10, 70, 90, 90, false, wxEmptyString ) );
	
	bSizer2->Add( m_radioBtn2, 0, wxALL, 5 );
	
	m_radioBtn3 = new wxRadioButton( setupBackupPaths, wxID_ANY, _("Manually select the files to backup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtn3->SetFont( wxFont( 10, 70, 90, 90, false, wxEmptyString ) );
	
	bSizer2->Add( m_radioBtn3, 0, wxALL, 5 );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( setupBackupPaths, wxID_ANY, _("Please select the volumes to backup:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( 13, 70, 90, 90, false, wxEmptyString ) );
	
	bSizer3->Add( m_staticText2, 0, wxALL, 5 );
	
	m_radioBtn4 = new wxRadioButton( setupBackupPaths, wxID_ANY, _("Backup the system volume (recommended)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_radioBtn4->SetFont( wxFont( 10, 70, 90, 90, false, wxEmptyString ) );
	
	bSizer3->Add( m_radioBtn4, 0, wxALL, 5 );
	
	m_radioBtn5 = new wxRadioButton( setupBackupPaths, wxID_ANY, _("Backup all internal disk volumes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtn5->SetFont( wxFont( 10, 70, 90, 90, false, wxEmptyString ) );
	
	bSizer3->Add( m_radioBtn5, 0, wxALL, 5 );
	
	m_radioBtn6 = new wxRadioButton( setupBackupPaths, wxID_ANY, _("Manually list the volume letters to backup:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtn6->SetFont( wxFont( 10, 70, 90, 90, false, wxEmptyString ) );
	
	bSizer3->Add( m_radioBtn6, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3 = new wxStaticText( setupBackupPaths, wxID_ANY, _("     "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALL, 5 );
	
	m_textCtrl1 = new wxTextCtrl( setupBackupPaths, wxID_ANY, wxT("C;D"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->Enable( false );
	
	bSizer4->Add( m_textCtrl1, 0, wxALL, 5 );
	
	
	bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	
	
	setupBackupPaths->SetSizer( bSizer1 );
	setupBackupPaths->Layout();
	bSizer1->Fit( setupBackupPaths );
	
	this->Centre( wxBOTH );
	
	for ( unsigned int i = 1; i < m_pages.GetCount(); i++ )
	{
		m_pages.Item( i )->SetPrev( m_pages.Item( i - 1 ) );
		m_pages.Item( i - 1 )->SetNext( m_pages.Item( i ) );
	}
	
	// Connect Events
	this->Connect( wxID_ANY, wxEVT_WIZARD_CANCEL, wxWizardEventHandler( GUISetupWizard::wizardCancel ) );
	this->Connect( wxID_ANY, wxEVT_WIZARD_FINISHED, wxWizardEventHandler( GUISetupWizard::wizardFinished ) );
	this->Connect( wxID_ANY, wxEVT_WIZARD_PAGE_CHANGED, wxWizardEventHandler( GUISetupWizard::wizardNext ) );
	m_radioBtn4->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUISetupWizard::manualVolumeConfig ), NULL, this );
	m_radioBtn5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUISetupWizard::manualVolumeConfig ), NULL, this );
	m_radioBtn6->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUISetupWizard::manualVolumeConfig ), NULL, this );
}

GUISetupWizard::~GUISetupWizard()
{
	// Disconnect Events
	this->Disconnect( wxID_ANY, wxEVT_WIZARD_CANCEL, wxWizardEventHandler( GUISetupWizard::wizardCancel ) );
	this->Disconnect( wxID_ANY, wxEVT_WIZARD_FINISHED, wxWizardEventHandler( GUISetupWizard::wizardFinished ) );
	this->Disconnect( wxID_ANY, wxEVT_WIZARD_PAGE_CHANGED, wxWizardEventHandler( GUISetupWizard::wizardNext ) );
	m_radioBtn4->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUISetupWizard::manualVolumeConfig ), NULL, this );
	m_radioBtn5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUISetupWizard::manualVolumeConfig ), NULL, this );
	m_radioBtn6->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUISetupWizard::manualVolumeConfig ), NULL, this );
	
	m_pages.Clear();
}
