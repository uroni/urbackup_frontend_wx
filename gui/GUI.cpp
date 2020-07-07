///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GUI.h"
#include "../main.h"
#include "../capa_bits.h"
#include "../../client_version.h"
#include "../Connector.h"

extern MyTimer *timer;
extern wxString res_path;

#undef _
#define _(s) wxGetTranslation(wxT(s))

wxSize findDlgUnit(wxWindow* parent, wxSize orig)
{
	for (int y = -1; y < 1000; ++y)
	{
		bool y_equal = false;
		wxSize sm(-1, -1);
		int diff_x = 10000;
		for (int x = -1; x < 1000; ++x)
		{
			wxSize other = wxDLG_UNIT(parent, wxSize(x, y));
			if (other == orig)
				return other;
			if (other.y == orig.y)
				y_equal = true;
			if (other.y == orig.y)
			{
				int diff = other.x - orig.x;
				if (diff < 0) diff *= -1;
				if (diff < diff_x)
				{
					diff_x = diff;
					sm = wxSize(x, y);
				}
			}
		}

		if (y_equal && diff_x<100)
			return sm;
	}
	return wxSize(-1, -1);
}

wxSize findDlgUnitY(wxWindow* parent, wxSize orig)
{
	for (int x = -1; x < 1000; ++x)
	{
		bool x_equal = false;
		wxSize sm(-1, -1);
		int diff_y = 10000;
		for (int y = -1; y < 1000; ++y)
		{
			wxSize other = wxDLG_UNIT(parent, wxSize(x, y));
			if (other == orig)
				return other;
			if (other.x == orig.x)
				x_equal = true;
			if (other.x == orig.x)
			{
				int diff = other.y - orig.y;
				if (diff < 0) diff *= -1;
				if (diff < diff_y)
				{
					diff_y = diff;
					sm = wxSize(x, y);
				}
			}
		}

		if (x_equal && diff_y<100)
			return sm;
	}
	return wxSize(-1, -1);
}

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

	capa = timer?timer->getCapa():Connector::getCapabilities();

#ifdef _WIN32
	if( !MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa) )
	{
		m_tab_imagebackups=new wxPanel(m_notebook, wxID_ANY);
		bSizer_imagebackups=new wxBoxSizer( wxVERTICAL );
	}
