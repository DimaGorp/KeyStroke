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
class HomePage : public wxFrame
{

	protected:
		wxStaticText* Welcome_Text;
		wxStaticText* Description;
		wxButton* LogIN_BTN;
		wxButton* SignUpBtn;

	public:

		HomePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 672,458 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		void OnLoginEnter(wxMouseEvent& event);
		void OnLoginLeave(wxMouseEvent& event);
		void OnSignUpEnter(wxMouseEvent& event);
		void OnSignUpLeave(wxMouseEvent& event);
		void OnLoginClick(wxCommandEvent& event);
		void OnSignUpClick(wxCommandEvent& event);
		void HomePage::GenerateCSVWithRandomNames(const std::string& directoryPath, const std::string& outputCSV) ;
		~HomePage();
	private:
    	std::string generateRandomName(int length); 
		

};