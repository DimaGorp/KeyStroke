#pragma once
///////////////////////////////////////////////////////////////////////////////
/// Class WelcomePage
///////////////////////////////////////////////////////////////////////////////
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <vector>
#include "Core/GMM/GMM.hpp"
#include "mathplot.h" // wxMathPlot header
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/scrolwin.h>
#include <wx/frame.h>

class WelcomePage : public wxFrame
{
private:
    bool LoadEvaluationMetrics(const std::string& filename, const std::string& username);
    void CreateROCCurve();

    std::string username;
    std::vector<Vec2> keystrokes;
    double loginLikelihood;
    double Far, frr, eer, successRate; // Evaluation metrics
    bool metricsLoaded; // Flag to indicate if metrics were found
    std::vector<std::pair<double, double>> rocPoints; // ROC curve data (FPR, TPR)

protected:
    wxScrolledWindow* m_scrolledWindow1;
    wxStaticText* HelpingText;
    wxStaticText* FARText;
    wxStaticText* FARValue;
    wxStaticText* FRR;
    wxStaticText* FRRValue;
    wxStaticText* EER;
    wxStaticText* EERValue;
    wxStaticText* SR;
    wxStaticText* SRValue;
    mpWindow* rocWindow; // ROC curve plot

public:
    WelcomePage(wxWindow* parent, wxWindowID id, const wxString& title, 
                const std::string& username, const std::vector<Vec2>& keystrokes, 
                double likelihood, const wxPoint& pos = wxDefaultPosition, 
                const wxSize& size = wxSize(1200, 800), long style = wxDEFAULT_FRAME_STYLE);
    ~WelcomePage();
};