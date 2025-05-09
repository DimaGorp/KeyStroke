#include "HomePage/HomePage.hpp"
#include "LogInPage/LoginPage.hpp"
#include "SignUpPage/SignupPage.hpp"
#include "Config.h"
#include <wx/log.h>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <filesystem>
#include <random>

wxBEGIN_EVENT_TABLE(HomePage, wxFrame)
    EVT_BUTTON(wxID_ANY, HomePage::OnLoginClick)
    EVT_BUTTON(wxID_ANY, HomePage::OnSignUpClick)
wxEND_EVENT_TABLE()

HomePage::HomePage(wxWindow* parent, wxWindowID id, const wxString& title, 
                   const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style) {
    wxLog::SetActiveTarget(new wxLogStderr());
    wxLogMessage("HomePage window initialized");

    this->SetSizeHints(wxSize(1200, 800), wxSize(1200, 800));
    this->SetPosition(wxPoint(1200-600, 800-400));
    this->SetBackgroundColour(wxColour(255, 240, 255));

    wxBoxSizer* Area = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* Context = new wxBoxSizer(wxVERTICAL);

    Welcome_Text = new wxStaticText(this, wxID_ANY, _("Welcome,User"), 
                                   wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
    Welcome_Text->SetLabelMarkup(_("Welcome,User"));
    Welcome_Text->Wrap(-1);
    Welcome_Text->SetFont(wxFont(32, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                                wxFONTWEIGHT_LIGHT, false, wxT("Century Gothic")));
    Context->Add(Welcome_Text, 0, wxEXPAND | wxALL, 10);

    Description = new wxStaticText(this, wxID_ANY, 
                                  _("That is a demonstration project of techic \"Keystroke dynamics\" as a part of Behavioral Biometrics Authentication topic"),
                                  wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
    Description->Wrap(300);
    Description->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                               wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Context->Add(Description, 0, wxEXPAND | wxALL, 10);

    wxBoxSizer* Buttons = new wxBoxSizer(wxHORIZONTAL);

    LogIN_BTN = new wxButton(this, wxID_ANY, _("Login"), wxDefaultPosition, wxSize(200, 30), wxBORDER_NONE);
    LogIN_BTN->SetDefault();
    LogIN_BTN->SetLabelMarkup(_("Login"));
    LogIN_BTN->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                             wxFONTWEIGHT_BOLD, false, wxT("Arial")));
    LogIN_BTN->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    LogIN_BTN->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    Buttons->Add(LogIN_BTN, 0, wxALL, 5);

    Buttons->AddStretchSpacer(1);

    SignUpBtn = new wxButton(this, wxID_ANY, _("SignUp"), wxDefaultPosition, wxSize(200, 30), wxBORDER_NONE);
    SignUpBtn->SetLabelMarkup(_("SignUp"));
    SignUpBtn->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                             wxFONTWEIGHT_BOLD, false, wxT("Arial")));
    SignUpBtn->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    SignUpBtn->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
    Buttons->Add(SignUpBtn, 0, wxALL, 5);

    Context->Add(Buttons, 0, wxEXPAND | wxTOP, 30);
    Area->Add(Context, 1, wxALIGN_CENTER | wxALL, 100);

    this->SetSizer(Area);
    this->Layout();
    this->Centre(wxBOTH);

    LogIN_BTN->Bind(wxEVT_ENTER_WINDOW, &HomePage::OnLoginEnter, this);
    LogIN_BTN->Bind(wxEVT_LEAVE_WINDOW, &HomePage::OnLoginLeave, this);
    SignUpBtn->Bind(wxEVT_ENTER_WINDOW, &HomePage::OnSignUpEnter, this);
    SignUpBtn->Bind(wxEVT_LEAVE_WINDOW, &HomePage::OnSignUpLeave, this);
    LogIN_BTN->Bind(wxEVT_BUTTON, &HomePage::OnLoginClick, this);
    SignUpBtn->Bind(wxEVT_BUTTON, &HomePage::OnSignUpClick, this);

    try {
        GenerateCSVWithRandomNames(KEYSTROKES_PATH, "resources/users.csv");
        TrainGMMsForParticipants(std::string(KEYSTROKES_PATH), "resources/users.csv");
    } catch (...) {
        wxLogError("Unhandled exception in initialization");
        wxMessageBox(_("An unexpected error occurred during initialization."), _("Error"), wxOK | wxICON_ERROR, this);
    }
}

HomePage::~HomePage() {}

std::string HomePage::generateRandomName(int length) {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distrib(0, characters.size() - 1);

    std::stringstream nameStream;
    for (int i = 0; i < length; ++i) {
        nameStream << characters[distrib(generator)];
    }
    return nameStream.str();
}

