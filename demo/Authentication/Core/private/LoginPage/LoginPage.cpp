#include "LoginPage/LoginPage.hpp"
#include "HomePage/HomePage.hpp"
Login::Login(wxWindow* parent, wxWindowID id, const wxString& title, 
             const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxSize(1200, 800), wxSize(1200, 800));
    this->SetPosition(wxPoint(1200-600,800-400));
    this->SetBackgroundColour(wxColour(255, 240, 255));

    wxBoxSizer* Area = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* BackSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* Context = new wxBoxSizer(wxVERTICAL);

    Back = new wxButton(this, wxID_ANY, wxT("<"), wxDefaultPosition, wxSize(25, 25), wxBORDER_NONE);
    Back->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
    Back->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    Back->SetBackgroundColour(wxColour(215, 0, 5));
    BackSizer->Add(Back, 0, wxALIGN_TOP | wxALL, 5);
    HelpingText = new wxStaticText(this, wxID_ANY, _("Start entering text that you see below:"), 
                                  wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    HelpingText->Wrap(300);
    HelpingText->SetFont(wxFont(22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                               wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Context->Add(HelpingText, 0, wxALL, 5);

    Context->Add(1, 20, 0, wxEXPAND, 5);

    Text = new wxStaticText(this, wxID_ANY, _("<<There will be a text every 10 second from now>>"), 
                           wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    Text->Wrap(300);
    Text->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                        wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Context->Add(Text, 0, wxALL, 5);

    EnterArea = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, 
                                  wxDefaultPosition, wxDefaultSize, 
                                  wxTE_PROCESS_ENTER | wxALWAYS_SHOW_SB | wxBORDER_NONE | 
                                  wxHSCROLL | wxVSCROLL | wxWANTS_CHARS);
    Context->Add(EnterArea, 1, wxALL | wxEXPAND, 5);

    LogIN_BTN = new wxButton(this, wxID_ANY, _("Login"), 
                            wxDefaultPosition, wxSize(200, 30), wxBORDER_NONE);
    LogIN_BTN->SetDefault();
    LogIN_BTN->SetLabelMarkup(_("Login"));
    LogIN_BTN->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                             wxFONTWEIGHT_BOLD, false, wxT("Arial")));
    LogIN_BTN->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    LogIN_BTN->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    Context->Add(LogIN_BTN, 0, wxALL | wxEXPAND, 5);

    Area->Add(BackSizer, 0, wxALIGN_LEFT | wxALL, 5);
    Area->Add(Context, 1, wxALIGN_CENTER | wxALL, 100);

    this->SetSizer(Area);
    this->Layout();
    this->Centre(wxBOTH);

    Back->Bind(wxEVT_ENTER_WINDOW, &Login::OnBackEnter, this);
    Back->Bind(wxEVT_LEAVE_WINDOW, &Login::OnBackLeave, this);
    Back->Bind(wxEVT_BUTTON, &Login::OnBackClicked, this);
}

Login::~Login()
{
}

void Login::OnBackEnter(wxMouseEvent& event){
    if (Back)
    {
        Back->SetBackgroundColour(wxColour(135, 206, 250)); // Light sky blue on hover
        Back->SetForegroundColour(wxColour(0, 0, 0));       // Black text for contrast
        Back->Refresh();
    }
    event.Skip();
}

void Login::OnBackLeave(wxMouseEvent& event){
    if (Back)
    {
        Back->SetBackgroundColour(wxColour(215, 0, 5)); // Original color
        Back->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        Back->Refresh();
    }
    event.Skip();
}

void Login::OnBackClicked(wxCommandEvent& event){
    HomePage* HomeWindow = new HomePage(nullptr, wxID_ANY, _("Home"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
    HomeWindow->Show(true);
    this->Close(true);
}