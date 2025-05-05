
#include <wx/filefn.h>
#include <wx/log.h>
#include <fstream>
#include <sstream>
#include <cctype>
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
    : wxFrame(parent, id, title, pos, size, style), currentTextIndex(0), testSectionId(0), storedGmm(2) {
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

    // Load users.csv
    if (!LoadUserMap("users.csv")) {
        wxLogError("Failed to load users.csv");
        wxMessageBox(_("Failed to load user database."), _("Error"), wxOK | wxICON_ERROR, this);
    }
}

Login::~Login() {}

bool Login::LoadUserMap(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        wxLogError("Cannot open %s", filename.c_str());
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string participant_id_str, name;
        if (std::getline(ss, participant_id_str, ',') && std::getline(ss, name)) {
            try {
                size_t participant_id = std::stoul(participant_id_str);
                userMap[name] = participant_id;
                wxLogMessage("Loaded user: %s, PARTICIPANT_ID: %zu", name.c_str(), participant_id);
            } catch (const std::exception& e) {
                wxLogError("Invalid PARTICIPANT_ID in %s: %s", filename.c_str(), line.c_str());
            }
        }
    }
    file.close();
    return true;
}

std::string Login::EscapeCSVField(const std::string& field) const {
    if (field.find(',') == std::string::npos && field.find('"') == std::string::npos) {
        return field;
    }
    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') escaped += '"';
        escaped += c;
    }
    escaped += "\"";
    return escaped;
}

