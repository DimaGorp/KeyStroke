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
#include <wx/log.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <vector>
#include <cmath>
#include <chrono>
// In SignupPage.hpp and LoginPage.hpp
#include "Core/GMM/GMM.hpp" // Instead of just "GMM.hpp"
class SignUp : public wxFrame
{
public:
    SignUp(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);
    ~SignUp();

private:
    wxStaticText* NameLabel;
    wxTextCtrl* m_textCtrl2;        // Name input
    wxStaticText* HelpingText;
    wxStaticText* Text;             // Text to type
    wxTextCtrl* EnterArea;          // Free-text input (changed from wxRichTextCtrl)

    struct KeyEvent {
        char key;
        std::chrono::steady_clock::time_point press_time;
        std::chrono::steady_clock::time_point release_time;
    };
    std::vector<KeyEvent> key_events;
    std::vector<wxString> textSamples; // List of texts to cycle through
    size_t currentTextIndex;           // Index of current text
    static const size_t MIN_SAMPLES = 20; // Minimum keystrokes for GMM

    void OnTextCtrlFocus(wxFocusEvent& event);
    void OnTextCtrlEnter(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnEnterPressed(wxCommandEvent& event); // Check text and handle signup
    void UpdateText();                          // Switch to next text

    DECLARE_EVENT_TABLE()
};