#include "WelcomePage/WelcomePage.hpp"


WelcomePage::WelcomePage( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
	this->SetBackgroundColour( wxColour( 255, 240, 250 ) );

	wxBoxSizer* Area;
	Area = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* Context;
	Context = new wxBoxSizer( wxVERTICAL );

	HelpingText = new wxStaticText( this, wxID_ANY, _("Greetings!!"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE );
	HelpingText->Wrap( 300 );
	HelpingText->SetFont( wxFont( 22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia") ) );

	Context->Add( HelpingText, 0, wxALL, 5 );


	Area->Add( Context, 1, wxALIGN_CENTER|wxALL, 100 );


	this->SetSizer( Area );
	this->Layout();

	this->Centre( wxBOTH );
}

WelcomePage::~WelcomePage()
{
}
