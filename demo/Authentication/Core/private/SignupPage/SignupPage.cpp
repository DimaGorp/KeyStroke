#include <wx/log.h>
#include <sstream>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <functional>
#include <algorithm>
#include "SignupPage/SignupPage.hpp"
#include "Config.h"
#include "HomePage/HomePage.hpp"

BEGIN_EVENT_TABLE(SignUp, wxFrame)
    EVT_KEY_DOWN(SignUp::OnKeyDown)
    EVT_KEY_UP(SignUp::OnKeyUp)
    EVT_TEXT_ENTER(wxID_ANY, SignUp::OnEnterPressed)
END_EVENT_TABLE()

SignUp::SignUp(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style), currentTextIndex(0) {
    wxLog::SetActiveTarget(new wxLogStderr());
    wxLogMessage("SignUp window initialized");

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
    Text = new wxStaticText(this, wxID_ANY, textSamples[0], wxDefaultPosition, wxSize(600, -1), wxST_NO_AUTORESIZE | wxALIGN_LEFT);
    Text->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Text->Wrap(600);
    Context->Add(Text, 0, wxALL | wxEXPAND, 5);
    wxLogMessage("Initial text set to: '%s'", Text->GetLabel().c_str());

    EnterArea = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(600, -1), wxTE_PROCESS_ENTER);
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
    wxString username = m_textCtrl2->GetValue().Trim();
    wxString usernameLower = username.Lower();
    if (username.IsEmpty()) {
        wxMessageBox(_("Please enter a name before proceeding."), _("Input Required"), wxOK | wxICON_WARNING, this);
        return;
    }

    namespace fs = std::filesystem;
    fs::path gmmPath = fs::u8path("./resources/" + usernameLower.ToStdString() + ".gmm");
    fs::path csvPath = fs::u8path("./resources/users.csv");
    wxLogMessage("Checking for existing user: GMM at %s, CSV at %s", gmmPath.u8string().c_str(), csvPath.u8string().c_str());

    bool userInCSV = false;
    bool gmmExists = fs::exists(gmmPath);

    if (fs::exists(csvPath)) {
        std::ifstream userFile(csvPath, std::ios::binary);
        if (!userFile.is_open()) {
            wxLogError("Cannot open users.csv for reading: %s", csvPath.u8string().c_str());
            wxMessageBox(_("Failed to read users.csv."), _("Error"), wxOK | wxICON_ERROR, this);
            return;
        }
        std::string line;
        std::getline(userFile, line);
        wxLogMessage("CSV header: %s", line.c_str());
        while (std::getline(userFile, line)) {
            if (line.empty()) continue;
            wxLogMessage("Checking CSV line: %s", line.c_str());
            std::stringstream ss(line);
            std::string pid, name, sentences;
            if (std::getline(ss, pid, ',') && std::getline(ss, name, ',')) {
                name.erase(0, name.find_first_not_of(" \t\r\n"));
                name.erase(name.find_last_not_of(" \t\r\n") + 1);
                std::string normalizedName = name;
                std::transform(normalizedName.begin(), normalizedName.end(), normalizedName.begin(), ::tolower);
                wxLogMessage("Comparing normalized CSV name '%s' with input '%s'", normalizedName.c_str(), usernameLower.ToStdString().c_str());
                if (normalizedName == usernameLower.ToStdString()) {
                    userInCSV = true;
                    wxLogMessage("Found user '%s' in users.csv", name.c_str());
                    break;
                }
            }
        }
        userFile.close();
    }

    wxLogMessage("Check result: gmmExists=%d, userInCSV=%d for username '%s'", gmmExists, userInCSV, usernameLower.c_str());

    if (gmmExists) {
        if (!userInCSV) {
            bool firstWrite = !fs::exists(csvPath);
            std::ofstream csvFile(csvPath, std::ios::app | std::ios::binary);
            if (!csvFile.is_open()) {
                wxLogError("Failed to open users.csv for appending: %s", csvPath.u8string().c_str());
                wxMessageBox(_("Failed to update users.csv."), _("Error"), wxOK | wxICON_ERROR, this);
                return;
            }

            if (firstWrite) {
                csvFile << "PARTICIPANT_ID,NAME,SENTENCES\n";
                wxLogMessage("Wrote users.csv header to %s", csvPath.u8string().c_str());
            }

            std::string sentences;
            for (size_t i = 0; i < textSamples.size(); ++i) {
                sentences += textSamples[i].ToStdString();
                if (i < textSamples.size() - 1) sentences += "|";
            }

            std::hash<std::string> hasher;
            size_t participant_id = hasher(usernameLower.ToStdString());
            csvFile << "\n"<<participant_id << "," << EscapeCSVField(username.ToStdString()) << "," << '"'<<EscapeCSVField(sentences)<<'"';
            csvFile.flush();
            csvFile.close();
            wxLogMessage("Appended user %s (ID: %zu) to %s with default SENTENCES due to existing GMM", usernameLower.c_str(), participant_id, csvPath.u8string().c_str());
        }
        wxMessageBox(wxString::Format(_("Username '%s' is already registered."), username),
                     _("Error"), wxOK | wxICON_ERROR, this);
        return;
    }

    EnterArea->Enable(true);
    EnterArea->SetFocus();
}

