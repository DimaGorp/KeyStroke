#include <wx/wx.h>
#include <wx/log.h>
#include "SignupPage/SignupPage.hpp"
#include "HomePage/HomePage.hpp"
#include <fstream>
#include <cctype>

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

void SignUp::SaveFeaturesToCSV(const std::string& filename, const std::vector<KeyEvent>& events, 
    const std::vector<Vec2>& feats, bool firstWrite) {
    std::ofstream csvFile(filename, firstWrite ? std::ios::out : std::ios::app);
    if (csvFile.is_open()) {
    if (firstWrite) {
    csvFile << "Key,DwellTime(ms),FlightTime(ms)\n";
    wxLogMessage("Wrote CSV header to %s", filename.c_str());
    }
    wxLogMessage("Saving %zu features to %s", feats.size(), filename.c_str());
    for (size_t i = 0; i < feats.size(); ++i) {
    int key = events[i].key;
    char keyChar = (key >= 0 && key < 128 && std::isprint(key)) ? static_cast<char>(key) : ' ';
    csvFile << keyChar << "," << feats[i].x << "," << feats[i].y << "\n";
    wxLogMessage("Wrote feature %zu: key=%d (char: %c), dwell=%f, flight=%f", 
    i + 1, key, keyChar, feats[i].x, feats[i].y);
    }
    csvFile.flush(); // Добавляем сброс буфера
    csvFile.close();
    wxLogMessage("Appended %zu features to %s (firstWrite: %d)", feats.size(), filename.c_str(), firstWrite);
    } else {
    wxLogError("Failed to open CSV file: %s", filename.c_str());
    }
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

    // Логируем все события в filteredEvents
    wxLogMessage("Logging filtered events...");
    for (size_t i = 0; i < filteredEvents.size(); ++i) {
        wxLogMessage("Filtered event %zu: key=%d (char: %c), press_time=%lld, release_time=%lld", 
                     i, 
                     filteredEvents[i].key, 
                     (filteredEvents[i].key >= 0 && filteredEvents[i].key < 128 && std::isprint(filteredEvents[i].key)) ? static_cast<char>(filteredEvents[i].key) : ' ',
                     std::chrono::duration_cast<std::chrono::milliseconds>(filteredEvents[i].press_time.time_since_epoch()).count(),
                     std::chrono::duration_cast<std::chrono::milliseconds>(filteredEvents[i].release_time.time_since_epoch()).count());
    }

    // Даем больше времени для обработки событий
    wxMilliSleep(100); // Ждем 100 мс
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
        double dwell = std::chrono::duration<double, std::milli>(filteredEvents[i].release_time - filteredEvents[i].press_time).count();
        double flight = std::chrono::duration<double, std::milli>(filteredEvents[i + 1].press_time - filteredEvents[i].release_time).count();
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

    wxString username = m_textCtrl2->GetValue();
    wxString csvFilename = username + "_keystrokes.csv";
    if (currentTextIndex < textSamples.size()) {
        if (currentFeatures.size() == filteredEvents.size() - 1) {
            SaveFeaturesToCSV(csvFilename.ToStdString(), filteredEvents, currentFeatures, currentTextIndex == 0);
        } else {
            wxLogError("Mismatch: %zu features computed, expected %zu", currentFeatures.size(), filteredEvents.size() - 1);
        }
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

        wxString gmmFilename = username + ".gmm";
        gmm.save(gmmFilename.ToStdString());

        wxMessageBox(wxString::Format(_("User %s signed up successfully with %zu keystrokes! Redirecting to Home Page..."), 
                                      username, features.size()),
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