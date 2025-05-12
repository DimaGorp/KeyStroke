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
#include <thread>
#include <chrono>
#include <cmath>
#include <numeric>
#include <algorithm>

wxBEGIN_EVENT_TABLE(HomePage, wxFrame)
    EVT_BUTTON(wxID_ANY, HomePage::OnLoginClick)
    EVT_BUTTON(wxID_ANY, HomePage::OnSignUpClick)
wxEND_EVENT_TABLE()

HomePage::HomePage(wxWindow* parent, wxWindowID id, const wxString& title, 
                   const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style) {
    wxLog::SetActiveTarget(new wxLogStderr());
    wxLogMessage("HomePage window initialized");

    namespace fs = std::filesystem;
    wxLogMessage("Current working directory: %s", fs::current_path().string().c_str());
    wxLogMessage("Using KEYSTROKES_PATH: %s", KEYSTROKES_PATH);

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
                                  _("That is a demonstration project of technic \"Keystroke dynamics\" as a part of Behavioral Biometrics Authentication topic"),
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
        fs::create_directories("resources\\csv");
        wxLogMessage("Created directory: resources\\csv");

        GenerateCSVWithRandomNames(KEYSTROKES_PATH, "resources\\users.csv");
        if (!fs::exists(fs::u8path("resources\\users.csv"))) {
            wxLogError("Failed to generate resources\\users.csv");
            return;
        }
        wxLogMessage("Generated resources\\users.csv");

        PrepaDataset(KEYSTROKES_PATH, "resources\\csv\\prepared_dataset.csv");
        if (!fs::exists(fs::u8path("resources\\csv\\prepared_dataset.csv"))) {
            wxLogError("Failed to generate resources\\csv\\prepared_dataset.csv");
            return;
        }
        wxLogMessage("Generated resources\\csv\\prepared_dataset.csv");

        TrainGMMsForParticipants("resources\\csv\\prepared_dataset.csv", "resources\\users.csv");
        EvaluateGMMsForParticipants("resources\\csv\\prepared_dataset.csv", "resources\\users.csv");
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
        fs::path outputPath = fs::u8path(outputCSV);
        if (!fs::exists("resources")) {
            fs::create_directory("resources");
        }

        if (fs::exists(outputPath)) {
            wxLogMessage("users.csv already exists, skipping generation.");
            return;
        }

        fs::path inputPath = fs::u8path(directoryPath);
        if (!fs::exists(inputPath) || !fs::is_directory(inputPath)) {
            wxLogError("Dataset directory does not exist: %s", directoryPath.c_str());
            return;
        }

        std::ofstream outFile(outputPath);
        if (!outFile.is_open()) {
            wxLogError("Could not open output CSV file: %s", outputCSV.c_str());
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
                    if (tokens.size() >= 3) {
                        std::string sentence = tokens[2];
                        sentence.erase(0, sentence.find_first_not_of(" \t\r\n\""));
                        sentence.erase(sentence.find_last_not_of(" \t\r\n\"") + 1);
                        if (!sentence.empty()) {
                            uniqueSentences.insert(sentence);
                        }
                    }
                }
                dataFile.close();

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

        std::ifstream checkFile(outputPath);
        size_t line_count = 0;
        std::string line;
        while (std::getline(checkFile, line)) line_count++;
        checkFile.close();
        wxLogMessage("Generated %s with %zu lines (including header)", outputCSV.c_str(), line_count);
    } catch (const std::exception& e) {
        wxLogError("Exception occurred during CSV generation: %s", e.what());
    }
}