#endif
	
	wxSize left_column_size = wxDLG_UNIT(this, wxSize(171, -1));
	wxSize string_input_size = wxDLG_UNIT(this, wxSize(85, -1));
	wxSize number_input_size = wxDLG_UNIT(this, wxSize(28, -1));

	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Interval for incremental file backups:"), wxDefaultPosition, left_column_size, 0 );
	m_staticText1->Wrap( -1 );
	bSizer29->Add( m_staticText1, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl1 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer29->Add( m_textCtrl1, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("hours"), wxPoint( -1,-1 ), wxSize( -1,-1 ), 0 );
	m_staticText2->Wrap( -1 );
	bSizer29->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxSize image_button_size = wxDLG_UNIT(this, wxSize(15, 13));
	wxSize size_sub = wxDLG_UNIT(this, wxSize(5, 5));

	wxBitmap fa_home;
#ifdef _WIN32
	fa_home.LoadFile("fa-home.png", wxBITMAP_TYPE_PNG);
#else
	fa_home.LoadFile(res_path+wxT("fa-home.png"), wxBITMAP_TYPE_PNG);
#endif
	fa_home_img_scaled = fa_home.ConvertToImage().Scale(image_button_size.GetWidth()- size_sub.GetWidth(), image_button_size.GetHeight()- size_sub.GetHeight(), wxIMAGE_QUALITY_BILINEAR);

	wxBitmap fa_client;
#ifdef _WIN32
	fa_client.LoadFile("fa-road.png", wxBITMAP_TYPE_PNG);
#else
	fa_client.LoadFile(res_path+wxT("fa-road.png"), wxBITMAP_TYPE_PNG);
#endif
	fa_client_img_scaled = fa_client.ConvertToImage().Scale(image_button_size.GetWidth() - size_sub.GetWidth(), image_button_size.GetHeight() - size_sub.GetHeight(), wxIMAGE_QUALITY_BILINEAR);

	wxBitmap fa_lock;
#ifdef _WIN32
	fa_lock.LoadFile("fa-lock.png", wxBITMAP_TYPE_PNG);
#else
	fa_lock.LoadFile(res_path+wxT("fa-lock.png"), wxBITMAP_TYPE_PNG);
#endif
	fa_lock_img_scaled = fa_lock.ConvertToImage().Scale(image_button_size.GetWidth() - size_sub.GetWidth(), image_button_size.GetHeight() - size_sub.GetHeight(), wxIMAGE_QUALITY_BILINEAR);

	m_bitmapButton1 = new wxBitmapButton(m_tab_filebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer29->Add(m_bitmapButton1, 0, wxALIGN_CENTER | wxALL, 5);
	
	bSizer_filebackups->Add( bSizer29, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Interval for full file backups:"), wxPoint( -1,-1 ), left_column_size, 0 );
	m_staticText3->Wrap( -1 );
	bSizer30->Add( m_staticText3, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl2 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer30->Add( m_textCtrl2, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("days"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer30->Add( m_staticText4, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_bitmapButton2 = new wxBitmapButton(m_tab_filebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer30->Add(m_bitmapButton2, 0, wxALIGN_CENTER | wxALL, 5);

	bSizer_filebackups->Add( bSizer30, 0, wxEXPAND, 5 );
	
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		wxBoxSizer* bSizer31;
		bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText6 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Interval for incremental image backups:"), wxDefaultPosition, left_column_size, 0 );
		m_staticText6->Wrap( -1 );
		bSizer31->Add( m_staticText6, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl21 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer31->Add( m_textCtrl21, 0, wxALL, 5 );
	
		m_staticText41 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("days"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText41->Wrap( -1 );
		m_staticText41->SetMinSize( number_input_size );
	
		bSizer31->Add( m_staticText41, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_checkBox1 = new wxCheckBox( m_tab_imagebackups, wxID_ANY, _("Active"), wxDefaultPosition, wxDefaultSize, 0 );
		m_checkBox1->SetValue(true); 
		bSizer31->Add( m_checkBox1, 0, wxALIGN_CENTER|wxALL, 5 );

		m_bitmapButton21 = new wxBitmapButton(m_tab_imagebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
		bSizer31->Add(m_bitmapButton21, 0, wxALIGN_CENTER | wxALL, 5);
	
		bSizer_imagebackups->Add( bSizer31, 0, wxEXPAND, 5 );
	
		wxBoxSizer* bSizer32;
		bSizer32 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText7 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Interval for full image backups:"), wxDefaultPosition, left_column_size, 0 );
		m_staticText7->Wrap( -1 );
		bSizer32->Add( m_staticText7, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl22 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer32->Add( m_textCtrl22, 0, wxALL, 5 );
	
		m_staticText42 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("days"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText42->Wrap( -1 );
		bSizer32->Add( m_staticText42, 0, wxALIGN_CENTER|wxALL, 5 );

		m_bitmapButton22 = new wxBitmapButton(m_tab_imagebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
		bSizer32->Add(m_bitmapButton22, 0, wxALIGN_CENTER | wxALL, 5);
	
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
	
	m_staticText9 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Minimal number of incremental file backups:"), wxDefaultPosition, left_column_size, 0 );
	m_staticText9->Wrap( -1 );
	bSizer341->Add( m_staticText9, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl13 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer341->Add( m_textCtrl13, 0, wxALL, 5 );

	m_bitmapButton13 = new wxBitmapButton(m_tab_filebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer341->Add(m_bitmapButton13, 0, wxALIGN_CENTER | wxALL, 5);
	
	bSizer_filebackups->Add( bSizer341, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText10 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Maximal number of incremental file backups:"), wxDefaultPosition, left_column_size, 0 );
	m_staticText10->Wrap( -1 );
	bSizer35->Add( m_staticText10, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl131 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_textCtrl131, 0, wxALL, 5 );

	m_bitmapButton131 = new wxBitmapButton(m_tab_filebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer35->Add(m_bitmapButton131, 0, wxALIGN_CENTER | wxALL, 5);
	
	bSizer_filebackups->Add( bSizer35, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Minimal number of full file backups:"), wxDefaultPosition, left_column_size, 0 );
	m_staticText11->Wrap( -1 );
	bSizer36->Add( m_staticText11, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl132 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer36->Add( m_textCtrl132, 0, wxALL, 5 );

	m_bitmapButton132 = new wxBitmapButton(m_tab_filebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer36->Add(m_bitmapButton132, 0, wxALIGN_CENTER | wxALL, 5);
	
	bSizer_filebackups->Add( bSizer36, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText12 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Maximal number of full file backups:"), wxDefaultPosition, left_column_size, 0 );
	m_staticText12->Wrap( -1 );
	bSizer37->Add( m_staticText12, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl133 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer37->Add( m_textCtrl133, 0, wxALL, 5 );

	m_bitmapButton133 = new wxBitmapButton(m_tab_filebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer37->Add(m_bitmapButton133, 0, wxALIGN_CENTER | wxALL, 5);
	
	bSizer_filebackups->Add( bSizer37, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText29 = new wxStaticText( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	bSizer38->Add( m_staticText29, 0, wxALL, 5 );
	
	bSizer_filebackups->Add( bSizer38, 0, wxEXPAND, 5 );
	
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		wxBoxSizer* bSizer38s;
		bSizer38s = new wxBoxSizer( wxVERTICAL );

		wxStaticText *m_staticText29s = new wxStaticText( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText29s->Wrap( -1 );
		bSizer38s->Add( m_staticText29s, 0, wxALL, 5 );
	
		bSizer_imagebackups->Add( bSizer38s, 0, wxEXPAND, 5 );

		wxBoxSizer* bSizer39;
		bSizer39 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText14 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Minimal number of incremental image backups:"), wxDefaultPosition, left_column_size, 0 );
		m_staticText14->Wrap( -1 );
		bSizer39->Add( m_staticText14, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl134 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer39->Add( m_textCtrl134, 0, wxALL, 5 );

		m_bitmapButton134 = new wxBitmapButton(m_tab_imagebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
		bSizer39->Add(m_bitmapButton134, 0, wxALIGN_CENTER | wxALL, 5);
	
		bSizer_imagebackups->Add( bSizer39, 0, wxEXPAND, 5 );
	
		wxBoxSizer* bSizer40;
		bSizer40 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText15 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Maximal number of incremental image backups:"), wxDefaultPosition, left_column_size, 0 );
		m_staticText15->Wrap( -1 );
		bSizer40->Add( m_staticText15, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl135 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer40->Add( m_textCtrl135, 0, wxALL, 5 );

		m_bitmapButton135 = new wxBitmapButton(m_tab_imagebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
		bSizer40->Add(m_bitmapButton135, 0, wxALIGN_CENTER | wxALL, 5);
	
		bSizer_imagebackups->Add( bSizer40, 0, wxEXPAND, 5 );
	
		wxBoxSizer* bSizer41;
		bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText16 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Minimal number of full image backups:"), wxDefaultPosition, left_column_size, 0 );
		m_staticText16->Wrap( -1 );
		bSizer41->Add( m_staticText16, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl136 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer41->Add( m_textCtrl136, 0, wxALL, 5 );

		m_bitmapButton136 = new wxBitmapButton(m_tab_imagebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
		bSizer41->Add(m_bitmapButton136, 0, wxALIGN_CENTER | wxALL, 5);
	
		bSizer_imagebackups->Add( bSizer41, 0, wxEXPAND, 5 );
	
		wxBoxSizer* bSizer42;
		bSizer42 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText17 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Maximal number of full image backups:"), wxDefaultPosition, left_column_size, 0 );
		m_staticText17->Wrap( -1 );
		bSizer42->Add( m_staticText17, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl137 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer42->Add( m_textCtrl137, 0, wxALL, 5 );

		m_bitmapButton137 = new wxBitmapButton(m_tab_imagebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
		bSizer42->Add(m_bitmapButton137, 0, wxALIGN_CENTER | wxALL, 5);
	
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
	
	m_staticText25 = new wxStaticText( m_tab_client, wxID_ANY, _("Computer name:"), wxDefaultPosition, left_column_size, 0 );
	m_staticText25->Wrap( -1 );
	bSizer28->Add( m_staticText25, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl15 = new wxTextCtrl( m_tab_client, wxID_ANY, wxEmptyString, wxDefaultPosition, string_input_size, 0 );
	bSizer28->Add( m_textCtrl15, 0, wxALL, 5 );
	
	bSizer_client->Add( bSizer28, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText26 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Exclude from backup (with wildcards):"), wxDefaultPosition, left_column_size, 0 );
	m_staticText26->Wrap( -1 );
	bSizer44->Add( m_staticText26, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl16 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, string_input_size, 0 );

	bSizer44->Add( m_textCtrl16, 0, wxALL, 5 );

	m_bitmapButton16 = new wxBitmapButton(m_tab_filebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer44->Add(m_bitmapButton16, 0, wxALIGN_CENTER | wxALL, 5);
	
	bSizer_filebackups->Add( bSizer44, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer441;
	bSizer441 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText261 = new wxStaticText( m_tab_filebackups, wxID_ANY, _("Include in backup (with wildcards):"), wxDefaultPosition, left_column_size, 0 );
	m_staticText261->Wrap( -1 );
	bSizer441->Add( m_staticText261, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl161 = new wxTextCtrl( m_tab_filebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, string_input_size, 0 );
	bSizer441->Add( m_textCtrl161, 0, wxALL, 5 );

	m_bitmapButton161 = new wxBitmapButton(m_tab_filebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer441->Add(m_bitmapButton161, 0, wxALIGN_CENTER | wxALL, 5);
	
	bSizer_filebackups->Add( bSizer441, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText27 = new wxStaticText( m_tab_client, wxID_ANY, _("Backup window:"), wxDefaultPosition, left_column_size, 0 );
	m_staticText27->Wrap( -1 );
	bSizer45->Add( m_staticText27, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl17 = new wxTextCtrl( m_tab_client, wxID_ANY, wxEmptyString, wxDefaultPosition, string_input_size, 0 );
	bSizer45->Add( m_textCtrl17, 0, wxALL, 5 );

	m_bitmapButton17 = new wxBitmapButton(m_tab_client, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer45->Add(m_bitmapButton17, 0, wxALIGN_CENTER | wxALL, 5);
	
	bSizer_client->Add( bSizer45, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer331;
	bSizer331 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText281 = new wxStaticText( m_tab_client, wxID_ANY, _("Backup delay after system start:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText281->Wrap( -1 );
	m_staticText281->SetMinSize( left_column_size );
	
	bSizer331->Add( m_staticText281, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl19 = new wxTextCtrl( m_tab_client, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer331->Add( m_textCtrl19, 0, wxALL, 5 );
	
	m_staticText291 = new wxStaticText( m_tab_client, wxID_ANY, _("min"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText291->Wrap( -1 );

	m_bitmapButton19 = new wxBitmapButton(m_tab_client, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer331->Add(m_bitmapButton19, 0, wxALIGN_CENTER | wxALL, 5);

	bSizer331->Add( m_staticText291, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer_client->Add( bSizer331, 0, wxEXPAND, 5 );

#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		wxBoxSizer* bSizer441;
		bSizer441 = new wxBoxSizer( wxHORIZONTAL );
	
		m_staticText301 = new wxStaticText( m_tab_imagebackups, wxID_ANY, _("Volumes to backup:"), wxDefaultPosition, left_column_size, 0 );
		m_staticText301->Wrap( -1 );
		bSizer441->Add( m_staticText301, 0, wxALIGN_CENTER|wxALL, 5 );
	
		m_textCtrl23 = new wxTextCtrl( m_tab_imagebackups, wxID_ANY, wxEmptyString, wxDefaultPosition, string_input_size, 0 );
		bSizer441->Add( m_textCtrl23, 0, wxALL, 5 );

		m_bitmapButton23 = new wxBitmapButton(m_tab_imagebackups, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
		bSizer441->Add(m_bitmapButton23, 0, wxALIGN_CENTER | wxALL, 5);
	
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
	m_staticTextLocalSpeed = new wxStaticText( m_tab_client, wxID_ANY, _("Max backup speed for local network:"), wxDefaultPosition, left_column_size, 0 );
	m_staticTextLocalSpeed->Wrap( -1 );
	bSizerH->Add( m_staticTextLocalSpeed, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlLocalSpeed = new wxTextCtrl( m_tab_client, wxID_ANY, wxEmptyString, wxDefaultPosition, number_input_size, 0 );
	bSizerH->Add( m_textCtrlLocalSpeed, 0, wxALL, 5 );	
	m_staticTextLocalSpeedUnit = new wxStaticText( m_tab_client, wxID_ANY, _("MBit/s"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticTextLocalSpeedUnit->Wrap( -1 );
	bSizerH->Add( m_staticTextLocalSpeedUnit, 0, wxALIGN_CENTER|wxALL, 5 );	

	m_bitmapButtonLocalSpeed = new wxBitmapButton(m_tab_client, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizerH->Add(m_bitmapButtonLocalSpeed, 0, wxALIGN_CENTER | wxALL, 5);

	bSizer_client->Add( bSizerH, 0, wxEXPAND, 5 );


	//Internet tab
	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetEnabled = new wxStaticText( m_tab_internet, wxID_ANY, _("Enable backup via internet:"), wxDefaultPosition, left_column_size, 0 );
	m_staticTextInternetEnabled->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetEnabled, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_checkBoxInternetEnabled = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerH->Add( m_checkBoxInternetEnabled, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticInternetServer = new wxStaticText( m_tab_internet, wxID_ANY, _("Internet server name/IP:"), wxDefaultPosition, left_column_size, 0 );
	m_staticInternetServer->Wrap( -1 );
	bSizerH->Add( m_staticInternetServer, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlInternetServer = new wxTextCtrl( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, string_input_size, 0 );
	bSizerH->Add( m_textCtrlInternetServer, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticInternetServerPort = new wxStaticText( m_tab_internet, wxID_ANY, _("Internet server port:"), wxDefaultPosition, left_column_size, 0 );
	m_staticInternetServerPort->Wrap( -1 );
	bSizerH->Add( m_staticInternetServerPort, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlInternetServerPort = new wxTextCtrl( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, number_input_size, 0 );
	bSizerH->Add( m_textCtrlInternetServerPort, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer(wxHORIZONTAL);
	m_staticInternetServerProxy = new wxStaticText(m_tab_internet, wxID_ANY, _("Internet server HTTP(s) proxy:"), wxDefaultPosition, left_column_size, 0);
	m_staticInternetServerProxy->Wrap(-1);
	bSizerH->Add(m_staticInternetServerProxy, 0, wxALIGN_CENTER | wxALL, 5);
	m_textCtrlInternetServerProxy = new wxTextCtrl(m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, string_input_size, 0);
	bSizerH->Add(m_textCtrlInternetServerProxy, 0, wxALL, 5);
	bSizer_internet->Add(bSizerH, 0, wxEXPAND, 5);

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticInternetServerAuthkey = new wxStaticText( m_tab_internet, wxID_ANY, _("Internet server password:"), wxDefaultPosition, left_column_size, 0 );
	m_staticInternetServerAuthkey->Wrap( -1 );
	bSizerH->Add( m_staticInternetServerAuthkey, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlInternetServerAuthkey = new wxTextCtrl( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, string_input_size, 0 );
	bSizerH->Add( m_textCtrlInternetServerAuthkey, 0, wxALL, 5 );	
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		bSizerH = new wxBoxSizer( wxHORIZONTAL );	
		m_staticTextInternetImage = new wxStaticText( m_tab_internet, wxID_ANY, _("Do image backups over internet:"), wxDefaultPosition, left_column_size, 0 );
		m_staticTextInternetImage->Wrap( -1 );
		bSizerH->Add( m_staticTextInternetImage, 0, wxALIGN_CENTER|wxALL, 5 );	
		m_checkBoxInternetImage = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizerH->Add( m_checkBoxInternetImage, 0, wxALL, 5 );	
		m_bitmapButtonInternetImage = new wxBitmapButton(m_tab_internet, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
		bSizerH->Add(m_bitmapButtonInternetImage, 0, wxALIGN_CENTER | wxALL, 5);
		bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );
	}
#endif

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetFullFile = new wxStaticText( m_tab_internet, wxID_ANY, _("Do full file backups over internet:"), wxDefaultPosition, left_column_size, 0 );
	m_staticTextInternetFullFile->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetFullFile, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_checkBoxInternetFullFile = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerH->Add( m_checkBoxInternetFullFile, 0, wxALL, 5 );	
	m_bitmapButtonInternetFullFile = new wxBitmapButton(m_tab_internet, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizerH->Add(m_bitmapButtonInternetFullFile, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetSpeed = new wxStaticText( m_tab_internet, wxID_ANY, _("Max backups speed for internet connection:"), wxDefaultPosition, left_column_size, 0 );
	m_staticTextInternetSpeed->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetSpeed, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_textCtrlInternetSpeed = new wxTextCtrl( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, number_input_size, 0 );
	bSizerH->Add( m_textCtrlInternetSpeed, 0, wxALL, 5 );	
	m_staticTextInternetSpeedUnit = new wxStaticText( m_tab_internet, wxID_ANY, _("KBit/s"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticTextInternetSpeedUnit->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetSpeedUnit, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_bitmapButtonInternetSpeed = new wxBitmapButton(m_tab_internet, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizerH->Add(m_bitmapButtonInternetSpeed, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetCompress = new wxStaticText( m_tab_internet, wxID_ANY, _("Compressed transfer:"), wxDefaultPosition, left_column_size, 0 );
	m_staticTextInternetCompress->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetCompress, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_checkBoxInternetCompress = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerH->Add( m_checkBoxInternetCompress, 0, wxALL, 5 );	
	m_bitmapButtonInternetCompress = new wxBitmapButton(m_tab_internet, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizerH->Add(m_bitmapButtonInternetCompress, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer_internet->Add( bSizerH, 0, wxEXPAND, 5 );

	bSizerH = new wxBoxSizer( wxHORIZONTAL );	
	m_staticTextInternetEncrypt = new wxStaticText( m_tab_internet, wxID_ANY, _("Encrypted transfer:"), wxDefaultPosition, left_column_size, 0 );
	m_staticTextInternetEncrypt->Wrap( -1 );
	bSizerH->Add( m_staticTextInternetEncrypt, 0, wxALIGN_CENTER|wxALL, 5 );	
	m_checkBoxInternetEncrypt = new wxCheckBox( m_tab_internet, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerH->Add( m_checkBoxInternetEncrypt, 0, wxALL, 5 );	
	m_bitmapButtonInternetEncrypt = new wxBitmapButton(m_tab_internet, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizerH->Add(m_bitmapButtonInternetEncrypt, 0, wxALIGN_CENTER | wxALL, 5);
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
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
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
	
	bSizer4->SetMinSize( wxDLG_UNIT(this,wxSize( -1,26 )) ); 
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->SetMinSize(wxDLG_UNIT(this, wxSize( -1,13 )) );
	
	bSizer4->Add( m_button1, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->SetMinSize(wxDLG_UNIT(this, wxSize( -1,13 )) );
	
	bSizer4->Add( m_button2, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	bSizer1->Add( bSizer4, 0, wxEXPAND|wxFIXED_MINSIZE, 5 );
	
	this->SetSizerAndFit( bSizer1 );
	this->Layout();	
	this->Centre( wxBOTH );
	
	// Connect Events
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		m_checkBox1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( GUISettings::OnDisableImageBackups ), NULL, this );
	}
#endif
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISettings::OnOkClick ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUISettings::OnAbortClick ), NULL, this );

	m_bitmapButton1->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButton2->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
#ifdef _WIN32
	if (!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		m_bitmapButton21->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
		m_bitmapButton22->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
		m_bitmapButton23->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
		m_bitmapButton134->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
		m_bitmapButton135->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
		m_bitmapButton136->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
		m_bitmapButton137->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
		m_bitmapButtonInternetImage->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	}
#endif
	m_bitmapButton13->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButton131->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButton132->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButton133->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	
	m_bitmapButton16->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButton161->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButton17->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButton19->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	
	m_bitmapButtonLocalSpeed->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButtonInternetFullFile->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	
	m_bitmapButtonInternetSpeed->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButtonInternetCompress->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);
	m_bitmapButtonInternetEncrypt->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUISettings::OnBitmapBtnClick), NULL, this);

#ifdef _WIN32
	if (!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		m_textCtrl21->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
		m_textCtrl22->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
		m_textCtrl23->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
		m_textCtrl134->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
		m_textCtrl135->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
		m_textCtrl136->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
		m_textCtrl137->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
		m_checkBoxInternetImage->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	}
#endif
	m_textCtrl1->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_textCtrl2->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	
	m_textCtrl13->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_textCtrl131->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_textCtrl132->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_textCtrl133->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	
	m_textCtrl16->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_textCtrl161->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_textCtrl17->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_textCtrl19->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	
	m_textCtrlLocalSpeed->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_checkBoxInternetFullFile->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	
	m_textCtrlInternetSpeed->Connect(wxEVT_TEXT, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_checkBoxInternetCompress->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
	m_checkBoxInternetEncrypt->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(GUISettings::OnCtlChange), NULL, this);
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
	
	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( res_path+wxT("logo1.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_bitmap1->SetMinSize( wxDLG_UNIT(this, wxSize( 50,41 )) );
	
	bSizer25->Add( m_bitmap1, 0, wxALL, 5 );
	
	
	bSizer25->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxSizer* bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, _("Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	bSizer26->Add( m_staticText21, 0, wxALL, 5 );
	
	m_versionSizer = new wxBoxSizer(wxHORIZONTAL);

	m_staticText22 = new wxStaticText( this, wxID_ANY, wxT("    ")+wxString(c_client_version), wxDefaultPosition, wxDefaultSize, wxEXPAND);
	m_staticText22->Wrap( -1 );

	m_versionSizer->Add( m_staticText22, 0, wxALL, 5 );

	bSizer26->Add(m_versionSizer);
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	bSizer26->Add( m_staticText23, 0, wxALL, 5 );
	
	m_textCtrl14 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this, wxSize( 220,200 )), wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL );
	bSizer26->Add( m_textCtrl14, 0, wxALL, 5 );
	
	bSizer25->Add( bSizer26, 5, wxEXPAND, 5 );
	
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
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler( GUIInfo::OnCloseInt ), NULL, this);
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
	listbox->SetMinSize( wxDLG_UNIT(this, wxSize( 300,200 )) );
	
	bSizer30->Add( listbox, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText27 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	m_staticText27->SetMinSize( wxSize( -1,-1 ) );
	
	bSizer32->Add( m_staticText27, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl18 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl18->Enable( false );
	m_textCtrl18->SetMinSize(wxDLG_UNIT(this, wxSize( 100,-1 )) );

	wxSize image_button_size = wxDLG_UNIT(this, wxSize(15, 13));
	wxSize size_sub = wxDLG_UNIT(this, wxSize(5, 5));

	wxBitmap fa_home;
#ifdef _WIN32
	fa_home.LoadFile("fa-home.png", wxBITMAP_TYPE_PNG);
#else
	fa_home.LoadFile(res_path+wxT("fa-home.png"), wxBITMAP_TYPE_PNG);
#endif
	fa_home_img_scaled = fa_home.ConvertToImage().Scale(image_button_size.GetWidth() - size_sub.GetWidth(), image_button_size.GetHeight() - size_sub.GetHeight(), wxIMAGE_QUALITY_BILINEAR);

	wxBitmap fa_client;
#ifdef _WIN32
	fa_client.LoadFile("fa-road.png", wxBITMAP_TYPE_PNG);
#else
	fa_client.LoadFile(res_path+wxT("fa-road.png"), wxBITMAP_TYPE_PNG);
#endif
	fa_client_img_scaled = fa_client.ConvertToImage().Scale(image_button_size.GetWidth() - size_sub.GetWidth(), image_button_size.GetHeight() - size_sub.GetHeight(), wxIMAGE_QUALITY_BILINEAR);

	wxBitmap fa_lock;
#ifdef _WIN32
	fa_lock.LoadFile("fa-lock.png", wxBITMAP_TYPE_PNG);
#else
	fa_lock.LoadFile(res_path+wxT("fa-lock.png"), wxBITMAP_TYPE_PNG);
#endif
	fa_lock_img_scaled = fa_lock.ConvertToImage().Scale(image_button_size.GetWidth() - size_sub.GetWidth(), image_button_size.GetHeight() - size_sub.GetHeight(), wxIMAGE_QUALITY_BILINEAR);

	wxBitmap fa_copy;
#ifdef _WIN32
	fa_copy.LoadFile("fa-copy.png", wxBITMAP_TYPE_PNG);
#else
	fa_copy.LoadFile(res_path+wxT("fa-copy.png"), wxBITMAP_TYPE_PNG);
#endif
	fa_copy_img_scaled = fa_copy.ConvertToImage().Scale(image_button_size.GetWidth() - size_sub.GetWidth(), image_button_size.GetHeight() - size_sub.GetHeight(), wxIMAGE_QUALITY_BILINEAR);

	bSizer32->Add( m_textCtrl18, 0, wxALL, 5 );

	bSizer32->Add(0, 0, 1, wxEXPAND, 5);

	m_bitmapButton1 = new wxBitmapButton(this, wxID_ANY, fa_home_img_scaled, wxPoint(-1, -1), image_button_size, 0);
	bSizer32->Add(m_bitmapButton1, 0, wxALIGN_CENTER | wxALL, 5);

	/*m_staticTextGroup = new wxStaticText( this, wxID_ANY, _("Gruppe:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextGroup->Wrap( -1 );
	m_staticTextGroup->SetMinSize( wxSize( -1,-1 ) );

	bSizer32->Add( m_staticTextGroup, 0, wxALIGN_CENTER|wxALL, 5 );

	wxString choices[] =
	{
		_("Default"),
		_("Continuous")
	};

	m_group = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choices);
	m_group->SetMinSize(wxSize(140, -1));
	m_group->Enable(false);

	bSizer32->Add( m_group, 0, wxALL, 5 );*/
	
	
	bSizer30->Add( bSizer32, 0, wxEXPAND, 5 );
	
	
	bSizer30->Add( 0, 20, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button5 = new wxButton( this, wxID_ANY, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_button5, 0, wxALL, 5 );
	
	m_button6 = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
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
	m_bitmapButton1->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUIConfigPath::OnClickSourceSwitch), NULL, this);
	//m_group->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( GUIConfigPath::OnGroupChange ), NULL, this );
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

GUIStatus::GUIStatus( wxWindow* parent, wxLongLong_t follow_only_process_id, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
	, follow_only_process_id(follow_only_process_id)
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	bSizer34 = new wxBoxSizer( wxVERTICAL );

	resizeForProcesses(1);

	if (follow_only_process_id == 0)
	{
		wxBoxSizer* bSizer12;
		bSizer12 = new wxBoxSizer(wxHORIZONTAL);

		m_staticText37 = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
		m_staticText37->Wrap(-1);
		bSizer12->Add(m_staticText37, 0, wxALL, 5);

		bSizer34->Add(bSizer12, 0, wxEXPAND, 5);

		wxBoxSizer* bSizer35;
		bSizer35 = new wxBoxSizer(wxHORIZONTAL);

		m_staticText32 = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
		m_staticText32->Wrap(-1);
		bSizer35->Add(m_staticText32, 0, wxALL, 5);

		m_staticText33 = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
		m_staticText33->Wrap(-1);
		bSizer35->Add(m_staticText33, 0, wxALL, 5);


		bSizer34->Add(bSizer35, 0, wxEXPAND, 5);

		m_staticText36 = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
		m_staticText36->Wrap(-1);
		bSizer34->Add(m_staticText36, 0, wxALL, 5);

		wxBoxSizer* bSizer37;
		bSizer37 = new wxBoxSizer(wxHORIZONTAL);

		m_staticText34 = new wxStaticText(this, wxID_ANY, _("Internet connection status:"), wxDefaultPosition, wxDefaultSize, 0);
		m_staticText34->Wrap(-1);
		bSizer37->Add(m_staticText34, 0, wxALL, 5);

		m_staticText35 = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
		m_staticText35->Wrap(-1);
		bSizer37->Add(m_staticText35, 0, wxALL, 5);


		bSizer34->Add(bSizer37, 0, wxEXPAND, 5);
	}
	else
	{
		this->SetTitle(_("UrBackup - File restore"));
	}
	
	
	this->SetSizer( bSizer34 );
	this->Layout();
	bSizer34->Fit( this );
	
	this->Centre( wxBOTH );

	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler( GUIStatus::OnCloseInt ), NULL, this);
}

void GUIStatus::removeCurrentProcesses(size_t new_size)
{
	for (size_t i = new_size; i < m_processItem.size(); ++i)
	{
		bSizer34->Remove(m_processItem[i].bSizer36);
		bSizer34->Remove(m_processItem[i].bSizer362);
		bSizer34->Detach(m_processItem[i].m_gauge1);
		bSizer34->Detach(m_processItem[i].m_staticLine);
		m_processItem[i].m_gauge1->Destroy();
		m_processItem[i].m_staticText31->Destroy();
		m_processItem[i].m_staticText312->Destroy();
		m_processItem[i].m_staticLine->Destroy();
	}
	m_processItem.resize(new_size);
}

void GUIStatus::resizeForProcesses(size_t new_size)
{
	for (size_t i = m_processItem.size(); i < new_size; ++i)
	{
		SProcessItem process_item;
		process_item.bSizer36 = new wxBoxSizer(wxHORIZONTAL);

		process_item.m_staticText31 = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
		process_item.m_staticText31->Wrap(-1);
		process_item.bSizer36->Add(process_item.m_staticText31, 0, wxALL, 5);

		bSizer34->Insert(i * 4 + 0,process_item.bSizer36, 0, wxEXPAND, 5);

		process_item.bSizer362 = new wxBoxSizer(wxHORIZONTAL);

		process_item.m_staticText312 = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
		process_item.m_staticText312->Wrap(-1);
		process_item.bSizer362->Add(process_item.m_staticText312, 0, wxALL, 5);

		bSizer34->Insert(i * 4 + 1, process_item.bSizer362, 0, wxEXPAND, 5);

		process_item.m_gauge1 = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
		process_item.m_gauge1->SetMinSize(wxDLG_UNIT(this, wxSize(290, -1)));
		process_item.m_gauge1->SetRange(100);
		bSizer34->Insert(i * 4 + 2, process_item.m_gauge1, 0, wxEXPAND|wxALL, 5);

		if (follow_only_process_id == 0)
		{
			process_item.m_staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);

			bSizer34->Insert(i * 4 + 3, process_item.m_staticLine, 0, wxEXPAND, 5);
		}

		m_processItem.push_back(process_item);
	}
}

void GUIStatus::relayout()
{
	this->Layout();
	bSizer34->Fit(this);
}

GUIStatus::~GUIStatus()
{
}

void GUIStatus::OnCloseInt(wxCloseEvent& event)
{
	event.Skip();
	OnClose();
}

GUICbtStatus::GUICbtStatus(wxWindow * parent, wxWindowID id, const wxString & title, const wxPoint & pos, const wxSize & size, long style)
	: wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);

	m_textCtrl3 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);
	bSizer9->Add(m_textCtrl3, 15, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer(wxHORIZONTAL);


	bSizer10->Add(0, 0, 1, wxEXPAND, 5);

	m_button5 = new wxButton(this, wxID_ANY, _("Exit"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer10->Add(m_button5, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	bSizer9->Add(bSizer10, 1, wxEXPAND | wxALIGN_RIGHT, 5);

	this->SetSizer(bSizer9);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	m_button5->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUICbtStatus::OnExitClick), NULL, this);
}

GUICbtStatus::~GUICbtStatus()
{
	m_button5->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GUICbtStatus::OnExitClick), NULL, this);
}