std::string HomePage::EscapeCSVField(const std::string& field) const {
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

void HomePage::GenerateCSVWithRandomNames(const std::string& directoryPath, const std::string& outputCSV) {
    namespace fs = std::filesystem;

    try {
        // Specify the folder 'resources' where files will be saved
        fs::path outputPath = fs::u8path(outputCSV);

        // Create the folder if it does not exist
        if (!fs::exists("resources")) {
            fs::create_directory("resources");
        }

        // Check if the CSV file already exists
        if (fs::exists(outputPath)) {
            wxLogMessage("users.csv already exists, skipping generation.");
            return;
        }

        fs::path inputPath = fs::u8path(directoryPath);

        if (!fs::exists(inputPath) || !fs::is_directory(inputPath)) {
            wxString wxDirPath = wxString::FromUTF8(directoryPath);
            wxLogMessage("Debug path: %s", wxDirPath);
            wxTextEntryDialog dlg(nullptr, "Dataset directory does not exist.\nYou can copy the full path here:", 
                                  "Directory Error", wxDirPath);
            dlg.ShowModal();
            wxLogError("Dataset directory does not exist:\n%s", wxDirPath);
            return;
        }

        std::ofstream outFile(outputPath);
        if (!outFile.is_open()) {
            wxLogError("Could not open output CSV file: %s", wxString::FromUTF8(outputCSV));
            return;
        }

        outFile << "PARTICIPANT_ID,NAME,SENTENCES\n";

        for (const auto& entry : fs::directory_iterator(inputPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::string filename = entry.path().filename().string();
                std::string participantID = filename.substr(0, filename.find('_'));
                std::string name;
                do {
                    name = generateRandomName(8);
                } while (name == participantID);

                // Read dataset file to extract unique sentences
                std::set<std::string> uniqueSentences;
                std::ifstream dataFile(entry.path());
                if (!dataFile.is_open()) {
                    wxLogError("Could not open dataset file: %s", filename.c_str());
                    continue;
                }

                std::string line;
                std::getline(dataFile, line); // Skip header
                while (std::getline(dataFile, line)) {
                    std::stringstream ss(line);
                    std::vector<std::string> tokens;
                    std::string token;
                    while (std::getline(ss, token, '\t')) {
                        tokens.push_back(token);
                    }
                    if (tokens.size() >= 3) { // SENTENCE is 3rd column (index 2)
                        std::string sentence = tokens[2];
                        // Trim quotes and whitespace
                        sentence.erase(0, sentence.find_first_not_of(" \t\r\n\""));
                        sentence.erase(sentence.find_last_not_of(" \t\r\n\"") + 1);
                        if (!sentence.empty()) {
                            uniqueSentences.insert(sentence);
                        }
                    }
                }
                dataFile.close();

                // Join unique sentences with |
                std::string sentences;
                size_t i = 0;
                for (const auto& sentence : uniqueSentences) {
                    sentences += sentence;
                    if (i < uniqueSentences.size() - 1) sentences += "|";
                    ++i;
                }

                outFile << participantID << "," << EscapeCSVField(name) << "," << EscapeCSVField(sentences) << "\n";
            }
        }

        wxLogMessage("CSV successfully saved to: %s", wxString::FromUTF8(outputCSV));
    } catch (const std::exception& e) {
        wxLogError("Exception occurred during CSV generation:\n%s", wxString(e.what()));
    }
}

void HomePage::TrainGMMsForParticipants(const std::string& dataDir, const std::string& userCSV) {
    namespace fs = std::filesystem;

    // Get the resources directory inside the build folder
    std::string resourcesDir = "resources";

    // Check if resources directory exists, if not, create it
    fs::path resourcesPath = fs::u8path(resourcesDir);
    if (!fs::exists(resourcesPath)) {
        try {
            fs::create_directory(resourcesPath);
        } catch (const std::exception& e) {
            wxLogError("Could not create resources directory: %s", e.what());
            return;
        }
    }

    // Load users.csv to map PARTICIPANT_ID to NAME
    std::map<unsigned long long, std::string> participantToName;
    std::ifstream userFile(userCSV);
    if (!userFile.is_open()) {
        wxLogError("Could not open user CSV: %s", userCSV.c_str());
        wxMessageBox(_("Failed to load user database for GMM training."), _("Error"), wxOK | wxICON_ERROR, this);
        return;
    }

    std::string line;
    std::getline(userFile, line); // Skip header
    while (std::getline(userFile, line)) {
        std::stringstream ss(line);
        std::string pid_str, name, sentences;
        if (std::getline(ss, pid_str, ',') && std::getline(ss, name, ',') && std::getline(ss, sentences)) {
            // Trim quotes and whitespace from name
            name.erase(0, name.find_first_not_of(" \t\r\n\""));
            name.erase(name.find_last_not_of(" \t\r\n\"") + 1);
            try {
                unsigned long long participant_id = std::stoull(pid_str);
                participantToName[participant_id] = name;
                wxLogMessage("Mapped PARTICIPANT_ID %llu to NAME %s", participant_id, name.c_str());
            } catch (const std::exception& e) {
                wxLogError("Invalid PARTICIPANT_ID in %s: %s (%s)", userCSV.c_str(), line.c_str(), e.what());
            }
        }
    }
    userFile.close();

    // Iterate through dataset files
    fs::path dataPath = fs::u8path(dataDir);
    if (!fs::exists(dataPath) || !fs::is_directory(dataPath)) {
        wxLogError("Dataset directory does not exist: %s", dataDir.c_str());
        wxMessageBox(_("Dataset directory does not exist for GMM training."), _("Error"), wxOK | wxICON_ERROR, this);
        return;
    }

    for (const auto& entry : fs::directory_iterator(dataPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            std::string filename = entry.path().filename().string();
            std::string pid_str = filename.substr(0, filename.find('_'));
            unsigned long long participant_id;
            try {
                participant_id = std::stoull(pid_str);
            } catch (const std::exception& e) {
                wxLogError("Invalid PARTICIPANT_ID in filename: %s (%s)", filename.c_str(), e.what());
                continue;
            }

            auto it = participantToName.find(participant_id);
            if (it == participantToName.end()) {
                wxLogError("No username found for PARTICIPANT_ID %llu", participant_id);
                continue;
            }
            std::string username = it->second;

            // Read keystroke data
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                wxLogError("Could not open dataset file: %s", filename.c_str());
                continue;
            }

            std::vector<KeyEvent> key_events;
            std::getline(file, line); // Skip header
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string token;
                std::vector<std::string> tokens;
                while (std::getline(ss, token, '\t')) {
                    tokens.push_back(token);
                }
                if (tokens.size() < 9) {
                    wxLogError("Malformed line in %s: %s", filename.c_str(), line.c_str());
                    continue;
                }

                try {
                    KeyEvent ke;
                    ke.key = std::stoi(tokens[8]); // KEYCODE
                    ke.press_time = std::chrono::steady_clock::time_point(
                        std::chrono::milliseconds(std::stoll(tokens[5]))); // PRESS_TIME
                    ke.release_time = std::chrono::steady_clock::time_point(
                        std::chrono::milliseconds(std::stoll(tokens[6]))); // RELEASE_TIME
                    key_events.push_back(ke);
                } catch (const std::exception& e) {
                    wxLogError("Error parsing line in %s: %s (%s)", filename.c_str(), line.c_str(), e.what());
                }
            }
            file.close();

            // Compute features
            std::vector<Vec2> features;
            for (size_t i = 0; i < key_events.size() - 1; ++i) {
                if (key_events[i].release_time != std::chrono::steady_clock::time_point() &&
                    key_events[i + 1].press_time != std::chrono::steady_clock::time_point()) {
                    double dwell = std::chrono::duration<double, std::milli>(key_events[i].release_time - key_events[i].press_time).count();
                    double flight = std::chrono::duration<double, std::milli>(key_events[i + 1].press_time - key_events[i].release_time).count();
                    if (dwell >= 0 && flight >= -1000 && flight < 10000) {
                        features.emplace_back(dwell, flight);
                        wxLogMessage("Feature for %s: dwell=%f, flight=%f", username.c_str(), dwell, flight);
                    }
                }
            }

            if (features.size() < 50) { // Match MIN_SAMPLES from SignUp/Login
                wxLogError("Not enough features (%zu) for %s", features.size(), username.c_str());
                continue;
            }

            // Train GMM
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
                wxLogMessage("Trained GMM for %s, log likelihood: %f", username.c_str(), ll);
                if (std::isnan(ll) || std::isinf(ll)) {
                    wxLogError("Invalid log likelihood for %s", username.c_str());
                    continue;
                }

                // Save GMM file inside the resources directory
                std::string gmmFile = (resourcesPath / (username + ".gmm")).string();
                gmm.save(gmmFile);
                wxLogMessage("Saved GMM to %s", gmmFile.c_str());
            } catch (const std::exception& e) {
                wxLogError("GMM training failed for %s: %s", username.c_str(), e.what());
            }
        }
    }

    wxLogMessage("Completed GMM training for participants");
}

