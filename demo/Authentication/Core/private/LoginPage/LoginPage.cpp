#include <wx/filefn.h>
#include <wx/log.h>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include "LoginPage/LoginPage.hpp"
#include "WelcomePage/WelcomePage.hpp"
#include "HomePage/HomePage.hpp"

wxBEGIN_EVENT_TABLE(Login, wxFrame)
    EVT_BUTTON(wxID_ANY, Login::OnBackClicked)
    EVT_KEY_DOWN(Login::OnKeyDown)
    EVT_KEY_UP(Login::OnKeyUp)
    EVT_TEXT_ENTER(wxID_ANY, Login::OnEnterPressed)
wxEND_EVENT_TABLE()

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
                                  wxDefaultPosition, wxSize(600, -1), 0);
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
                           wxDefaultPosition, wxSize(600, -1), 0);
    Text->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                        wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Text->Wrap(600);
    Context->Add(Text, 0, wxALL | wxEXPAND, 5);

    EnterArea = new wxTextCtrl(this, wxID_ANY, wxEmptyString, 
                              wxDefaultPosition, wxSize(600, -1), wxTE_PROCESS_ENTER);
    EnterArea->Enable(false); // Disable by default
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

    // Load users.csv
    if (!LoadUserMap("./resources/users.csv")) {
        wxLogError("Failed to load users.csv");
        wxMessageBox(_("Failed to load user database."), _("Error"), wxOK | wxICON_ERROR, this);
    }
}

Login::~Login() {}

bool Login::LoadUserMap(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        wxLogError("Cannot open %s", filename.c_str());
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header
    wxLogMessage("CSV header: %s", line.c_str());
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        wxLogMessage("Parsing CSV line: %s", line.c_str());
        std::stringstream ss(line);
        std::string participant_id_str, name, sentences;

        // Parse PARTICIPANT_ID
        if (!std::getline(ss, participant_id_str, ',')) {
            wxLogWarning("Invalid CSV line (missing PARTICIPANT_ID): %s", line.c_str());
            continue;
        }
        // Parse NAME
        if (!std::getline(ss, name, ',')) {
            wxLogWarning("Invalid CSV line (missing NAME): %s", line.c_str());
            continue;
        }
        // Parse SENTENCES
        std::getline(ss, sentences); // Read rest of line
        // Trim whitespace and handle quotes
        name.erase(0, name.find_first_not_of(" \t\r\n\""));
        name.erase(name.find_last_not_of(" \t\r\n\"") + 1);
        sentences.erase(0, sentences.find_first_not_of(" \t\r\n\""));
        sentences.erase(sentences.find_last_not_of(" \t\r\n\"") + 1);
        // Clean sentences: remove internal newlines and control characters
        sentences.erase(std::remove_if(sentences.begin(), sentences.end(), 
                                       [](char c) { return c == '\n' || c == '\r' || (c < 32 && c != '\t'); }), 
                       sentences.end());
        if (name.empty()) {
            wxLogWarning("Empty username in CSV line: %s", line.c_str());
            continue;
        }
        // Normalize to lowercase for userMap
        std::string normalizedName = name;
        std::transform(normalizedName.begin(), normalizedName.end(), normalizedName.begin(), ::tolower);
        try {
            unsigned long long participant_id = std::stoull(participant_id_str);
            userMap[normalizedName] = {participant_id, sentences};
            wxLogMessage("Loaded user: %s (normalized: %s), PARTICIPANT_ID: %llu, SENTENCES: %s", 
                         name.c_str(), normalizedName.c_str(), participant_id, sentences.c_str());
        } catch (const std::exception& e) {
            wxLogError("Invalid PARTICIPANT_ID in %s: %s (%s)", filename.c_str(), line.c_str(), e.what());
        }
    }
    file.close();
    wxLogMessage("Loaded %zu users into userMap", userMap.size());
    return true;
}

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

