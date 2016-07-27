///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GUIRestoreWindowsComponents.h"

///////////////////////////////////////////////////////////////////////////

GUISelectRestoreComponents::GUISelectRestoreComponents( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer2->Add( m_listBox1, 15, wxALL|wxEXPAND, 5 );
	
	m_treeCtrl1 = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer2->Add( m_treeCtrl1, 85, wxALL|wxEXPAND, 5 );
	
	
	bSizer1->Add( bSizer2, 20, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, _("Start restore"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button1, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button2, 0, wxALL, 5 );
	
	
	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GUISelectRestoreComponents::onListBoxSelected ), NULL, this );
	m_treeCtrl1->Connect( wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler( GUISelectRestoreComponents::evtOnTreeStateImageClick ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISelectRestoreComponents::onStartRestore ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISelectRestoreComponents::onCancel ), NULL, this );
}

GUISelectRestoreComponents::~GUISelectRestoreComponents()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GUISelectRestoreComponents::onListBoxSelected ), NULL, this );
	m_treeCtrl1->Disconnect( wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler( GUISelectRestoreComponents::evtOnTreeStateImageClick ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISelectRestoreComponents::onStartRestore ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISelectRestoreComponents::onCancel ), NULL, this );
	
}

GUIRestoreComponents::GUIRestoreComponents( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Starting restore operation..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer4->Add( m_staticText1, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALL, 5 );
	
	m_gauge1 = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_gauge1->SetValue( 0 ); 
	bSizer4->Add( m_gauge1, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	bSizer4->Add( m_textCtrl1, 20, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button3->Enable( false );
	
	bSizer5->Add( m_button3, 0, wxALL, 5 );
	
	
	bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer4 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIRestoreComponents::onOkClick ), NULL, this );
}

GUIRestoreComponents::~GUIRestoreComponents()
{
	// Disconnect Events
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIRestoreComponents::onOkClick ), NULL, this );
	
}