void HomePage::OnLoginEnter(wxMouseEvent& event) {
    LogIN_BTN->SetBackgroundColour(wxColour(135, 206, 250));
    LogIN_BTN->SetForegroundColour(wxColour(0, 0, 0));
    LogIN_BTN->Refresh();
}

void HomePage::OnLoginLeave(wxMouseEvent& event) {
    LogIN_BTN->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    LogIN_BTN->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    LogIN_BTN->Refresh();
}

void HomePage::OnSignUpEnter(wxMouseEvent& event) {
    SignUpBtn->SetBackgroundColour(wxColour(173, 216, 230));
    SignUpBtn->SetForegroundColour(wxColour(0, 0, 0));
    SignUpBtn->Refresh();
}

void HomePage::OnSignUpLeave(wxMouseEvent& event) {
    SignUpBtn->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
    SignUpBtn->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    SignUpBtn->Refresh();
}

void HomePage::OnLoginClick(wxCommandEvent& event) {
    Login* loginWindow = new Login(nullptr, wxID_ANY, _("Login"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
    loginWindow->Show(true);
    this->Close(true);
}

void HomePage::OnSignUpClick(wxCommandEvent& event) {
    SignUp* signupWindow = new SignUp(nullptr, wxID_ANY, _("Signup"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
    signupWindow->Show(true);
    this->Close(true);
}