void Login::LoadGMMFromUsername() {
    wxString username = usernameInput->GetValue().Trim();
    wxString usernameLower = username.Lower();
    if (username.IsEmpty()) {
        wxMessageBox(_("Please enter a username."), _("Input Required"), wxOK | wxICON_WARNING, this);
        EnterArea->Enable(false);
        EnterArea->Clear();
        return;
    }

    std::string usernameLowerStr = usernameLower.ToStdString();
    auto it = userMap.find(usernameLowerStr);
    if (it == userMap.end()) {
        wxLogMessage("Username not found: %s", usernameLowerStr.c_str());
        wxMessageBox(wxString::Format(_("Username '%s' not found in user database."), username), 
                     _("Error"), wxOK | wxICON_ERROR, this);
        usernameInput->Clear();
        EnterArea->Enable(false);
        EnterArea->Clear();
        return;
    }

    // Load sentences from userMap
    textSamples.clear();
    std::string sentences = it->second.second;
    if (sentences.empty()) {
        wxLogMessage("No sentences found for user: %s", usernameLowerStr.c_str());
        wxMessageBox(wxString::Format(_("No sentences found for user '%s'."), username), 
                     _("Error"), wxOK | wxICON_ERROR, this);
        usernameInput->Clear();
        EnterArea->Enable(false);
        EnterArea->Clear();
        return;
    }

    std::stringstream ss(sentences);
    std::string sentence;
    while (std::getline(ss, sentence, '|')) {
        if (!sentence.empty()) {
            // Clean individual sentence
            sentence.erase(std::remove_if(sentence.begin(), sentence.end(), 
                                         [](char c) { return c == '\n' || c == '\r' || (c < 32 && c != '\t'); }), 
                          sentence.end());
            textSamples.push_back(wxString(sentence));
            wxLogMessage("Loaded sentence for %s: %s (length: %zu)", 
                         usernameLowerStr.c_str(), sentence.c_str(), sentence.length());
        }
    }
    if (textSamples.empty()) {
        wxLogMessage("No valid sentences parsed for user: %s", usernameLowerStr.c_str());
        wxMessageBox(wxString::Format(_("No valid sentences found for user '%s'."), username), 
                     _("Error"), wxOK | wxICON_ERROR, this);
        usernameInput->Clear();
        EnterArea->Enable(false);
        EnterArea->Clear();
        return;
    }

    currentTextIndex = 0;
    Text->SetLabel(textSamples[0]);
    Text->Wrap(600);
    this->Layout();
    wxLogMessage("Set initial text to: '%s' for user %s (length: %zu)", 
                 textSamples[0].c_str(), usernameLowerStr.c_str(), textSamples[0].length());

    gmmFilename = "./resources/" + usernameLower + ".gmm";
    if (!wxFileExists(gmmFilename)) {
        wxLogMessage("GMM file not found: %s", gmmFilename.c_str());
        wxMessageBox(wxString::Format(_("No GMM file found for user '%s'."), username), 
                     _("Error"), wxOK | wxICON_ERROR, this);
        usernameInput->Clear();
        EnterArea->Enable(false);
        EnterArea->Clear();
        return;
    }

    try {
        storedGmm.load(gmmFilename.ToStdString());
        wxLogMessage("Loaded GMM file: %s", gmmFilename.c_str());
        EnterArea->Enable(true);
        EnterArea->SetFocus();
    } catch (const std::exception& e) {
        wxLogMessage("Failed to load GMM: %s", e.what());
        wxMessageBox(wxString::Format(_("Failed to load GMM for '%s'."), username), 
                     _("Error"), wxOK | wxICON_ERROR, this);
        usernameInput->Clear();
        EnterArea->Enable(false);
        EnterArea->Clear();
    }
}

void Login::OnUsernameEnter(wxCommandEvent& event) {
    LoadGMMFromUsername();
}

void Login::OnKeyDown(wxKeyEvent& event) {
    int key = event.GetKeyCode();
    if (key == 306 || key == 307) { // Ignore Shift keys
        wxLogMessage("Ignoring Shift key down: %d", key);
        event.Skip();
        return;
    }
    KeyEvent ke;
    ke.key = key;
    ke.press_time = std::chrono::steady_clock::now();
    key_events.push_back(ke);
    wxLogMessage("Key down: %d (char: %c)", ke.key, (ke.key >= 0 && ke.key < 128 && std::isprint(ke.key)) ? static_cast<char>(ke.key) : ' ');
    event.Skip();
}

