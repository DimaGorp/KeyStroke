#include "WelcomePage/WelcomePage.hpp"
#include <wx/log.h>
#include <wx/scrolwin.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "mathplot.h"

WelcomePage::WelcomePage(wxWindow* parent, wxWindowID id, const wxString& title, 
                         const std::string& username, const std::vector<Vec2>& keystrokes, 
                         double likelihood, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style), 
      username(username), keystrokes(keystrokes), loginLikelihood(likelihood),
      Far(0.0), frr(0.0), eer(0.0), successRate(0.0), metricsLoaded(false),
      rocWindow(nullptr) {
    wxLog::SetActiveTarget(new wxLogStderr());
    wxLogMessage("WelcomePage initialized for user: %s", username.c_str());

    // Fix window size and position, prevent resizing
    this->SetSizeHints(wxSize(1200, 800), wxSize(1200, 800));
    this->SetPosition(wxPoint(1200-600, 800-400));
    this->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    this->SetBackgroundColour(wxColour(245, 245, 255)); // Softer background

    wxBoxSizer* Area = new wxBoxSizer(wxVERTICAL);

    // Use a panel instead of scrolled window to prevent scrolling
    wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(1200, 800));
    mainPanel->SetBackgroundColour(wxColour(245, 245, 255));

    wxBoxSizer* Context = new wxBoxSizer(wxVERTICAL);

    // Header
    HelpingText = new wxStaticText(mainPanel, wxID_ANY, wxString::Format(_("Greetings, %s!"), username.c_str()), 
                                   wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL|wxST_NO_AUTORESIZE);
    HelpingText->Wrap(300);
    HelpingText->SetFont(wxFont(22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));
    Context->Add(HelpingText, 0, wxALL|wxEXPAND, 15);

    // Stats Panel (Keystroke Statistics)
    wxPanel* statsPanel = new wxPanel(mainPanel, wxID_ANY);
    statsPanel->SetBackgroundColour(wxColour(255, 255, 255));
    wxStaticBoxSizer* sbSizer1 = new wxStaticBoxSizer(wxVERTICAL, statsPanel, _("Authentication Metrics"));
    sbSizer1->GetStaticBox()->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                                            wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));

    // Load evaluation metrics
    metricsLoaded = LoadEvaluationMetrics("./resources/evaluation.csv", username);
    if (!metricsLoaded) {
        wxLogWarning("No evaluation metrics found for user: %s", username.c_str());
    }

    size_t numKeystrokes = keystrokes.size();
    double avgDwell = 0.0, avgFlight = 0.0, maxDwell = 0.0, maxFlight = 0.0;
    if (numKeystrokes > 0) {
        for (const auto& feature : keystrokes) {
            avgDwell += feature.x;
            avgFlight += feature.y;
            maxDwell = std::max(maxDwell, feature.x);
            maxFlight = std::max(maxFlight, std::abs(feature.y));
        }
        avgDwell /= numKeystrokes;
        avgFlight /= numKeystrokes;
    }

    wxGridSizer* KeystrokeStatistics = new wxGridSizer(6, 2, 10, 20);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, _("Keystrokes:"), 
                                             wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%zu"), numKeystrokes)), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, _("Likelihood:"), 
                                             wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f"), loginLikelihood)), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, _("Avg Dwell:"), 
                                             wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f ms"), avgDwell)), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, _("Avg Flight:"), 
                                             wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f ms"), avgFlight)), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, _("Max Dwell:"), 
                                             wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f ms"), maxDwell)), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, _("Max Flight:"), 
                                             wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
                             0, wxALIGN_CENTER_VERTICAL);
    KeystrokeStatistics->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f ms"), maxFlight)), 
                             0, wxALIGN_CENTER_VERTICAL);

    sbSizer1->Add(KeystrokeStatistics, 1, wxEXPAND | wxALL, 10);
    statsPanel->SetSizer(sbSizer1);
    Context->Add(statsPanel, 0, wxEXPAND | wxALL, 15);

    // GMM Metrics
    wxStaticBoxSizer* sbSizer2 = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, _("GMM Metrics")), wxVERTICAL);
    wxGridSizer* gSizer2 = new wxGridSizer(2, 2, 10, 20);

    // FAR
    wxBoxSizer* FARSizer = new wxBoxSizer(wxVERTICAL);
    FARText = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, _("FAR (False Acceptance Rate)"), 
                               wxDefaultPosition, wxDefaultSize, 0);
    FARText->Wrap(-1);
    FARText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString));
    FARText->SetForegroundColour(wxColour(255, 255, 255));
    FARText->SetBackgroundColour(wxColour(255, 0, 0));
    FARSizer->Add(FARText, 0, wxALIGN_CENTER|wxALL, 5);
    FARValue = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, 
                                metricsLoaded ? wxString::Format(_("%.2f"), Far) : _("N/A"), 
                                wxDefaultPosition, wxDefaultSize, 0);
    FARValue->Wrap(-1);
    FARValue->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    FARSizer->Add(FARValue, 0, wxALIGN_CENTER|wxALL, 5);
    gSizer2->Add(FARSizer, 1, wxEXPAND, 5);

    // FRR
    wxBoxSizer* FRRSizer = new wxBoxSizer(wxVERTICAL);
    FRR = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, _("FRR (False Rejection Rate)"), 
                           wxDefaultPosition, wxDefaultSize, 0);
    FRR->Wrap(-1);
    FRR->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    FRR->SetBackgroundColour(wxColour(0, 0, 255));
    FRRSizer->Add(FRR, 0, wxALIGN_CENTER|wxALL, 5);
    FRRValue = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, 
                                metricsLoaded ? wxString::Format(_("%.2f"), frr) : _("N/A"), 
                                wxDefaultPosition, wxDefaultSize, 0);
    FRRValue->Wrap(-1);
    FRRValue->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    FRRSizer->Add(FRRValue, 0, wxALIGN_CENTER|wxALL, 5);
    gSizer2->Add(FRRSizer, 1, wxEXPAND, 5);

    // EER
    wxBoxSizer* ERRSizer = new wxBoxSizer(wxVERTICAL);
    EER = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, _("EER (Equal Error Rate)"), 
                           wxDefaultPosition, wxDefaultSize, 0);
    EER->Wrap(-1);
    EER->SetForegroundColour(wxColour(0, 0, 0));
    EER->SetBackgroundColour(wxColour(0, 255, 128));
    ERRSizer->Add(EER, 0, wxALIGN_CENTER|wxALL, 5);
    EERValue = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, 
                                metricsLoaded ? wxString::Format(_("%.2f"), eer) : _("N/A"), 
                                wxDefaultPosition, wxDefaultSize, 0);
    EERValue->Wrap(-1);
    EERValue->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    ERRSizer->Add(EERValue, 0, wxALIGN_CENTER|wxALL, 5);
    gSizer2->Add(ERRSizer, 1, wxEXPAND, 5);

    // Success Rate
    wxBoxSizer* SRSizer = new wxBoxSizer(wxVERTICAL);
    SR = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, _("Success Rate"), 
                          wxDefaultPosition, wxDefaultSize, 0);
    SR->Wrap(-1);
    SR->SetForegroundColour(wxColour(0, 0, 0));
    SR->SetBackgroundColour(wxColour(255, 128, 255));
    SRSizer->Add(SR, 0, wxALIGN_CENTER|wxALL, 5);
    SRValue = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, 
                               metricsLoaded ? wxString::Format(_("%.2f"), successRate) : _("N/A"), 
                               wxDefaultPosition, wxDefaultSize, 0);
    SRValue->Wrap(-1);
    SRValue->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    SRSizer->Add(SRValue, 0, wxALIGN_CENTER|wxALL, 5);
    gSizer2->Add(SRSizer, 1, wxEXPAND, 5);

    sbSizer2->Add(gSizer2, 1, wxEXPAND, 5);
    Context->Add(sbSizer2, 0, wxEXPAND | wxALL, 15);

    // ROC Curve (Fixed panel, no scrolling)
    wxStaticBoxSizer* rocBox = new wxStaticBoxSizer(wxVERTICAL, mainPanel, _("ROC Curve"));
    wxPanel* rocPanel = new wxPanel(rocBox->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(600, 400));
    rocPanel->SetMinSize(wxSize(600, 400));
    rocWindow = new mpWindow(rocPanel, wxID_ANY, wxPoint(0, 0), wxSize(600, 400));
    rocWindow->SetMargins(60, 60, 60, 60);
    rocBox->Add(rocPanel, 0, wxALIGN_CENTER | wxALL, 5);
    Context->Add(rocBox, 0, wxALIGN_CENTER | wxALL, 15);

    mainPanel->SetSizer(Context);
    mainPanel->Layout();

    Area->Add(mainPanel, 1, wxEXPAND | wxALL, 5);
    this->SetSizer(Area);
    this->Layout();
    this->Centre(wxBOTH);

    CreateROCCurve();
}