void SignUp::OnKeyDown(wxKeyEvent& event) {
    int key = event.GetKeyCode();
    if (key == 306 || key == 307) {
        wxLogMessage("Ignoring Shift key down: %d", key);
        event.Skip();
        return;
    }
    KeyEvent ke;
    ke.key = key;
    ke.press_time = std::chrono::steady_clock::now();
    all_key_events.push_back(ke);
    key_events.push_back(ke);
    wxLogMessage("Key down: %d (char: %c)", ke.key, (ke.key >= 0 && ke.key < 128 && std::isprint(ke.key)) ? static_cast<char>(ke.key) : ' ');
    event.Skip();
}

void SignUp::OnKeyUp(wxKeyEvent& event) {
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

std::string SignUp::EscapeCSVField(const std::string& field) const {
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

void SignUp::SaveFeaturesToCSV(const std::string& filename, const std::vector<KeyEvent>& events, bool firstWrite) {
    std::ofstream csvFile(filename, firstWrite ? std::ios::out : std::ios::app);
    if (!csvFile.is_open()) {
        wxLogError("Failed to open CSV file: %s", filename.c_str());
        return;
    }

    if (firstWrite) {
        csvFile << "PARTICIPANT_ID,TEST_SECTION_ID,SENTENCE,USER_INPUT,KEYSTROKE_ID,PRESS_TIME,RELEASE_TIME,LETTER,KEYCODE\n";
        wxLogMessage("Wrote CSV header to %s", filename.c_str());
    }

    std::string username = m_textCtrl2->GetValue().Lower().ToStdString();
    std::hash<std::string> hasher;
    size_t participant_id = hasher(username);

    std::string sentence = Text->GetLabel().ToStdString();
    std::string user_input = EnterArea->GetValue().ToStdString();

    size_t test_section_id = currentTextIndex + 1;

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

        csvFile << participant_id << ","
                << test_section_id << ","
                << EscapeCSVField(sentence) << ","
                << EscapeCSVField(user_input) << ","
                << keystroke_id << ","
                << press_ms << ","
                << release_ms << ","
                << EscapeCSVField(letter) << ","
                << keycode << "\n";

        wxLogMessage("Wrote keystroke %zu: participant=%zu, test_section=%zu, keystroke_id=%zu, letter=%s, keycode=%d",
                     i + 1, participant_id, test_section_id, keystroke_id, letter.c_str(), keycode);
    }

    csvFile.flush();
    csvFile.close();
    wxLogMessage("Saved %zu keystrokes to %s (firstWrite: %d)", events.size(), filename.c_str(), firstWrite);
}

void SignUp::OnEnterPressed(wxCommandEvent& event) {
    wxString entered = EnterArea->GetValue().Trim().Trim(false);
    wxString expected = Text->GetLabel().Trim().Trim(false);

    wxLogMessage("Entered: '%s' (length: %zu)", entered.c_str(), entered.length());
    wxLogMessage("Expected: '%s' (length: %zu)", expected.c_str(), expected.length());
    wxLogMessage("Current sentence index: %zu", currentTextIndex);
    wxLogMessage("Raw key events collected: %zu", key_events.size());

    if (entered != expected) {
        wxMessageBox(wxString::Format(_("Text does not match. Entered: '%s', Expected: '%s'"), entered, expected),
                     _("Incorrect Input"), wxOK | wxICON_WARNING, this);
        key_events.clear();
        EnterArea->Clear();
        return;
    }

    // Add sentence to typedSentences
    typedSentences.insert(Text->GetLabel());
    wxLogMessage("Added sentence to typedSentences: '%s' (total: %zu)", Text->GetLabel().c_str(), typedSentences.size());

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
        key_events.clear();
        EnterArea->Clear();
        return;
    }

    wxLogMessage("Logging filtered events...");
    for (size_t i = 0; i < filteredEvents.size(); ++i) {
        wxLogMessage("Filtered event %zu: key=%d (char: %c), press_time=%lld, release_time=%lld",
                     i,
                     filteredEvents[i].key,
                     (filteredEvents[i].key >= 0 && filteredEvents[i].key < 128 && std::isprint(filteredEvents[i].key)) ? static_cast<char>(filteredEvents[i].key) : ' ',
                     std::chrono::duration_cast<std::chrono::milliseconds>(filteredEvents[i].press_time.time_since_epoch()).count(),
                     std::chrono::duration_cast<std::chrono::milliseconds>(filteredEvents[i].release_time.time_since_epoch()).count());
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

    std::vector<Vec2> currentFeatures;
    for (size_t i = 0; i < filteredEvents.size() - 1; ++i) {
        double dwell = std::chrono::duration_cast<std::chrono::microseconds>(filteredEvents[i].release_time - filteredEvents[i].press_time).count() / 1000.0;
        double flight = std::chrono::duration_cast<std::chrono::microseconds>(filteredEvents[i + 1].press_time - filteredEvents[i].release_time).count() / 1000.0;
        wxLogMessage("Processing feature %zu: key=%d (char: %c) to key=%d (char: %c), dwell=%f, flight=%f",
                     i + 1,
                     filteredEvents[i].key,
                     (filteredEvents[i].key >= 0 && filteredEvents[i].key < 128 && std::isprint(filteredEvents[i].key)) ? static_cast<char>(filteredEvents[i].key) : ' ',
                     filteredEvents[i + 1].key,
                     (filteredEvents[i + 1].key >= 0 && filteredEvents[i + 1].key < 128 && std::isprint(filteredEvents[i + 1].key)) ? static_cast<char>(filteredEvents[i + 1].key) : ' ',
                     dwell, flight);
        if (dwell >= 0 && flight >= -1000 && flight < 10000) {
            currentFeatures.emplace_back(dwell, flight);
            wxLogMessage("Added feature %zu: key=%d (char: %c), dwell=%f, flight=%f",
                         currentFeatures.size(),
                         filteredEvents[i].key,
                         (filteredEvents[i].key >= 0 && filteredEvents[i].key < 128 && std::isprint(filteredEvents[i].key)) ? static_cast<char>(filteredEvents[i].key) : ' ',
                         dwell, flight);
        } else {
            wxLogMessage("Skipped feature %zu: key=%d (char: %c), dwell=%f, flight=%f (out of bounds)",
                         i + 1,
                         filteredEvents[i].key,
                         (filteredEvents[i].key >= 0 && filteredEvents[i].key < 128 && std::isprint(filteredEvents[i].key)) ? static_cast<char>(filteredEvents[i].key) : ' ',
                         dwell, flight);
        }
    }

    wxLogMessage("Computed %zu features for current phrase", currentFeatures.size());

    features.insert(features.end(), currentFeatures.begin(), currentFeatures.end());

    wxString usernameLower = m_textCtrl2->GetValue().Lower();
    wxString csvFilename = usernameLower + "_keystrokes.csv";
    if (currentTextIndex < textSamples.size()) {
        SaveFeaturesToCSV(csvFilename.ToStdString(), filteredEvents, currentTextIndex == 0);
    }

    if (currentTextIndex < textSamples.size() - 1) {
        UpdateText();
        EnterArea->Clear();
        key_events.clear();
        wxLogMessage(wxString::Format(_("Completed sentence %zu of %zu. Total features so far: %zu"),
                                      currentTextIndex, textSamples.size(), features.size()));
        return;
    }

    if (features.size() < MIN_SAMPLES) {
        wxMessageBox(wxString::Format(_("Not enough valid keystroke data (%zu features) to train GMM. Required: %zu"),
                                      features.size(), MIN_SAMPLES),
                     _("Error"), wxOK | wxICON_ERROR, this);
        features.clear();
        key_events.clear();
        currentTextIndex = 0;
        Text->SetLabel(textSamples[0]);
        EnterArea->Clear();
        return;
    }

    GMM gmm(2);
    try {
        double max_dwell = 0, max_flight = 0;
        for (const auto& f : features) {
            if (std::isnan(f.x) || std::isinf(f.x) || std::isnan(f.y) || std::isinf(f.y)) {
                throw std::runtime_error("Invalid feature values detected");
            }
            max_dwell = std::max(max_dwell, std::abs(f.x));
            max_flight = std::max(max_flight, std::abs(f.y));
        }
        std::vector<Vec2> norm_features = features;
        for (auto& f : norm_features) {
            f.x /= max_dwell ? max_dwell : 1.0;
            f.y /= max_flight ? max_flight : 1.0;
        }

        gmm.train(norm_features);
        double ll = gmm.logLikelihood(norm_features);
        wxLogMessage(wxString::Format(_("Log likelihood after training: %f"), ll));
        if (std::isnan(ll) || std::isinf(ll)) {
            wxMessageBox(_("GMM training produced NaN values."), _("Error"), wxOK | wxICON_ERROR, this);
            return;
        }

        wxString gmmFilename = "./resources/" + usernameLower + ".gmm";
        gmm.save(gmmFilename.ToStdString());

        namespace fs = std::filesystem;
        fs::path csvPath = fs::u8path("./resources/users.csv");
        bool userInCSV = false;
        std::vector<std::string> existingLines;
        std::string existingPid;
        if (fs::exists(csvPath)) {
            std::ifstream userFile(csvPath, std::ios::binary);
            if (!userFile.is_open()) {
                wxLogError("Cannot open users.csv for reading: %s", csvPath.u8string().c_str());
            } else {
                std::string line;
                std::getline(userFile, line);
                existingLines.push_back("PARTICIPANT_ID,NAME,SENTENCES");
                while (std::getline(userFile, line)) {
                    if (line.empty()) continue;
                    std::stringstream ss(line);
                    std::string pid, name, sentences;
                    if (std::getline(ss, pid, ',') && std::getline(ss, name, ',')) {
                        std::string normalizedName = name;
                        std::transform(normalizedName.begin(), normalizedName.end(), normalizedName.begin(), ::tolower);
                        if (normalizedName == usernameLower.ToStdString()) {
                            userInCSV = true;
                            existingPid = pid;
                            wxLogMessage("Found user '%s' in users.csv during signup", name.c_str());
                        } else {
                            existingLines.push_back(line);
                        }
                    }
                }
                userFile.close();
            }
        }

        if (!userInCSV) {
            bool firstWrite = !fs::exists(csvPath);
            std::ofstream csvFile(csvPath, std::ios::out | std::ios::binary);
            if (!csvFile.is_open()) {
                wxLogError("Failed to open users.csv for writing: %s", csvPath.u8string().c_str());
                wxMessageBox(_("Failed to save user data to users.csv."), _("Error"), wxOK | wxICON_ERROR, this);
                return;
            }

            // Write header and existing lines
            for (const auto& line : existingLines) {
                csvFile << line << "\n";
            }

            // Join typedSentences for SENTENCES column
            std::string sentences;
            size_t i = 0;
            for (const auto& sentence : typedSentences) {
                sentences += sentence.ToStdString();
                if (i < typedSentences.size() - 1) sentences += "|";
                ++i;
            }
            wxLogMessage("Saving sentences to users.csv: %s", sentences.c_str());

            std::hash<std::string> hasher;
            size_t participant_id = hasher(usernameLower.ToStdString());
            csvFile << "\n"<<participant_id << ","
                    << EscapeCSVField(m_textCtrl2->GetValue().ToStdString()) << ","
                    << '"'<<EscapeCSVField(sentences) <<'"';
            csvFile.flush();
            csvFile.close();
            wxLogMessage("Appended user %s (ID: %zu) to %s with sentences", usernameLower.c_str(), participant_id, csvPath.u8string().c_str());
        } else {
            // Update existing user's SENTENCES
            std::ofstream csvFile(csvPath, std::ios::out | std::ios::binary);
            if (!csvFile.is_open()) {
                wxLogError("Failed to open users.csv for writing: %s", csvPath.u8string().c_str());
                wxMessageBox(_("Failed to save user data to users.csv."), _("Error"), wxOK | wxICON_ERROR, this);
                return;
            }

            // Write header and existing lines
            for (const auto& line : existingLines) {
                csvFile << line << "\n";
            }

            // Join typedSentences for SENTENCES column
            std::string sentences;
            size_t i = 0;
            for (const auto& sentence : typedSentences) {
                sentences += sentence.ToStdString();
                if (i < typedSentences.size() - 1) sentences += "|";
                ++i;
            }
            wxLogMessage("Updating sentences in users.csv for %s: %s", usernameLower.c_str(), sentences.c_str());

            csvFile << existingPid << ","
                    << EscapeCSVField(m_textCtrl2->GetValue().ToStdString()) << ","
                    << EscapeCSVField(sentences) << "\n";
            csvFile.flush();
            csvFile.close();
            wxLogMessage("Updated user %s (ID: %s) in %s with sentences", usernameLower.c_str(), existingPid.c_str(), csvPath.u8string().c_str());
        }

        wxMessageBox(wxString::Format(_("User %s signed up successfully with %zu keystrokes! Redirecting to Home Page..."),
                                      m_textCtrl2->GetValue(), features.size()),
                     _("Success"), wxOK | wxICON_INFORMATION, this);
        HomePage* homeWindow = new HomePage(nullptr, wxID_ANY, "Home");
        homeWindow->Show(true);
        this->Close(true);
    } catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("GMM training failed: %s"), wxString(e.what(), wxConvUTF8)),
                     _("Error"), wxOK | wxICON_ERROR, this);
    }
}

void SignUp::UpdateText() {
    currentTextIndex = currentTextIndex + 1;
    Text->SetLabel(textSamples[currentTextIndex]);
    Text->Wrap(600);
    wxLogMessage("Updated text to: '%s' (index: %zu)", Text->GetLabel().c_str(), currentTextIndex);
}