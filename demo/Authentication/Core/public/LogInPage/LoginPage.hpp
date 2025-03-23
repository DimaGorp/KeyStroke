#pragma once
///////////////////////////////////////////////////////////////////////////////
/// Class Login
///////////////////////////////////////////////////////////////////////////////
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/gdicmn.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
class Login : public wxFrame
{
	private:

	protected:
		wxButton* Back;
		wxStaticText* HelpingText;
		wxStaticText* Text;
		wxRichTextCtrl* EnterArea;
		wxButton* LogIN_BTN;

	public:

		Login( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 726,534 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~Login();
		void OnBackEnter(wxMouseEvent& event);
		void OnBackLeave(wxMouseEvent& event);
		void OnBackClicked(wxCommandEvent& event);
};