WelcomePage::~WelcomePage() {}

bool WelcomePage::LoadEvaluationMetrics(const std::string& filename, const std::string& username) {
    namespace fs = std::filesystem;
    fs::path filePath = fs::u8path(filename);
    if (!fs::exists(filePath)) {
        wxLogError("Evaluation metrics file not found: %s", filename.c_str());
        return false;
    }

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        wxLogError("Cannot open evaluation metrics file: %s", filename.c_str());
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header
    wxLogMessage("Evaluation metrics CSV header: %s", line.c_str());

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        wxLogMessage("Parsing evaluation metrics CSV line: %s", line.c_str());
        std::stringstream ss(line);
        std::string csvUsername, farStr, frrStr, eerStr, successRateStr, rocStr;

        // Parse username
        if (!std::getline(ss, csvUsername, ',')) {
            wxLogWarning("Invalid CSV line (missing username): %s", line.c_str());
            continue;
        }
        csvUsername.erase(0, csvUsername.find_first_not_of(" \t\r\n\""));
        csvUsername.erase(csvUsername.find_last_not_of(" \t\r\n\"") + 1);

        if (csvUsername != username) {
            continue; // Skip other users
        }

        // Parse FAR
        if (!std::getline(ss, farStr, ',')) {
            wxLogWarning("Invalid CSV line (missing FAR): %s", line.c_str());
            continue;
        }
        // Parse FRR
        if (!std::getline(ss, frrStr, ',')) {
            wxLogWarning("Invalid CSV line (missing FRR): %s", line.c_str());
            continue;
        }
        // Parse EER
        if (!std::getline(ss, eerStr, ',')) {
            wxLogWarning("Invalid CSV line (missing EER): %s", line.c_str());
            continue;
        }
        // Parse SuccessRate
        if (!std::getline(ss, successRateStr, ',')) {
            wxLogWarning("Invalid CSV line (missing SuccessRate): %s", line.c_str());
            continue;
        }
        // Parse ROC data (optional)
        std::getline(ss, rocStr);
        rocStr.erase(0, rocStr.find_first_not_of(" \t\r\n\""));
        rocStr.erase(rocStr.find_last_not_of(" \t\r\n\"") + 1);

        try {
            Far = std::stod(farStr);
            frr = std::stod(frrStr);
            eer = std::stod(eerStr);
            successRate = std::stod(successRateStr);

            // Parse ROC data if available
            rocPoints.clear();
            if (!rocStr.empty() && rocStr.front() == '[' && rocStr.back() == ']') {
                rocStr = rocStr.substr(1, rocStr.size() - 2); // Remove []
                std::stringstream rocSS(rocStr);
                std::string point;
                while (std::getline(rocSS, point, ')')) {
                    if (point.empty()) continue;
                    if (point.front() == ',') point = point.substr(1);
                    if (point.front() == '(') point = point.substr(1);
                    std::stringstream pointSS(point);
                    double fpr, tpr;
                    char comma;
                    pointSS >> fpr >> comma >> tpr;
                    if (pointSS.fail()) continue;
                    rocPoints.emplace_back(fpr, tpr);
                    wxLogMessage("Loaded ROC point for %s: FPR=%.2f, TPR=%.2f", username.c_str(), fpr, tpr);
                }
            }

            // If no ROC data, generate synthetic points
            if (rocPoints.empty()) {
                double tpr = 1.0 - frr;
                double fpr = Far;
                rocPoints = {
                    {0.00, 0.00}, // Start
                    {fpr * 0.50, tpr * 0.50}, // Midpoint (interpolated)
                    {fpr, tpr}, // Provided FAR, FRR
                    {fpr + (1.0 - fpr) * 0.50, tpr + (1.0 - tpr) * 0.50}, // Beyond
                    {1.00, 1.00} // End
                };
                wxLogMessage("Generated synthetic ROC points for %s", username.c_str());
            }

            wxLogMessage("Loaded metrics for %s: FAR=%.2f, FRR=%.2f, EER=%.2f, SuccessRate=%.2f, ROC points=%zu",
                         username.c_str(), Far, frr, eer, successRate, rocPoints.size());
            file.close();
            return true;
        } catch (const std::exception& e) {
            wxLogWarning("Error parsing metrics for %s: %s (%s)", username.c_str(), line.c_str(), e.what());
            continue;
        }
    }

    file.close();
    wxLogWarning("No metrics found for user: %s in %s", username.c_str(), filename.c_str());
    return false;
}

