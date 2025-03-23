#include "SignupPage/SignupPage.hpp"
#include "HomePage/HomePage.hpp"
BEGIN_EVENT_TABLE(SignUp, wxFrame)
    EVT_KEY_DOWN(SignUp::OnKeyDown)
    EVT_KEY_UP(SignUp::OnKeyUp)
    EVT_TEXT_ENTER(wxID_ANY, SignUp::OnEnterPressed)
END_EVENT_TABLE()

SignUp::SignUp(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style), currentTextIndex(0) {
    this->SetSizeHints(wxSize(1200, 800), wxSize(1200, 800));
    this->SetPosition(wxPoint(1200-600, 800-400));
    this->SetBackgroundColour(wxColour(255, 240, 255));

    wxBoxSizer* Area = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* Context = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* Name = new wxBoxSizer(wxHORIZONTAL);
    NameLabel = new wxStaticText(this, wxID_ANY, _("What is your name? :"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    NameLabel->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Name->Add(NameLabel, 0, wxALL|wxEXPAND, 5);
    m_textCtrl2 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    Name->Add(m_textCtrl2, 1, wxALL|wxEXPAND, 5);
    Context->Add(Name, 0, wxALL|wxEXPAND, 5);

    HelpingText = new wxStaticText(this, wxID_ANY, _("Type the text below and press Enter:"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    HelpingText->SetFont(wxFont(22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Context->Add(HelpingText, 0, wxALL, 5);
    Context->Add(1, 30, 0, wxEXPAND, 5);

    textSamples = {
        _("The quick brown fox jumps over the lazy dog"),
        _("Pack my box with five dozen liquor jugs"),
        _("How razorback jumping frogs can level six piqued gymnasts"),
        _("Crazy Fredrick bought many very exquisite opal jewels")
    };
    Text = new wxStaticText(this, wxID_ANY, textSamples[0], wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    Text->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Context->Add(Text, 0, wxALL, 5);

    EnterArea = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    Context->Add(EnterArea, 1, wxALL|wxEXPAND, 5);

    Area->Add(Context, 1, wxALIGN_CENTER|wxALL, 100);
    this->SetSizer(Area);
    this->Layout();
    this->Centre(wxBOTH);

    m_textCtrl2->Bind(wxEVT_SET_FOCUS, &SignUp::OnTextCtrlFocus, this);
    m_textCtrl2->Bind(wxEVT_TEXT_ENTER, &SignUp::OnTextCtrlEnter, this);
    EnterArea->Bind(wxEVT_KEY_DOWN, &SignUp::OnKeyDown, this);
    EnterArea->Bind(wxEVT_KEY_UP, &SignUp::OnKeyUp, this);
    EnterArea->Bind(wxEVT_TEXT_ENTER, &SignUp::OnEnterPressed, this);
}

SignUp::~SignUp() {}

void SignUp::OnTextCtrlFocus(wxFocusEvent& event) {
    EnterArea->Enable(false);
    event.Skip();
}

void SignUp::OnTextCtrlEnter(wxCommandEvent& event) {
    if (!m_textCtrl2->GetValue().IsEmpty()) {
        EnterArea->Enable(true);
        EnterArea->SetFocus();
    } else {
        wxMessageBox(_("Please enter a name before proceeding."), _("Input Required"), wxOK | wxICON_WARNING, this);
    }
}

void SignUp::OnKeyDown(wxKeyEvent& event) {
    KeyEvent ke;
    ke.key = event.GetKeyCode();
    ke.press_time = std::chrono::steady_clock::now();
    key_events.push_back(ke);
    event.Skip();
}

void SignUp::OnKeyUp(wxKeyEvent& event) {
    char key = event.GetKeyCode();
    for (auto& ke : key_events) {
        if (ke.key == key && ke.release_time == std::chrono::steady_clock::time_point()) {
            ke.release_time = std::chrono::steady_clock::now();
            break;
        }
    }
    event.Skip();
}

void SignUp::OnEnterPressed(wxCommandEvent& event) {
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
                if (dwell >= 0 && flight >= -1000 && flight < 1000) {
                    features.emplace_back(dwell, flight);
                    wxLogMessage("Feature %zu: dwell=%f, flight=%f", features.size(), dwell, flight);
                }
            }
        }

        wxLogMessage("Total features collected: %zu", features.size());
        if (features.size() < 2) {
            wxMessageBox(_("Not enough valid keystroke data to train GMM."), _("Error"), wxOK | wxICON_ERROR, this);
            key_events.clear();
            EnterArea->Clear();
            currentTextIndex = 0;
            Text->SetLabel(textSamples[0]);
            return;
        }

        GMM gmm(2);
        try {
            // Normalize features to reduce scale issues
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

            gmm.train(norm_features); // Train on normalized data
            double ll = gmm.logLikelihood(norm_features);
            wxLogMessage("Log likelihood after training: %f", ll);
            if (std::isnan(static_cast<double>(ll))) {
                wxMessageBox(_("GMM training produced NaN values."), _("Error"), wxOK | wxICON_ERROR, this);
                return;
            }

            wxString username = m_textCtrl2->GetValue();
            std::string filename = (username + ".gmm").ToStdString();
            gmm.save(filename);

            wxMessageBox(wxString::Format(_("User %s signed up successfully with %zu keystrokes! Redirecting to Home Page..."), username, key_events.size()),
                         _("Success"), wxOK | wxICON_INFORMATION, this);
            HomePage* homeWindow = new HomePage(nullptr, wxID_ANY, "Home");
            homeWindow->Show(true);
            this->Close(true);
        } catch (const std::exception& e) {
            wxMessageBox(wxString::Format(_("GMM training failed: %s"), e.what()), _("Error"), wxOK | wxICON_ERROR, this);
        }
    } else {
        UpdateText();
        EnterArea->Clear();
    }
}
void SignUp::UpdateText() {
    currentTextIndex = (currentTextIndex + 1) % textSamples.size();
    Text->SetLabel(textSamples[currentTextIndex]);
}