void Login::SaveKeystrokesToCSV(const std::string& filename, const std::vector<KeyEvent>& events, 
                                size_t participantId, size_t testSectionId, bool firstWrite) {
    std::ofstream csvFile(filename, firstWrite ? std::ios::out : std::ios::app);
    if (!csvFile.is_open()) {
        wxLogError("Failed to open CSV file: %s", filename.c_str());
        return;
    }

    if (firstWrite) {
        csvFile << "PARTICIPANT_ID,TEST_SECTION_ID,SENTENCE,USER_INPUT,KEYSTROKE_ID,PRESS_TIME,RELEASE_TIME,LETTER,KEYCODE\n";
        wxLogMessage("Wrote CSV header to %s", filename.c_str());
    }

    std::string sentence = Text->GetLabel().ToStdString();
    std::string user_input = EnterArea->GetValue().ToStdString();

    for (size_t i = 0; i < events.size(); ++i) {
        const auto& event = events[i];
        size_t keystroke_id = i + 1;
        auto press_ms = std::chrono::duration_cast<std::chrono::milliseconds>(event.press_time.time_since_epoch()).count();
        auto release_ms = std::chrono::duration_cast<std::chrono::milliseconds>(event.release_time.time_since_epoch()).count();

        std::string letter;
        if (event.key == WXK_SPACE) {
            letter = " ";
        } else if (event.key == WXK_BACK) {
            letter = "BKSP";
        } else if (event.key >= 0 && event.key < 128 && std::isprint(event.key)) {
            letter = std::string(1, static_cast<char>(event.key));
        } else {
            letter = "UNKNOWN";
        }

        int keycode = event.key;

        csvFile << participantId << ","
                << testSectionId << ","
                << EscapeCSVField(sentence) << ","
                << EscapeCSVField(user_input) << ","
                << keystroke_id << ","
                << press_ms << ","
                << release_ms << ","
                << EscapeCSVField(letter) << ","
                << keycode << "\n";

        wxLogMessage("Wrote keystroke %zu: participant=%zu, test_section=%zu, keystroke_id=%zu, letter=%s, keycode=%d",
                     i + 1, participantId, testSectionId, keystroke_id, letter.c_str(), keycode);
    }

    csvFile.flush();
    csvFile.close();
    wxLogMessage("Saved %zu keystrokes to %s (firstWrite: %d)", events.size(), filename.c_str(), firstWrite);
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

void Login::OnUsernameEnter(wxCommandEvent& event) {
    LoadGMMFromUsername();
}

void Login::LoadGMMFromUsername() {
    wxString username = usernameInput->GetValue().Trim();
    if (username.IsEmpty()) {
        wxMessageBox(_("Please enter a username."), _("Input Required"), wxOK | wxICON_WARNING, this);
        return;
    }

    std::string usernameStr = username.ToStdString();
    auto it = userMap.find(usernameStr);
    if (it == userMap.end()) {
        wxLogMessage("Username not found: %s", usernameStr.c_str());
        wxMessageBox(wxString::Format(_("Username '%s' not found in user database."), username), 
                     _("Error"), wxOK | wxICON_ERROR, this);
        usernameInput->Clear();
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
            wxLogMessage("Key up: %d (char: %c)", key, (key >= 0 && key < 128 && std::isprint(key)) ? static_cast<char>(key) : ' ');
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
    std::string usernameStr = username.ToStdString();
    auto it = userMap.find(usernameStr);
    if (it == userMap.end()) {
        wxMessageBox(wxString::Format(_("Username '%s' not found in user database."), username), 
                     _("Error"), wxOK | wxICON_ERROR, this);
        return;
    }
    size_t participantId = it->second;

    wxString entered = EnterArea->GetValue().Trim().Trim(false);
    wxString expected = Text->GetLabel().Trim().Trim(false);

    wxLogMessage("Entered: '%s' (length: %zu)", entered.c_str(), entered.length());
    wxLogMessage("Expected: '%s' (length: %zu)", expected.c_str(), expected.length());
    wxLogMessage("Raw key events collected: %zu", key_events.size());

    if (entered != expected) {
        wxMessageBox(_("Text does not match. Please try again."), _("Incorrect Input"), wxOK | wxICON_WARNING, this);
        EnterArea->Clear();
        key_events.clear();
        return;
    }

    std::vector<KeyEvent> filteredEvents;
    std::string expectedStr = expected.ToStdString();
    size_t expectedPos = 0;

    wxLogMessage("Filtering key events...");
    for (size_t i = 0; i < key_events.size() && expectedPos < expectedStr.length(); ++i) {
        int key = key_events[i].key;
        char keyChar = (key >= 0 && key < 128 && std::isprint(key)) ? static_cast<char>(key) : ' ';
        if (key == WXK_BACK) {
            if (!filteredEvents.empty()) {
                filteredEvents.pop_back();
                if (expectedPos > 0) expectedPos--;
            }
            wxLogMessage("Backspace detected at index %zu, removed last event. Expected pos: %zu", i, expectedPos);
        } else {
            char expectedChar = std::tolower(expectedStr[expectedPos]);
            if ((std::tolower(keyChar) == expectedChar) || (keyChar == ' ' && expectedChar == ' ')) {
                filteredEvents.push_back(key_events[i]);
                expectedPos++;
                wxLogMessage("Matched key '%c' (code %d) at pos %zu", keyChar, key, expectedPos - 1);
            } else {
                wxLogMessage("Skipped key '%c' (code %d) at pos %zu, expected '%c'", keyChar, key, expectedPos, expectedChar);
            }
        }
    }

    wxLogMessage("Filtered events: %zu, Expected length: %zu", filteredEvents.size(), expectedStr.length());
    if (filteredEvents.size() != expectedStr.length()) {
        wxMessageBox(wxString::Format(_("Keystroke data does not match expected phrase length. Filtered: %zu, Expected: %zu"),
                                      filteredEvents.size(), expectedStr.length()),
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

    // Save keystrokes to CSV
    wxString csvFilename = username + "_keystrokes.csv";
    testSectionId++;
    SaveKeystrokesToCSV(csvFilename.ToStdString(), filteredEvents, participantId, testSectionId, testSectionId == 1);

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
            testSectionId = 0;
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
            testSectionId = 0;
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