void Login::OnKeyUp(wxKeyEvent& event) {
    int key = event.GetKeyCode();
    if (key == 306 || key == 307) {
        wxLogMessage("Ignoring Shift key up: %d", key);
        event.Skip();
        return;
    }
    for (auto& ke : key_events) {
        if (ke.key == key && ke.release_time == std::chrono::steady_clock::time_point()) {
            ke.release_time = std::chrono::steady_clock::now();
            wxLogMessage("Key up: %d (char: %c)", key, (key >= 0 && ke.key < 128 && std::isprint(key)) ? static_cast<char>(key) : ' ');
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

    wxString username = usernameInput->GetValue().Trim();
    wxString usernameLower = username.Lower();

    // Use the original sentence from textSamples for comparison
    wxString expected = textSamples[currentTextIndex];
    wxString entered = EnterArea->GetValue();

    // Normalize strings: remove newlines, control characters, and trim
    std::string enteredStr = entered.ToStdString();
    std::string expectedStr = expected.ToStdString();
    enteredStr.erase(std::remove_if(enteredStr.begin(), enteredStr.end(), 
                                    [](char c) { return c == '\n' || c == '\r' || (c < 32 && c != '\t'); }), 
                     enteredStr.end());
    expectedStr.erase(std::remove_if(expectedStr.begin(), expectedStr.end(), 
                                     [](char c) { return c == '\n' || c == '\r' || (c < 32 && c != '\t'); }), 
                      expectedStr.end());
    entered = wxString(enteredStr).Trim().Trim(false);
    expected = wxString(expectedStr).Trim().Trim(false);

    wxLogMessage("Raw Entered: '%s' (length: %zu)", EnterArea->GetValue().c_str(), EnterArea->GetValue().length());
    wxLogMessage("Raw Expected (from textSamples): '%s' (length: %zu)", textSamples[currentTextIndex].c_str(), textSamples[currentTextIndex].length());
    wxLogMessage("Raw Displayed (from Text): '%s' (length: %zu)", Text->GetLabel().c_str(), Text->GetLabel().length());
    wxLogMessage("Normalized Entered: '%s' (length: %zu)", entered.c_str(), entered.length());
    wxLogMessage("Normalized Expected: '%s' (length: %zu)", expected.c_str(), expected.length());
    wxLogMessage("Raw key events collected: %zu", key_events.size());

    if (entered != expected) {
        wxMessageBox(_("Text does not match. Please try again."), _("Incorrect Input"), wxOK | wxICON_WARNING, this);
        EnterArea->Clear();
        key_events.clear();
        return;
    }

    std::vector<KeyEvent> filteredEvents;
    std::string expectedCleanStr = expected.ToStdString();
    size_t expectedPos = 0;

    wxLogMessage("Filtering key events...");
    for (size_t i = 0; i < key_events.size() && expectedPos < expectedCleanStr.length(); ++i) {
        int key = key_events[i].key;
        char keyChar = (key >= 0 && key < 128 && std::isprint(key)) ? static_cast<char>(key) : ' ';
        if (key == WXK_BACK) {
            if (!filteredEvents.empty()) {
                filteredEvents.pop_back();
                if (expectedPos > 0) expectedPos--;
            }
            wxLogMessage("Backspace detected at index %zu, removed last event. Expected pos: %zu", i, expectedPos);
        } else {
            char expectedChar = std::tolower(expectedCleanStr[expectedPos]);
            if ((std::tolower(keyChar) == expectedChar) || (keyChar == ' ' && expectedChar == ' ')) {
                filteredEvents.push_back(key_events[i]);
                expectedPos++;
                wxLogMessage("Matched key '%c' (code %d) at pos %zu", keyChar, key, expectedPos - 1);
            } else {
                wxLogMessage("Skipped key '%c' (code %d) at pos %zu, expected '%c'", keyChar, key, expectedPos, expectedChar);
            }
        }
    }

    wxLogMessage("Filtered events: %zu, Expected length: %zu", filteredEvents.size(), expectedCleanStr.length());
    if (filteredEvents.size() != expectedCleanStr.length()) {
        wxMessageBox(wxString::Format(_("Keystroke data does not match expected phrase length. Filtered: %zu, Expected: %zu"),
                                      filteredEvents.size(), expectedCleanStr.length()),
                     _("Error"), wxOK | wxICON_WARNING, this);
        EnterArea->Clear();
        key_events.clear();
        return;
    }

    wxMilliSleep(100);
    wxYield();

    for (size_t i = 0; i < filteredEvents.size(); ++i) {
        if (filteredEvents[i].release_time == std::chrono::steady_clock::time_point()) {
            if (i + 1 < filteredEvents.size() && filteredEvents[i + 1].press_time != std::chrono::steady_clock::time_point()) {
                filteredEvents[i].release_time = filteredEvents[i + 1].press_time;
            } else {
                filteredEvents[i].release_time = std::chrono::steady_clock::now();
            }
            wxLogMessage("Assigned release_time for key %d (char: %c) at index %zu", 
                         filteredEvents[i].key, 
                         (filteredEvents[i].key >= 0 && filteredEvents[i].key < 128 && std::isprint(filteredEvents[i].key)) ? static_cast<char>(filteredEvents[i].key) : ' ', 
                         i);
        }
    }

    std::vector<Vec2> features;
    for (size_t i = 0; i < filteredEvents.size() - 1; ++i) {
        if (filteredEvents[i].release_time != std::chrono::steady_clock::time_point() &&
            filteredEvents[i + 1].press_time != std::chrono::steady_clock::time_point()) {
            double dwell = std::chrono::duration<double, std::milli>(filteredEvents[i].release_time - filteredEvents[i].press_time).count();
            double flight = std::chrono::duration<double, std::milli>(filteredEvents[i + 1].press_time - filteredEvents[i].release_time).count();
            if (dwell >= 0 && flight >= -1000 && flight < 10000) {
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
            Text->Wrap(600);
            this->Layout();
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
        wxMessageBox(wxString::Format(_("Login successful for %s! Redirecting to Welcome Page..."), username),
                     _("Success"), wxOK | wxICON_INFORMATION, this);
        WelcomePage* welcomePage = new WelcomePage(nullptr, wxID_ANY, _("Welcome"), features, ll);
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
            Text->Wrap(600);
            this->Layout();
        }
        EnterArea->Clear();
        key_events.clear();
    }
}

void Login::UpdateText() {
    currentTextIndex = (currentTextIndex + 1) % textSamples.size();
    Text->SetLabel(textSamples[currentTextIndex]);
    Text->Wrap(600);
    this->Layout();
    wxLogMessage("Updated text to: '%s' (index: %zu, length: %zu)", 
                 textSamples[currentTextIndex].c_str(), currentTextIndex, textSamples[currentTextIndex].length());
}