void WelcomePage::CreateROCCurve() {
    if (!metricsLoaded || rocPoints.empty()) {
        wxLogWarning("No ROC points to plot for %s", username.c_str());
        return;
    }

    // Log initial points
    wxLogMessage("Initial ROC points for %s: %zu", username.c_str(), rocPoints.size());
    for (const auto& point : rocPoints) {
        wxLogMessage("Initial point: FPR=%.2f, TPR=%.2f", point.first, point.second);
    }

    // Interpolate ROC points for smoother curve
    std::vector<std::pair<double, double>> interpolatedPoints;
    for (size_t i = 0; i < rocPoints.size() - 1; ++i) {
        auto [fpr1, tpr1] = rocPoints[i];
        auto [fpr2, tpr2] = rocPoints[i + 1];
        for (int j = 0; j <= 20; ++j) { // Reduced to 20 steps for testing
            double t = j / 20.0;
            double fpr = fpr1 + t * (fpr2 - fpr1);
            double tpr = tpr1 + t * (tpr2 - tpr1);
            fpr = std::round(fpr * 100) / 100.0;
            tpr = std::round(tpr * 100) / 100.0;
            interpolatedPoints.emplace_back(fpr, tpr);
        }
    }
    // Add the last point explicitly
    interpolatedPoints.emplace_back(rocPoints.back().first, rocPoints.back().second);

    rocPoints = interpolatedPoints;
    wxLogMessage("Interpolated ROC points for %s: %zu", username.c_str(), rocPoints.size());

    // Ensure monotonicity
    std::sort(rocPoints.begin(), rocPoints.end());
    for (size_t i = 1; i < rocPoints.size(); ++i) {
        if (rocPoints[i].second < rocPoints[i - 1].second) {
            rocPoints[i].second = rocPoints[i - 1].second;
        }
    }

    // Remove duplicate points
    rocPoints.erase(
        std::unique(rocPoints.begin(), rocPoints.end(),
                    [](const auto& a, const auto& b) {
                        return std::abs(a.first - b.first) < 0.005 && std::abs(a.second - b.second) < 0.005;
                    }),
        rocPoints.end()
    );

    std::vector<double> fprValues, tprValues;
    for (const auto& point : rocPoints) {
        fprValues.push_back(point.first);
        tprValues.push_back(point.second);
    }

    mpScaleX* xAxis = new mpScaleX("False Positive Rate (FPR)", mpALIGN_CENTER, true);
    mpScaleY* yAxis = new mpScaleY("True Positive Rate (TPR)", mpALIGN_CENTER, true);
    xAxis->SetTicks(true); // Enable automatic ticks
    yAxis->SetTicks(true); // Enable automatic ticks

    rocWindow->AddLayer(xAxis);
    rocWindow->AddLayer(yAxis);

    mpFXYVector* rocLayer = new mpFXYVector("ROC Curve", mpALIGN_CENTER);
    rocLayer->SetData(fprValues, tprValues);
    rocLayer->SetContinuity(true);
    rocLayer->SetPen(wxPen(*wxBLUE, 2, wxPENSTYLE_SOLID));
    rocLayer->SetDrawOutsideMargins(false);
    rocWindow->AddLayer(rocLayer);

    std::vector<double> diagonalX = {0.00, 1.00};
    std::vector<double> diagonalY = {0.00, 1.00};
    mpFXYVector* diagonalLayer = new mpFXYVector("Reference", mpALIGN_CENTER);
    diagonalLayer->SetData(diagonalX, diagonalY);
    diagonalLayer->SetContinuity(true);
    diagonalLayer->SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
    diagonalLayer->SetDrawOutsideMargins(false);
    rocWindow->AddLayer(diagonalLayer);

    // Force tighter fit to reveal detail
    rocWindow->Fit(0.0, 1.0, 0.0, 1.0);
    rocWindow->UpdateAll();
    wxLogMessage("Created ROC curve for %s with %zu points", username.c_str(), rocPoints.size());
}
