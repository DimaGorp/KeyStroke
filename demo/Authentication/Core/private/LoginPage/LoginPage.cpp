// Core/private/LoginPage/LoginPage.cpp
#include "LoginPage/LoginPage.hpp"
#include "WelcomePage/WelcomePage.hpp"
#include "HomePage/HomePage.hpp"
BEGIN_EVENT_TABLE(Login, wxFrame)
    EVT_BUTTON(wxID_ANY, Login::OnBackClicked)
    EVT_KEY_DOWN(Login::OnKeyDown)
    EVT_KEY_UP(Login::OnKeyUp)
    EVT_TEXT_ENTER(wxID_ANY, Login::OnEnterPressed)
END_EVENT_TABLE()

Login::Login(wxWindow* parent, wxWindowID id, const wxString& title, 
             const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style), currentTextIndex(0), storedGmm(2) {
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

    HelpingText = new wxStaticText(this, wxID_ANY, _("Type the text below and press Enter:"), 
                                  wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    HelpingText->Wrap(300);
    HelpingText->SetFont(wxFont(22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                               wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Context->Add(HelpingText, 0, wxALL, 5);

    Context->Add(1, 20, 0, wxEXPAND, 5);

    textSamples = {
        _("The quick brown fox jumps over the lazy dog"),
        _("Pack my box with five dozen liquor jugs"),
        _("How razorback jumping frogs can level six piqued gymnasts"),
        _("Crazy Fredrick bought many very exquisite opal jewels")
    };
    Text = new wxStaticText(this, wxID_ANY, textSamples[0], 
                           wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    Text->Wrap(300);
    Text->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                        wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
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

    // Prompt for GMM file when LoginPage opens
    wxFileDialog openFileDialog(this, _("Select your GMM file"), "", "",
                                "GMM files (*.gmm)|*.gmm", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        this->Close(true); // Close if user cancels GMM selection
        return;
    }

    gmmFilename = openFileDialog.GetPath();
    try {
        storedGmm.load(gmmFilename.ToStdString());
    } catch (const std::exception& e) {
        wxMessageBox(_("Failed to load GMM file. Closing login."), _("Error"), wxOK | wxICON_ERROR, this);
        this->Close(true);
    }
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
    HomePage* homeWindow = new HomePage(nullptr, wxID_ANY, _("Home"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
    homeWindow->Show(true);
    this->Close(true); // Stops authentication
}

void Login::OnKeyDown(wxKeyEvent& event) {
    KeyEvent ke;
    ke.key = event.GetKeyCode();
    ke.press_time = std::chrono::steady_clock::now();
    key_events.push_back(ke);
    event.Skip();
}

void Login::OnKeyUp(wxKeyEvent& event) {
    char key = event.GetKeyCode();
    for (auto& ke : key_events) {
        if (ke.key == key && ke.release_time == std::chrono::steady_clock::time_point()) {
            ke.release_time = std::chrono::steady_clock::now();
            break;
        }
    }
    event.Skip();
}

void Login::OnEnterPressed(wxCommandEvent& event) {
    wxString entered = EnterArea->GetValue();
    if (entered != Text->GetLabel()) {
        wxMessageBox(_("Text does not match. Please try again."), _("Incorrect Input"), wxOK | wxICON_WARNING, this);
        return;
    }

    if (key_events.size() >= MIN_SAMPLES) {
        std::vector<Vec2> features;
        for (size_t i = 0; i < key_events.size() - 1; ++i) {
            if (key_events[i].release_time != std::chrono::steady_clock::time_point() &&
                key_events[i + 1].press_time != std::chrono::steady_clock::time_point()) {
                double dwell = std::chrono::duration<double, std::milli>(key_events[i].release_time - key_events[i].press_time).count();
                double flight = std::chrono::duration<double, std::milli>(key_events[i + 1].press_time - key_events[i].release_time).count();
                features.emplace_back(dwell, flight);
                wxLogMessage("Login Feature %zu: dwell=%f, flight=%f", features.size(), dwell, flight);
            }
        }

        wxLogMessage("Login Total features collected: %zu", features.size());
        if (features.size() < 2) {
            wxMessageBox(_("Not enough valid keystrokes for authentication."), _("Error"), wxOK | wxICON_WARNING, this);
            UpdateText();
            EnterArea->Clear();
            return;
        }

        // Normalize features to match SignUp
        double max_dwell = 0, max_flight = 0;
        for (const auto& f : features) {
            max_dwell = std::max(max_dwell, std::abs(f.x));
            max_flight = std::max(max_flight, std::abs(f.y));
        }
        std::vector<Vec2> norm_features = features;
        for (auto& f : norm_features) {
            f.x /= max_dwell ? max_dwell : 1.0;  // Avoid div by 0
            f.y /= max_flight ? max_flight : 1.0;
        }

        double ll = storedGmm.logLikelihood(norm_features);
        wxLogMessage("Login Log likelihood: %f", ll);
        const double THRESHOLD = -50.0; // May need adjustment
        if (ll > THRESHOLD) {
            wxString matchedUser = wxFileName(gmmFilename).GetName();
            wxMessageBox(wxString::Format(_("Login successful for %s! Redirecting to Home Page..."), matchedUser),
                         _("Success"), wxOK | wxICON_INFORMATION, this);
            WelcomePage* welcomepage = new WelcomePage(nullptr, wxID_ANY, _("Welcome"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
            welcomepage->Show(true);
            this->Close(true);
        } else {
            wxMessageBox(_("Authentication failed. Keystroke pattern does not match."), _("Login Failed"), wxOK | wxICON_ERROR, this);
            key_events.clear();
            EnterArea->Clear();
            currentTextIndex = 0;
            Text->SetLabel(textSamples[0]);
        }
    } else {
        UpdateText();
        EnterArea->Clear();
    }
}

void Login::UpdateText() {
    currentTextIndex = (currentTextIndex + 1) % textSamples.size();
    Text->SetLabel(textSamples[currentTextIndex]);
}