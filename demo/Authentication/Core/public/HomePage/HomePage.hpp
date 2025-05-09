#pragma once
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
#include <fstream>
#include <filesystem>
#include <sstream>
#include <random>
///////////////////////////////////////////////////////////////////////////////
/// Class HomePage
///////////////////////////////////////////////////////////////////////////////
class HomePage : public wxFrame {
	public:
		HomePage(wxWindow* parent, wxWindowID id, const wxString& title, 
				 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
				 long style = wxDEFAULT_FRAME_STYLE);
		~HomePage();
	
	private:
		wxStaticText* Welcome_Text;
		wxStaticText* Description;
		wxButton* LogIN_BTN;
		wxButton* SignUpBtn;
	
		void OnLoginEnter(wxMouseEvent& event);
		void OnLoginLeave(wxMouseEvent& event);
		void OnSignUpEnter(wxMouseEvent& event);
		void OnSignUpLeave(wxMouseEvent& event);
		void OnLoginClick(wxCommandEvent& event);
		void OnSignUpClick(wxCommandEvent& event);
	private:
		std::string generateRandomName(int length);
		std::string EscapeCSVField(const std::string& field) const;
		void GenerateCSVWithRandomNames(const std::string& directoryPath, const std::string& outputCSV);
		void TrainGMMsForParticipants(const std::string& dataDir, const std::string& userCSV) ;
	
		wxDECLARE_EVENT_TABLE();
	};