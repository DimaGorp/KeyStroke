#include "HomePage/HomePage.hpp"
#include "LogInPage/LoginPage.hpp"
#include "SignUpPage/SignupPage.hpp"
HomePage::HomePage( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints(wxSize(1200, 800), wxSize(1200, 800));
	this->SetBackgroundColour( wxColour( 255, 240, 255 ) );

	wxBoxSizer* Area;
	Area = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* Context;
	Context = new wxBoxSizer( wxVERTICAL );

	Welcome_Text = new wxStaticText( this, wxID_ANY, _("Welcome,User"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	Welcome_Text->SetLabelMarkup( _("Welcome,User") );
	Welcome_Text->Wrap( -1 );
	Welcome_Text->SetFont( wxFont( 32, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false, wxT("Century Gothic") ) );

	Context->Add(Welcome_Text, 0, wxEXPAND | wxALL, 10);

	Description = new wxStaticText( this, wxID_ANY, _("That is a demonstration project of techic \"Keystroke dynamics\" as  a part of Behavioral Biometrics Authentication topic"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	Description->Wrap( 300 );
	Description->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia") ) );

	Context->Add(Description, 0, wxEXPAND | wxALL, 10);

	wxBoxSizer* Buttons;
	Buttons = new wxBoxSizer( wxHORIZONTAL );

	LogIN_BTN = new wxButton( this, wxID_ANY, _("Login"), wxDefaultPosition, wxSize( 200,30 ) , wxBORDER_NONE);

	LogIN_BTN->SetDefault();
	LogIN_BTN->SetLabelMarkup( _("Login") );
	LogIN_BTN->SetFont( wxFont( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );
	LogIN_BTN->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	LogIN_BTN->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );

	Buttons->Add( LogIN_BTN, 0, wxALL, 5 );


	Buttons->AddStretchSpacer(1);

	SignUpBtn = new wxButton( this, wxID_ANY, _("SignUp"), wxDefaultPosition, wxSize( 200,30 ), wxBORDER_NONE );
	SignUpBtn->SetLabelMarkup( _("SignUp") );
	SignUpBtn->SetFont( wxFont( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );
	SignUpBtn->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	SignUpBtn->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );

	Buttons->Add( SignUpBtn, 0, wxALL, 5 );


	Context->Add(Buttons, 0, wxEXPAND | wxTOP, 30);


	Area->Add( Context, 1, wxALIGN_CENTER|wxALL, 100 );


	this->SetSizer( Area );
	this->Layout();

	this->Centre( wxBOTH );
	LogIN_BTN->Bind(wxEVT_ENTER_WINDOW, &HomePage::OnLoginEnter, this);
    LogIN_BTN->Bind(wxEVT_LEAVE_WINDOW, &HomePage::OnLoginLeave, this);
    SignUpBtn->Bind(wxEVT_ENTER_WINDOW, &HomePage::OnSignUpEnter, this);
    SignUpBtn->Bind(wxEVT_LEAVE_WINDOW, &HomePage::OnSignUpLeave, this);
	// Bind click events
    LogIN_BTN->Bind(wxEVT_BUTTON, &HomePage::OnLoginClick, this);
    SignUpBtn->Bind(wxEVT_BUTTON, &HomePage::OnSignUpClick, this);
}

HomePage::~HomePage()
{
}


void HomePage::OnLoginEnter(wxMouseEvent& event) {
	LogIN_BTN->SetBackgroundColour(wxColour(135, 206, 250)); // Light sky blue on hover
	LogIN_BTN->SetForegroundColour(wxColour(0, 0, 0));       // Black text for contrast
	LogIN_BTN->Refresh();
}
void HomePage::OnLoginLeave(wxMouseEvent& event) {
	LogIN_BTN->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)); // Original color
	LogIN_BTN->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
	LogIN_BTN->Refresh();
}
void HomePage::OnSignUpEnter(wxMouseEvent& event) {
	SignUpBtn->SetBackgroundColour(wxColour(173, 216, 230)); // Light blue on hover
	SignUpBtn->SetForegroundColour(wxColour(0, 0, 0));       // Black text for contrast
	SignUpBtn->Refresh();
}
void HomePage::OnSignUpLeave(wxMouseEvent& event) {
	SignUpBtn->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND)); // Original color
	SignUpBtn->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	SignUpBtn->Refresh();
}


void HomePage::OnLoginClick(wxCommandEvent& event)
{
	Login* loginWindow = new Login(nullptr, wxID_ANY, _("Login Window"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
	loginWindow->Show(true);
	this->Close(true);
}

void HomePage::OnSignUpClick(wxCommandEvent& event)
{
	SignUp* signupWindow = new SignUp(nullptr, wxID_ANY, _("Login Window"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
	signupWindow->Show(true);
	this->Close(true);
}