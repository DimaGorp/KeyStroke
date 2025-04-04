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

#include <wx/wx.h>
#include <vector>
#include "GMM/GMM.hpp" // For Vec2 definition (assuming Vec2 is in GMM.hpp)
#include "mathplot.h" // wxMathPlot header

class WelcomePage : public wxFrame {
	public:
		WelcomePage(wxWindow* parent, wxWindowID id, const wxString& title, 
					const std::vector<Vec2>& keystrokes = std::vector<Vec2>(), 
					double likelihood = 0.0, 
					const wxPoint& pos = wxDefaultPosition, 
					const wxSize& size = wxDefaultSize, 
					long style = wxDEFAULT_FRAME_STYLE);
		~WelcomePage();
	
	private:
		wxStaticText* HelpingText;
		wxStaticText* KeystrokeInfo;
		mpWindow* chartWindow;
		std::vector<Vec2> keystrokes;
		double loginLikelihood;
	
		void CreateChart();
};
