#pragma once

///////////////////////////////////////////////////////////////////////////////
/// Class SignUp
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
#include <wx/richtext/richtextctrl.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
class SignUp : public wxFrame
{
	private:

	protected:
		wxStaticText* NameLabel;
		wxTextCtrl* m_textCtrl2;
		wxStaticText* HelpingText;
		wxStaticText* Text;
		wxRichTextCtrl* EnterArea;
		wxButton* LogIN_BTN;

	public:

		SignUp( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 886,638 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~SignUp();

};