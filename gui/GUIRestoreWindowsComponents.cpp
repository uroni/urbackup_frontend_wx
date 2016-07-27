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