void HomePage::PrepaDataset(const std::string& inputDir, const std::string& outputCSV) {
    namespace fs = std::filesystem;

    wxLogMessage("PrepaDataset called with inputDir=%s, outputCSV=%s", inputDir.c_str(), outputCSV.c_str());

    fs::path outputPath = fs::u8path(outputCSV);
    
    // Check if output CSV already exists
    if (fs::exists(outputPath)) {
        wxLogMessage("Skipping PrepaDataset: %s already exists", outputCSV.c_str());
        return;
    }

    fs::path inputPath = fs::u8path(inputDir);
    if (!fs::exists(inputPath) || !fs::is_directory(inputPath)) {
        wxLogError("Input directory does not exist: %s", inputDir.c_str());
        return;
    }

    wxLogMessage("Creating directory: %s", outputPath.parent_path().u8string().c_str());
    fs::create_directories(outputPath.parent_path());

    // Open file in write mode (only if it doesn't exist)
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        wxLogError("Could not open output CSV: %s", outputCSV.c_str());
        return;
    }
    wxLogMessage("Successfully opened %s for writing", outputCSV.c_str());

    // Write header
    outFile << "PARTICIPANT_ID,USERNAME,DWELL,FLIGHT,NORM_DWELL,NORM_FLIGHT,MAX_DWELL,MAX_FLIGHT\n";
    if (!outFile.good()) {
        wxLogError("Failed to write header to %s", outputCSV.c_str());
        outFile.close();
        return;
    }
    wxLogMessage("Wrote header to %s", outputCSV.c_str());

    std::map<unsigned long long, std::string> participantToName;
    std::ifstream userFile(fs::u8path("resources/users.csv"));
    if (userFile.is_open()) {
        std::string line;
        std::getline(userFile, line); // Skip header
        while (std::getline(userFile, line)) {
            std::stringstream ss(line);
            std::string pid_str, name, sentences;
            if (std::getline(ss, pid_str, ',') && std::getline(ss, name, ',')) {
                name.erase(0, name.find_first_not_of(" \t\r\n\""));
                name.erase(name.find_last_not_of(" \t\r\n\"") + 1);
                try {
                    unsigned long long participant_id = std::stoull(pid_str);
                    participantToName[participant_id] = name;
                } catch (const std::exception& e) {
                    wxLogWarning("Invalid PARTICIPANT_ID in users.csv: %s (%s)", line.c_str(), e.what());
                }
            }
        }
        userFile.close();
        wxLogMessage("Loaded %zu username mappings from users.csv", participantToName.size());
    } else {
        wxLogWarning("Could not open resources/users.csv for username mapping");
    }

    for (const auto& entry : fs::directory_iterator(inputPath)) {
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

            std::string username = participantToName.count(participant_id) ? 
                                  participantToName[participant_id] : generateRandomName(8);

            std::ifstream file(entry.path());
            if (!file.is_open()) {
                wxLogError("Could not open dataset file: %s", filename.c_str());
                continue;
            }

            std::vector<KeyEvent> key_events;
            std::string line;
            std::getline(file, line); // Skip header
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string token;
                std::vector<std::string> tokens;
                while (std::getline(ss, token, '\t')) {
                    tokens.push_back(token);
                }
                if (tokens.size() < 9) {
                    wxLogError("Invalid line in %s: %zu columns (expected >= 9)", filename.c_str(), tokens.size());
                    continue;
                }

                try {
                    KeyEvent ke;
                    ke.key = std::stoi(tokens[8]);
                    ke.press_time = std::chrono::steady_clock::time_point(
                        std::chrono::milliseconds(std::stoll(tokens[5])));
                    ke.release_time = std::chrono::steady_clock::time_point(
                        std::chrono::milliseconds(std::stoll(tokens[6])));
                    if (ke.release_time >= ke.press_time) { // Validate timestamp order
                        key_events.push_back(ke);
                    } else {
                        wxLogWarning("Invalid timestamp order for PID %llu: press=%lld, release=%lld",
                                     participant_id, std::stoll(tokens[5]), std::stoll(tokens[6]));
                    }
                } catch (const std::exception& e) {
                    wxLogWarning("Error parsing line in %s: %s (%s)", filename.c_str(), line.c_str(), e.what());
                }
            }
            file.close();

            // Sort key events by press time
            std::sort(key_events.begin(), key_events.end(),
                      [](const KeyEvent& a, const KeyEvent& b) {
                          return a.press_time < b.press_time;
                      });

            double max_dwell = 0, max_flight = 0;
            std::vector<std::tuple<double, double>> features;
            size_t filtered_count = 0;
            for (size_t i = 0; i < key_events.size() - 1; ++i) {
                double dwell = std::chrono::duration<double, std::milli>(
                    key_events[i].release_time - key_events[i].press_time).count();
                double flight = std::chrono::duration<double, std::milli>(
                    key_events[i + 1].press_time - key_events[i].release_time).count();
                if (dwell > 0 && dwell < 2000 && flight > 0 && flight < 10000 &&
                    std::isfinite(dwell) && std::isfinite(flight)) {
                    features.emplace_back(dwell, flight);
                    max_dwell = std::max(max_dwell, dwell);
                    max_flight = std::max(max_flight, flight);
                } else {
                    wxLogWarning("Filtered feature for PID %llu (%s): dwell=%f, flight=%f",
                                 participant_id, username.c_str(), dwell, flight);
                    filtered_count++;
                }
            }
            wxLogMessage("PID %llu (%s): %zu features filtered out", participant_id, username.c_str(), filtered_count);

            for (const auto& [dwell, flight] : features) {
                double norm_dwell = max_dwell ? dwell / max_dwell : 0.0;
                double norm_flight = max_flight ? flight / max_flight : 0.0;
                if (std::isfinite(norm_dwell) && std::isfinite(norm_flight)) {
                    outFile << participant_id << "," << EscapeCSVField(username) << "," 
                            << dwell << "," << flight << ","
                            << norm_dwell << "," << norm_flight << "," 
                            << max_dwell << "," << max_flight << "\n";
                    if (!outFile.good()) {
                        wxLogError("Write error for PID %llu to %s", participant_id, outputCSV.c_str());
                        outFile.close();
                        return;
                    }
                }
            }
            wxLogMessage("Prepared data for PID %llu (%s): %zu features", participant_id, username.c_str(), features.size());
        }
    }

    outFile.flush();
    if (!outFile.good()) {
        wxLogError("Flush failed for %s", outputCSV.c_str());
    }
    outFile.close();
    wxLogMessage("Generated %s with new data", outputCSV.c_str());

    // Verify file contents
    std::ifstream verifyFile(outputPath, std::ios::binary);
    if (verifyFile.is_open()) {
        std::string line;
        size_t lineCount = 0;
        while (std::getline(verifyFile, line)) {
            lineCount++;
        }
        verifyFile.close();
        wxLogMessage("Verified %s: contains %zu lines", outputCSV.c_str(), lineCount);
    } else {
        wxLogError("Could not open %s for verification", outputCSV.c_str());
    }
}

