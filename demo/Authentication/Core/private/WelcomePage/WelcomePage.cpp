#include "WelcomePage/WelcomePage.hpp"

WelcomePage::WelcomePage(wxWindow* parent, wxWindowID id, const wxString& title, 
                         const std::vector<Vec2>& keystrokes, double likelihood, 
                         const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style), keystrokes(keystrokes), loginLikelihood(likelihood) {
    this->SetSizeHints(wxSize(1200, 800), wxSize(1200, 800));
    this->SetPosition(wxPoint(1200-600, 800-400));
    this->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    this->SetBackgroundColour(wxColour(245, 245, 255)); // Softer background

    wxBoxSizer* Area = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* Context = new wxBoxSizer(wxVERTICAL);

    // Header
    HelpingText = new wxStaticText(this, wxID_ANY, _("Greetings!!"), 
                                   wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    HelpingText->SetFont(wxFont(28, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, 
                               wxFONTWEIGHT_BOLD, false, wxT("Arial")));
    HelpingText->SetForegroundColour(wxColour(0, 102, 204)); // Blue accent
    Context->Add(HelpingText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);

    // Stats Panel
    wxPanel* statsPanel = new wxPanel(this, wxID_ANY);
    statsPanel->SetBackgroundColour(wxColour(255, 255, 255));
    wxStaticBoxSizer* statsBox = new wxStaticBoxSizer(wxVERTICAL, statsPanel, _("Keystroke Statistics"));
    statsBox->GetStaticBox()->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                                            wxFONTWEIGHT_NORMAL, false, wxT("Georgia")));

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

    wxGridSizer* statsGrid = new wxGridSizer(6, 2, 10, 20); // 6 rows, 2 cols
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, _("Keystrokes:"), 
									wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%zu"), numKeystrokes)), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, _("Likelihood:"), 
									wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f"), loginLikelihood)), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, _("Avg Dwell:"), 
									wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f ms"), avgDwell)), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, _("Avg Flight:"), 
									wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f ms"), avgFlight)), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, _("Max Dwell:"), 
									wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f ms"), maxDwell)), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, _("Max Flight:"), 
									wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), 
				0, wxALIGN_CENTER_VERTICAL);
	statsGrid->Add(new wxStaticText(statsPanel, wxID_ANY, wxString::Format(_("%.2f ms"), maxFlight)), 
				0, wxALIGN_CENTER_VERTICAL);

    statsBox->Add(statsGrid, 1, wxEXPAND | wxALL, 10);
    statsPanel->SetSizer(statsBox);
    Context->Add(statsPanel, 0, wxEXPAND | wxALL, 15);

    // Chart
    chartWindow = new mpWindow(this, wxID_ANY, wxDefaultPosition, wxSize(800, 500)); // Larger chart
    chartWindow->SetMargins(60, 60, 60, 60);
    
    Context->Add(chartWindow, 1, wxEXPAND | wxALL, 15);

    Area->Add(Context, 1, wxEXPAND | wxALL, 30);

    this->SetSizer(Area);
    this->Layout();
    this->Centre(wxBOTH);

    CreateChart();
}

WelcomePage::~WelcomePage() {}

void WelcomePage::CreateChart() {
    if (keystrokes.empty()) return;

    std::vector<double> dwellTimes, flightTimes, xValues;
    for (size_t i = 0; i < keystrokes.size(); ++i) {
        xValues.push_back(static_cast<double>(i));
        dwellTimes.push_back(keystrokes[i].x);
        flightTimes.push_back(keystrokes[i].y);
    }

    mpFXYVector* dwellLayer = new mpFXYVector(_("Dwell Time"), mpALIGN_CENTER);
    dwellLayer->SetData(xValues, dwellTimes);
    dwellLayer->SetPen(wxPen(*wxBLUE, 3, wxPENSTYLE_SOLID));
    dwellLayer->SetContinuity(true);
    dwellLayer->ShowName(true); // Show in legend

    mpFXYVector* flightLayer = new mpFXYVector(_("Flight Time"), mpALIGN_CENTER);
    flightLayer->SetData(xValues, flightTimes);
    flightLayer->SetPen(wxPen(*wxRED, 3, wxPENSTYLE_SOLID));
    flightLayer->SetContinuity(true);
    flightLayer->ShowName(true); // Show in legend

    mpScaleX* xAxis = new mpScaleX(_("Keystroke Index"), mpALIGN_BOTTOM, true);
    xAxis->SetPen(wxPen(*wxBLACK, 1));
    xAxis->SetTicks(true);

    mpScaleY* yAxis = new mpScaleY(_("Time (ms)"), mpALIGN_LEFT, true);
    yAxis->SetPen(wxPen(*wxBLACK, 1));
    yAxis->SetTicks(true);

    chartWindow->AddLayer(xAxis);
    chartWindow->AddLayer(yAxis);
    chartWindow->AddLayer(dwellLayer);
    chartWindow->AddLayer(flightLayer);
    chartWindow->AddLayer(new mpInfoLegend(wxRect(10, 10, 150, 50), wxWHITE_BRUSH)); // Legend

    chartWindow->Fit();
    chartWindow->UpdateAll();
}