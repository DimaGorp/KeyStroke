#include <wx/wx.h>
#include <wx/log.h>
#include "LoginPage/LoginPage.hpp"
#include "WelcomePage/WelcomePage.hpp"
#include "HomePage/HomePage.hpp"
#include <wx/filefn.h>

// Define MIN_SAMPLES (match this to SignUp)
const size_t MIN_SAMPLES = 10; // Example; adjust if different in SignUp

BEGIN_EVENT_TABLE(Login, wxFrame)
    EVT_BUTTON(wxID_ANY, Login::OnBackClicked)
    EVT_KEY_DOWN(Login::OnKeyDown)
    EVT_KEY_UP(Login::OnKeyUp)
    EVT_TEXT_ENTER(wxID_ANY, Login::OnEnterPressed)
END_EVENT_TABLE()

Login::Login(wxWindow* parent, wxWindowID id, const wxString& title, 
             const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style), currentTextIndex(0), storedGmm(2) {
    wxLog::SetActiveTarget(new wxLogStderr());
    wxLogMessage("Login window initialized");

    this->SetSizeHints(wxSize(1200, 800), wxSize(1200, 800));
    this->SetPosition(wxPoint(1200-600, 800-400));
    this->SetBackgroundColour(wxColour(255, 240, 255));

    wxBoxSizer* Area = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* BackSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* Context = new wxBoxSizer(wxVERTICAL);

    Back = new wxButton(this, wxID_ANY, wxT("<"), wxDefaultPosition, wxSize(25, 25), wxBORDER_NONE);
    Back->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
    Back->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    Back->SetBackgroundColour(wxColour(215, 0, 5));
    BackSizer->Add(Back, 0, wxALIGN_TOP | wxALL, 5);

    wxBoxSizer* UsernameSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* usernameLabel = new wxStaticText(this, wxID_ANY, _("Username:"), 
                                                  wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    usernameLabel->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                                 wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    UsernameSizer->Add(usernameLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    usernameInput = new wxTextCtrl(this, wxID_ANY, wxEmptyString, 
                                  wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    UsernameSizer->Add(usernameInput, 1, wxALL | wxEXPAND, 5);
    Context->Add(UsernameSizer, 0, wxALL | wxEXPAND, 5);

    HelpingText = new wxStaticText(this, wxID_ANY, _("Type the text below and press Enter:"), 
                                  wxDefaultPosition, wxSize(600, -1), wxST_NO_AUTORESIZE);
    HelpingText->SetFont(wxFont(22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                               wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    HelpingText->Wrap(600);
    Context->Add(HelpingText, 0, wxALL, 5);

    Context->AddSpacer(20);

    textSamples = {
        _("The quick brown fox jumps over the lazy dog"),
        _("Pack my box with five dozen liquor jugs"),
        _("How razorback jumping frogs can level six piqued gymnasts"),
        _("Crazy Fredrick bought many very exquisite opal jewels")
    };
    Text = new wxStaticText(this, wxID_ANY, textSamples[0], 
                           wxDefaultPosition, wxSize(600, -1), wxST_NO_AUTORESIZE);
    Text->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                        wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Text->Wrap(600);
    Context->Add(Text, 0, wxALL, 5);

    EnterArea = new wxTextCtrl(this, wxID_ANY, wxEmptyString, 
                              wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    Context->Add(EnterArea, 1, wxALL | wxEXPAND, 5);

    Area->Add(BackSizer, 0, wxALIGN_LEFT | wxALL, 5);
    Area->Add(Context, 1, wxALIGN_CENTER | wxALL, 100);

    this->SetSizer(Area);
    this->Layout();
    this->Centre(wxBOTH);

    Back->Bind(wxEVT_ENTER_WINDOW, &Login::OnBackEnter, this);
    Back->Bind(wxEVT_LEAVE_WINDOW, &Login::OnBackLeave, this);
    Back->Bind(wxEVT_BUTTON, &Login::OnBackClicked, this);
    EnterArea->Bind(wxEVT_KEY_DOWN, &Login::OnKeyDown, this);
    EnterArea->Bind(wxEVT_KEY_UP, &Login::OnKeyUp, this);
    EnterArea->Bind(wxEVT_TEXT_ENTER, &Login::OnEnterPressed, this);
    usernameInput->Bind(wxEVT_TEXT_ENTER, &Login::OnUsernameEnter, this);
}

Login::~Login() {}

void Login::OnBackEnter(wxMouseEvent& event) {
    if (Back) {
        Back->SetBackgroundColour(wxColour(135, 206, 250));
        Back->SetForegroundColour(wxColour(0, 0, 0));
        Back->Refresh();
    }
    event.Skip();
}

void Login::OnBackLeave(wxMouseEvent& event) {
    if (Back) {
        Back->SetBackgroundColour(wxColour(215, 0, 5));
        Back->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        Back->Refresh();
    }
    event.Skip();
}

void Login::OnBackClicked(wxCommandEvent& event) {
    wxLogMessage("Back button clicked, returning to HomePage");
    HomePage* homeWindow = new HomePage(nullptr, wxID_ANY, _("Home"));
    homeWindow->Show(true);
    this->Close(true);
}

void Login::OnUsernameEnter(wxCommandEvent& event) {
    LoadGMMFromUsername();
}

void Login::LoadGMMFromUsername() {
    wxString username = usernameInput->GetValue().Trim();
    if (username.IsEmpty()) {
        wxMessageBox(_("Please enter a username."), _("Input Required"), wxOK | wxICON_WARNING, this);
        return;
    }

    gmmFilename = username + ".gmm";
    if (!wxFileExists(gmmFilename)) {
        wxLogMessage("GMM file not found: %s", gmmFilename.c_str());
        wxMessageBox(wxString::Format(_("No GMM file found for user '%s'."), username), 
                     _("Error"), wxOK | wxICON_ERROR, this);
        usernameInput->Clear();
        return;
    }

    try {
        storedGmm.load(gmmFilename.ToStdString());
        wxLogMessage("Loaded GMM file: %s", gmmFilename.c_str());
        EnterArea->SetFocus();
    } catch (const std::exception& e) {
        wxLogMessage("Failed to load GMM: %s", e.what());
        wxMessageBox(wxString::Format(_("Failed to load GMM for '%s'."), username), 
                     _("Error"), wxOK | wxICON_ERROR, this);
        usernameInput->Clear();
    }
}

void Login::OnKeyDown(wxKeyEvent& event) {
    KeyEvent ke;
    ke.key = event.GetKeyCode();
    ke.press_time = std::chrono::steady_clock::now();
    key_events.push_back(ke);
    event.Skip();
}

void Login::OnKeyUp(wxKeyEvent& event) {
    int key = event.GetKeyCode();
    for (auto& ke : key_events) {
        if (ke.key == key && ke.release_time == std::chrono::steady_clock::time_point()) {
            ke.release_time = std::chrono::steady_clock::now();
            break;
        }
    }
    event.Skip();
}

void Login::OnEnterPressed(wxCommandEvent& event) {
    if (gmmFilename.IsEmpty()) {
        LoadGMMFromUsername();
        if (gmmFilename.IsEmpty()) return;
    }

    wxString entered = EnterArea->GetValue().Trim().Trim(false);
    wxString expected = Text->GetLabel().Trim().Trim(false);

    wxLogMessage("Entered: '%s'", entered.c_str());
    wxLogMessage("Expected: '%s'", expected.c_str());

    if (entered != expected) {
        wxMessageBox(_("Text does not match. Please try again."), _("Incorrect Input"), wxOK | wxICON_WARNING, this);
        EnterArea->Clear();
        key_events.clear();
        return;
    }

    std::vector<Vec2> features;
    for (size_t i = 0; i < key_events.size() - 1; ++i) {
        if (key_events[i].release_time != std::chrono::steady_clock::time_point() &&
            key_events[i + 1].press_time != std::chrono::steady_clock::time_point()) {
            double dwell = std::chrono::duration<double, std::milli>(key_events[i].release_time - key_events[i].press_time).count();
            double flight = std::chrono::duration<double, std::milli>(key_events[i + 1].press_time - key_events[i].release_time).count();
            if (dwell >= 0 && flight >= -1000 && flight < 1000) {
                features.emplace_back(dwell, flight);
                wxLogMessage("Login Feature %zu: dwell=%f, flight=%f", features.size(), dwell, flight);
            }
        }
    }

    wxLogMessage("Login Total features collected: %zu", features.size());
    if (features.size() < MIN_SAMPLES) {
        wxLogMessage("Not enough features: %zu < %zu", features.size(), MIN_SAMPLES);
        if (currentTextIndex < textSamples.size() - 1) {
            UpdateText();
            EnterArea->Clear();
            key_events.clear();
        } else {
            wxMessageBox(_("Not enough valid keystrokes for authentication."), _("Error"), wxOK | wxICON_WARNING, this);
            currentTextIndex = 0;
            Text->SetLabel(textSamples[0]);
            EnterArea->Clear();
            key_events.clear();
        }
        return;
    }

    double max_dwell = 0, max_flight = 0;
    for (const auto& f : features) {
        max_dwell = std::max(max_dwell, std::abs(f.x));
        max_flight = std::max(max_flight, std::abs(f.y));
    }
    std::vector<Vec2> norm_features = features;
    for (auto& f : norm_features) {
        f.x /= max_dwell ? max_dwell : 1.0;
        f.y /= max_flight ? max_flight : 1.0;
    }

    double ll = storedGmm.logLikelihood(norm_features);
    wxLogMessage("Login Log likelihood: %f", ll);

    const double THRESHOLD = -50.0;
    if (ll > THRESHOLD) {
        wxString matchedUser = wxFileName(gmmFilename).GetName();
        wxLogMessage("Authentication successful for %s", matchedUser.c_str());
        wxMessageBox(wxString::Format(_("Login successful for %s! Redirecting to Welcome Page..."), matchedUser),
                     _("Success"), wxOK | wxICON_INFORMATION, this);
        // Pass keystroke data to WelcomePage
        WelcomePage* welcomePage = new WelcomePage(nullptr, wxID_ANY, _("Welcome"),
            features, ll); // Pass features and likelihood
        welcomePage->Show(true);
        this->Close(true);
    } else {
        wxLogMessage("Authentication failed, likelihood %f < %f", ll, THRESHOLD);
        wxMessageBox(_("Authentication failed. Keystroke pattern does not match."), 
                     _("Login Failed"), wxOK | wxICON_ERROR, this);
        if (currentTextIndex < textSamples.size() - 1) {
            UpdateText();
        } else {
            currentTextIndex = 0;
            Text->SetLabel(textSamples[0]);
        }
        EnterArea->Clear();
        key_events.clear();
    }
}

void Login::UpdateText() {
    currentTextIndex = (currentTextIndex + 1) % textSamples.size();
    Text->SetLabel(textSamples[currentTextIndex]);
    Text->Wrap(600);
    wxLogMessage("Updated text to: '%s' (index: %zu)", Text->GetLabel().c_str(), currentTextIndex);
}