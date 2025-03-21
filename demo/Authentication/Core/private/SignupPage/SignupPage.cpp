#include "SignupPage/SignupPage.hpp"
SignUp::SignUp( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints(wxSize(1200, 800), wxSize(1200, 800));
	this->SetBackgroundColour( wxColour( 255, 240, 255 ) );

	wxBoxSizer* Area;
	Area = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* Context;
	Context = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* Name;
	Name = new wxBoxSizer( wxHORIZONTAL );

	NameLabel = new wxStaticText( this, wxID_ANY, _("What is your name? :"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	NameLabel->Wrap( 300 );
	NameLabel->SetFont( wxFont( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia") ) );

	Name->Add( NameLabel, 0, wxALL|wxEXPAND, 5 );

	m_textCtrl2 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	Name->Add( m_textCtrl2, 1, wxALL|wxEXPAND, 5 );


	Context->Add( Name, 0, wxALL|wxEXPAND, 5 );

	HelpingText = new wxStaticText( this, wxID_ANY, _("Start entering text that you see below:"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE );
	HelpingText->Wrap( 300 );
	HelpingText->SetFont( wxFont( 22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia") ) );

	Context->Add( HelpingText, 0, wxALL, 5 );


	Context->Add( 1, 30, 0, wxEXPAND, 5 );

	Text = new wxStaticText( this, wxID_ANY, _("<<There will be a text every 10 second from know>>"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE );
	Text->Wrap( 300 );
	Text->SetFont( wxFont( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia") ) );

	Context->Add( Text, 0, wxALL, 5 );

	EnterArea = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxALWAYS_SHOW_SB|wxBORDER_NONE|wxHSCROLL|wxVSCROLL|wxWANTS_CHARS );
	Context->Add( EnterArea, 1, wxALL|wxEXPAND, 5 );

	LogIN_BTN = new wxButton( this, wxID_ANY, _("SignUp"), wxDefaultPosition, wxSize( 200,30 ), wxBORDER_NONE );

	LogIN_BTN->SetDefault();
	LogIN_BTN->SetLabelMarkup( _("SignUp") );
	LogIN_BTN->SetFont( wxFont( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );
	LogIN_BTN->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	LogIN_BTN->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );

	Context->Add( LogIN_BTN, 0, wxALL|wxEXPAND, 5 );


	Area->Add( Context, 1, wxALIGN_CENTER|wxALL, 100 );


	this->SetSizer( Area );
	this->Layout();

	this->Centre( wxBOTH );
}

SignUp::~SignUp()
{
}