void HomePage::TrainGMMsForParticipants(const std::string& dataCSV, const std::string& userCSV) {
    namespace fs = std::filesystem;

    std::ifstream userFile;
    int retries = 5;
    while (retries-- > 0) {
        userFile.open(fs::u8path(userCSV));
        if (userFile.is_open()) break;
        wxLogWarning("Retrying to open user CSV: %s (%d retries left)", userCSV.c_str(), retries);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    if (!userFile.is_open()) {
        wxLogError("Could not open user CSV: %s", userCSV.c_str());
        return;
    }

    std::map<unsigned long long, std::string> participantToName;
    std::string line;
    std::getline(userFile, line); // Skip header
    while (std::getline(userFile, line)) {
        std::stringstream ss(line);
        std::string pid_str, name, sentences;
        if (std::getline(ss, pid_str, ',') && std::getline(ss, name, ',')) {
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

    std::ifstream dataFile;
    retries = 5;
    while (retries-- > 0) {
        dataFile.open(fs::u8path(dataCSV));
        if (dataFile.is_open()) break;
        wxLogWarning("Retrying to open prepared CSV: %s (%d retries left)", dataCSV.c_str(), retries);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    if (!dataFile.is_open()) {
        wxLogError("Could not open prepared CSV: %s", dataCSV.c_str());
        return;
    }

    std::map<unsigned long long, std::vector<Vec2>> participantFeatures;
    std::map<unsigned long long, std::pair<double, double>> normalizationRanges; // Store max_dwell, max_flight
    size_t invalid_lines = 0;
    std::getline(dataFile, line); // Skip header
    while (std::getline(dataFile, line)) {
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 8) {
            wxLogWarning("Malformed line in %s: %s", dataCSV.c_str(), line.c_str());
            invalid_lines++;
            continue;
        }

        try {
            unsigned long long pid = std::stoull(tokens[0]);
            double norm_dwell = std::stod(tokens[4]);
            double norm_flight = std::stod(tokens[5]);
            double max_dwell = std::stod(tokens[6]);
            double max_flight = std::stod(tokens[7]);
            if (std::isfinite(norm_dwell) && std::isfinite(norm_flight)) {
                participantFeatures[pid].emplace_back(norm_dwell, norm_flight);
                normalizationRanges[pid] = {max_dwell, max_flight};
            }
        } catch (const std::exception& e) {
            wxLogWarning("Error parsing line in %s: %s (%s)", dataCSV.c_str(), line.c_str(), e.what());
            invalid_lines++;
        }
    }
    dataFile.close();
    wxLogMessage("Loaded %s: %zu participants, %zu invalid lines", dataCSV.c_str(), participantFeatures.size(), invalid_lines);

    std::set<unsigned long long> unique_pids;
    for (const auto& [pid, features] : participantFeatures) {
        unique_pids.insert(pid);
    }
    wxLogMessage("Found %zu unique PARTICIPANT_IDs in prepared_dataset.csv", unique_pids.size());

    size_t successful_gmms = 0;
    for (const auto& [pid, username] : participantToName) {
        wxLogMessage("Training GMM for user %s (PARTICIPANT_ID %llu)", username.c_str(), pid);

        auto it = participantFeatures.find(pid);
        if (it == participantFeatures.end()) {
            wxLogError("No features found for user %s (PARTICIPANT_ID %llu)", username.c_str(), pid);
            continue;
        }

        auto features = it->second;
        wxLogMessage("User %s: Features=%zu", username.c_str(), features.size());

        if (features.size() < 50) {
            wxLogError("Not enough features (%zu < 50) for user %s", features.size(), username.c_str());
            continue;
        }

        // Compute statistics and filter outliers
        double mean_dwell = 0, mean_flight = 0;
        for (const auto& f : features) {
            mean_dwell += f.x;
            mean_flight += f.y;
        }
        mean_dwell /= features.size();
        mean_flight /= features.size();

        double var_dwell = 0, var_flight = 0;
        for (const auto& f : features) {
            var_dwell += std::pow(f.x - mean_dwell, 2);
            var_flight += std::pow(f.y - mean_flight, 2);
        }
        var_dwell = features.size() > 1 ? var_dwell / (features.size() - 1) : 1e-6;
        var_flight = features.size() > 1 ? var_flight / (features.size() - 1) : 1e-6;

        if (var_dwell < 1e-6 || var_flight < 1e-6) {
            wxLogError("Insufficient feature variance for %s: var_dwell=%f, var_flight=%f", username.c_str(), var_dwell, var_flight);
            continue;
        }

        wxLogMessage("User %s: mean_dwell=%f, var_dwell=%f, mean_flight=%f, var_flight=%f",
                     username.c_str(), mean_dwell, var_dwell, mean_flight, var_flight);

        std::vector<Vec2> filtered_features;
        double std_dwell = std::sqrt(var_dwell);
        double std_flight = std::sqrt(var_flight);
        for (const auto& f : features) {
            if (std::abs(f.x - mean_dwell) <= 3 * std_dwell &&
                std::abs(f.y - mean_flight) <= 3 * std_flight &&
                std::isfinite(f.x) && std::isfinite(f.y)) {
                filtered_features.push_back(f);
            }
        }

        wxLogMessage("User %s: Filtered features=%zu", username.c_str(), filtered_features.size());

        if (filtered_features.size() < 50) {
            wxLogError("Not enough valid features after filtering (%zu < 50) for user %s", filtered_features.size(), username.c_str());
            continue;
        }

        // Normalize features with larger epsilon
        double min_dwell = filtered_features[0].x, max_dwell = filtered_features[0].x;
        double min_flight = filtered_features[0].y, max_flight = filtered_features[0].y;
        for (const auto& feature : filtered_features) {
            min_dwell = std::min(min_dwell, feature.x);
            max_dwell = std::max(max_dwell, feature.x);
            min_flight = std::min(min_flight, feature.y);
            max_flight = std::max(max_flight, feature.y);
        }
        double dwell_range = max_dwell - min_dwell + 1e-4;
        double flight_range = max_flight - min_flight + 1e-4;

        std::vector<Vec2> normalized_features = filtered_features;
        for (auto& feature : normalized_features) {
            feature.x = (feature.x - min_dwell) / dwell_range;
            feature.y = (feature.y - min_flight) / flight_range;
        }

        // Log features for 1tWFaGkY
        if (username == "1tWFaGkY") {
            std::ofstream feat_out("resources\\1tWFaGkY_features.csv");
            for (const auto& f : normalized_features) {
                feat_out << f.x << "," << f.y << "\n";
            }
            feat_out.close();
            wxLogMessage("Dumped features for 1tWFaGkY to resources\\1tWFaGkY_features.csv");
        }

        // Try training GMM with retries
        bool trained = false;
        for (int attempt = 0; attempt < 5 && !trained; ++attempt) {
            GMM gmm(3);
            try {
                // Add small jitter to avoid numerical instability
                std::random_device rd;
                std::mt19937 gen(rd());
                std::normal_distribution<double> jitter(0.0, 1e-4);
                std::vector<Vec2> jittered_features = normalized_features;
                for (auto& f : jittered_features) {
                    f.x = std::max(0.0, std::min(1.0, f.x + jitter(gen)));
                    f.y = std::max(0.0, std::min(1.0, f.y + jitter(gen)));
                }

                gmm.train(jittered_features, 100, 1e-6);
                double ll = gmm.logLikelihood(jittered_features);
                if (std::isfinite(ll) && ll > -1e10) {
                    wxLogMessage("Trained GMM for %s (attempt %d), log likelihood: %f", username.c_str(), attempt + 1, ll);
                    std::string gmmFile = "resources\\" + username + ".gmm";
                    if (!fs::exists("resources")) {
                        fs::create_directory("resources");
                    }
                    gmm.save(gmmFile);
                    wxLogMessage("Saved GMM to %s", gmmFile.c_str());
                    successful_gmms++;
                    trained = true;
                } else {
                    wxLogWarning("Attempt %d: Invalid log likelihood for %s: %f", attempt + 1, username.c_str(), ll);
                }
            } catch (const std::exception& e) {
                wxLogWarning("Attempt %d: GMM training failed for %s: %s", attempt + 1, username.c_str(), e.what());
            }
        }

        if (!trained) {
            wxLogError("Failed to train GMM for %s after 5 attempts", username.c_str());
        }
    }

    wxLogMessage("Completed GMM training: %zu/%zu GMMs generated", successful_gmms, participantToName.size());
}

void HomePage::EvaluateGMMsForParticipants(const std::string& dataCSV, const std::string& userCSV) {
    namespace fs = std::filesystem;

    std::ifstream userFile;
    int retries = 5;
    while (retries-- > 0) {
        userFile.open(fs::u8path(userCSV));
        if (userFile.is_open()) break;
        wxLogWarning("Retrying to open user CSV: %s (%d retries left)", userCSV.c_str(), retries);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    if (!userFile.is_open()) {
        wxLogError("Could not open user CSV: %s", userCSV.c_str());
        return;
    }

    std::map<unsigned long long, std::string> participantToName;
    std::string line;
    std::getline(userFile, line); // Skip header
    while (std::getline(userFile, line)) {
        std::stringstream ss(line);
        std::string pid_str, name, sentences;
        if (std::getline(ss, pid_str, ',') && std::getline(ss, name, ',')) {
            name.erase(0, name.find_first_not_of(" \t\r\n\""));
            name.erase(name.find_last_not_of(" \t\r\n\"") + 1);
            try {
                unsigned long long participant_id = std::stoull(pid_str);
                participantToName[participant_id] = name;
            } catch (const std::exception& e) {
                wxLogError("Invalid PARTICIPANT_ID in %s: %s (%s)", userCSV.c_str(), line.c_str(), e.what());
            }
        }
    }
    userFile.close();

    std::ifstream dataFile;
    retries = 5;
    while (retries-- > 0) {
        dataFile.open(fs::u8path(dataCSV));
        if (dataFile.is_open()) break;
        wxLogWarning("Retrying to open prepared CSV: %s (%d retries left)", dataCSV.c_str(), retries);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    if (!dataFile.is_open()) {
        wxLogError("Could not open prepared CSV: %s", dataCSV.c_str());
        return;
    }

    std::map<unsigned long long, std::vector<Vec2>> participantFeatures;
    std::map<unsigned long long, std::pair<double, double>> normalizationRanges;
    size_t invalid_lines = 0;
    std::getline(dataFile, line); // Skip header
    while (std::getline(dataFile, line)) {
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 8) {
            wxLogWarning("Malformed line in %s: %s", dataCSV.c_str(), line.c_str());
            invalid_lines++;
            continue;
        }

        try {
            unsigned long long pid = std::stoull(tokens[0]);
            double norm_dwell = std::stod(tokens[4]);
            double norm_flight = std::stod(tokens[5]);
            double max_dwell = std::stod(tokens[6]);
            double max_flight = std::stod(tokens[7]);
            if (!std::isnan(norm_dwell) && !std::isinf(norm_dwell) &&
                !std::isnan(norm_flight) && !std::isinf(norm_flight)) {
                participantFeatures[pid].emplace_back(norm_dwell, norm_flight);
                normalizationRanges[pid] = {max_dwell, max_flight};
            }
        } catch (const std::exception& e) {
            wxLogWarning("Error parsing line in %s: %s (%s)", dataCSV.c_str(), line.c_str(), e.what());
            invalid_lines++;
        }
    }
    dataFile.close();
    wxLogMessage("Loaded %s for evaluation: %zu participants, %zu invalid lines", dataCSV.c_str(), participantFeatures.size(), invalid_lines);

    struct Metrics {
        double Far;
        double frr;
        double eer;
        double success_rate;
        int own_trials;
        int other_trials;
        std::vector<std::pair<double, double>> roc_points; // (FPR, TPR)
        Metrics() : Far(0.0), frr(0.0), eer(0.0), success_rate(0.0), own_trials(0), other_trials(0) {}
    };
    std::map<std::string, Metrics> user_metrics;

    for (const auto& [pid, username] : participantToName) {
        std::string gmm_file = "resources\\" + username + ".gmm";
        if (!fs::exists(fs::u8path(gmm_file))) {
            wxLogWarning("GMM file for %s not found", username.c_str());
            continue;
        }

        GMM gmm(2);
        try {
            gmm.load(gmm_file);
        } catch (const std::exception& e) {
            wxLogError("Failed to load GMM for %s: %s", username.c_str(), e.what());
            continue;
        }

        Metrics metrics;
        auto it = participantFeatures.find(pid);
        if (it == participantFeatures.end()) {
            wxLogWarning("No features found for user %s (PARTICIPANT_ID %llu)", username.c_str(), pid);
            continue;
        }

        auto own_features = it->second;
        auto norm_ranges = normalizationRanges[pid];
        double max_dwell = norm_ranges.first;
        double max_flight = norm_ranges.second;
        if (own_features.size() < 10) {
            wxLogWarning("Insufficient features for user %s: %zu < 10", username.c_str(), own_features.size());
            continue;
        }

        std::vector<Vec2> normalized_features = own_features;
        for (auto& feature : normalized_features) {
            feature.x = max_dwell ? feature.x * max_dwell / (max_dwell + 1e-4) : feature.x;
            feature.y = max_flight ? feature.y * max_flight / (max_flight + 1e-4) : feature.y;
        }

        size_t subset_size = std::max(size_t(1), own_features.size() / 10);
        size_t num_subsets = own_features.size() / subset_size;
        std::vector<double> own_log_likelihoods;
        for (size_t i = 0; i < num_subsets; ++i) {
            size_t start = i * subset_size;
            size_t end = std::min((i + 1) * subset_size, own_features.size());
            std::vector<Vec2> subset(normalized_features.begin() + start, normalized_features.begin() + end);
            double ll = gmm.logLikelihood(subset);
            if (std::isfinite(ll)) {
                own_log_likelihoods.push_back(ll);
            } else {
                wxLogWarning("Invalid log-likelihood for user %s: %f", username.c_str(), ll);
            }
        }

        std::vector<double> other_log_likelihoods;
        for (const auto& [other_pid, other_username] : participantToName) {
            if (other_username != username) {
                auto other_it = participantFeatures.find(other_pid);
                if (other_it == participantFeatures.end() || other_it->second.size() < 10) continue;

                auto other_features = other_it->second;
                auto other_norm_ranges = normalizationRanges[other_pid];
                double other_max_dwell = other_norm_ranges.first;
                double other_max_flight = other_norm_ranges.second;
                std::vector<Vec2> normalized_other_features = other_features;
                for (auto& feature : normalized_other_features) {
                    feature.x = other_max_dwell ? feature.x * other_max_dwell / (other_max_dwell + 1e-4) : feature.x;
                    feature.y = other_max_flight ? feature.y * other_max_flight / (other_max_flight + 1e-4) : feature.y;
                }

                double ll = gmm.logLikelihood(normalized_other_features);
                if (std::isfinite(ll)) {
                    other_log_likelihoods.push_back(ll);
                } else {
                    wxLogWarning("Invalid log-likelihood for user %s (other user %s): %f", username.c_str(), other_username.c_str(), ll);
                }
            }
        }

        if (own_log_likelihoods.empty() || other_log_likelihoods.empty()) {
            wxLogWarning("No valid log-likelihoods for user %s: own=%zu, other=%zu", username.c_str(), own_log_likelihoods.size(), other_log_likelihoods.size());
            continue;
        }

        // Combine all log-likelihoods to determine threshold range
        std::vector<double> all_log_likelihoods = own_log_likelihoods;
        all_log_likelihoods.insert(all_log_likelihoods.end(), other_log_likelihoods.begin(), other_log_likelihoods.end());
        std::sort(all_log_likelihoods.begin(), all_log_likelihoods.end());
        double min_ll = all_log_likelihoods.front();
        double max_ll = all_log_likelihoods.back();
        wxLogMessage("User %s: Log-likelihood range: min=%f, max=%f", username.c_str(), min_ll, max_ll);

        // Use actual log-likelihood values as thresholds to capture all transitions
        std::vector<double> thresholds = all_log_likelihoods;
        // Ensure uniqueness
        thresholds.erase(std::unique(thresholds.begin(), thresholds.end(), 
                                     [](double a, double b) { return std::abs(a - b) < 1e-6; }), 
                         thresholds.end());

        // Compute ROC curve
        for (double threshold : thresholds) {
            double tpr = 0.0, fpr = 0.0;
            int tpr_count = 0, fpr_count = 0;

            for (double ll : own_log_likelihoods) {
                if (ll >= threshold) tpr += 1.0;
                tpr_count++;
            }

            for (double ll : other_log_likelihoods) {
                if (ll >= threshold) fpr += 1.0;
                fpr_count++;
            }

            tpr = tpr_count > 0 ? tpr / tpr_count : 0.0;
            fpr = fpr_count > 0 ? fpr / fpr_count : 0.0;
            if (std::isfinite(tpr) && std::isfinite(fpr)) {
                metrics.roc_points.emplace_back(fpr, tpr);
                wxLogMessage("User %s: Threshold=%f, FPR=%f, TPR=%f", username.c_str(), threshold, fpr, tpr);
            }
        }

        // Sort ROC points by FPR and remove duplicates
        std::sort(metrics.roc_points.begin(), metrics.roc_points.end());
        metrics.roc_points.erase(
            std::unique(metrics.roc_points.begin(), metrics.roc_points.end(),
                        [](const auto& a, const auto& b) {
                            return std::abs(a.first - b.first) < 1e-6 && std::abs(a.second - b.second) < 1e-6;
                        }),
            metrics.roc_points.end()
        );

        // Ensure (0,0) and (1,1) are included
        if (metrics.roc_points.empty() || metrics.roc_points.front() != std::make_pair(0.0, 0.0)) {
            metrics.roc_points.insert(metrics.roc_points.begin(), {0.0, 0.0});
        }
        if (metrics.roc_points.back() != std::make_pair(1.0, 1.0)) {
            metrics.roc_points.push_back({1.0, 1.0});
        }

        // Compute baseline metrics
        double mean_ll = std::accumulate(own_log_likelihoods.begin(), own_log_likelihoods.end(), 0.0) / own_log_likelihoods.size();
        double var_ll = 0.0;
        for (double ll : own_log_likelihoods) {
            var_ll += std::pow(ll - mean_ll, 2);
        }
        var_ll /= own_log_likelihoods.size();
        double std_ll = std::sqrt(var_ll);
        double baseline_threshold = mean_ll - 0.5 * std_ll;
        baseline_threshold = std::max(baseline_threshold, min_ll);

        for (double ll : own_log_likelihoods) {
            bool rejected = ll < baseline_threshold;
            metrics.frr += rejected ? 1.0 : 0.0;
            metrics.success_rate += rejected ? 0.0 : 1.0;
            metrics.own_trials++;
            wxLogMessage("User %s: Own trial, log_likelihood=%f, threshold=%f, rejected=%d", 
                         username.c_str(), ll, baseline_threshold, rejected);
        }

        for (double ll : other_log_likelihoods) {
            bool accepted = ll >= baseline_threshold;
            metrics.Far += accepted ? 1.0 : 0.0;
            metrics.other_trials++;
            wxLogMessage("User %s: Other trial, log_likelihood=%f, threshold=%f, accepted=%d", 
                         username.c_str(), ll, baseline_threshold, accepted);
        }

        if (metrics.own_trials > 0 || metrics.other_trials > 0) {
            metrics.frr = metrics.own_trials > 0 ? metrics.frr / metrics.own_trials : 1.0;
            metrics.success_rate = metrics.own_trials > 0 ? metrics.success_rate / metrics.own_trials : 0.0;
            metrics.Far = metrics.other_trials > 0 ? metrics.Far / metrics.other_trials : 0.0;
            metrics.eer = (metrics.Far + metrics.frr) / 2.0;
            user_metrics[username] = metrics;
            wxLogMessage("Metrics for %s: FAR=%f, FRR=%f, EER=%f, SuccessRate=%f, OwnTrials=%d, OtherTrials=%d, ROCPoints=%zu",
                         username.c_str(), metrics.Far, metrics.frr, metrics.eer, metrics.success_rate, 
                         metrics.own_trials, metrics.other_trials, metrics.roc_points.size());
        } else {
            wxLogWarning("No valid trials for user %s", username.c_str());
        }
    }

    std::ofstream csv_file("resources\\evaluation.csv", std::ios::trunc);
    if (!csv_file.is_open()) {
        wxLogError("Failed to open evaluation.csv");
        return;
    }
    csv_file << "username,FAR,FRR,EER,SuccessRate,ROC\n";
    for (const auto& [username, metrics] : user_metrics) {
        std::stringstream roc_ss;
        roc_ss << "[";
        for (size_t i = 0; i < metrics.roc_points.size(); ++i) {
            roc_ss << "(" << metrics.roc_points[i].first << "," << metrics.roc_points[i].second << ")";
            if (i < metrics.roc_points.size() - 1) roc_ss << ",";
        }
        roc_ss << "]";
        std::string roc_str = roc_ss.str();

        csv_file << username << "," << metrics.Far << "," << metrics.frr << "," 
                 << metrics.eer << "," << metrics.success_rate << "," 
                 << EscapeCSVField(roc_str) << "\n";
    }
    csv_file.close();
    wxLogMessage("Evaluation completed, results written to resources\\evaluation.csv");
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