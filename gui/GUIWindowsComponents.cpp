///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GUIWindowsComponents.h"

///////////////////////////////////////////////////////////////////////////

GUIWindowsComponents::GUIWindowsComponents( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl1 = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer1->Add( m_treeCtrl1, 10, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Numer of file backups after which to run a virtual full file backup:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer3->Add( m_staticText1, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_textCtrl1, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button1, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button2, 0, wxALL, 5 );
	
	
	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_treeCtrl1->Connect( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, wxTreeEventHandler( GUIWindowsComponents::evtOnTreeItemGetTooltip ), NULL, this );
	m_treeCtrl1->Connect( wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler( GUIWindowsComponents::evtOnTreeStateImageClick ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIWindowsComponents::onOkClick ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIWindowsComponents::onCancel ), NULL, this );
}

GUIWindowsComponents::~GUIWindowsComponents()
{
	// Disconnect Events
	m_treeCtrl1->Disconnect( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, wxTreeEventHandler( GUIWindowsComponents::evtOnTreeItemGetTooltip ), NULL, this );
	m_treeCtrl1->Disconnect( wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler( GUIWindowsComponents::evtOnTreeStateImageClick ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIWindowsComponents::onOkClick ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIWindowsComponents::onCancel ), NULL, this );
	
}
