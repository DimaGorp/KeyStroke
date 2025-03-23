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
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/log.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <vector>
#include <chrono>
#include <filesystem>
#include "GMM/GMM.hpp"

class Login : public wxFrame
{
public:
    Login(wxWindow* parent, wxWindowID id, const wxString& title, 
          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
          long style = wxDEFAULT_FRAME_STYLE);
    ~Login();

private:
    wxButton* Back;
    wxStaticText* HelpingText;
    wxStaticText* Text;             // Text to type
    wxTextCtrl* EnterArea;          // Free-text input
    GMM storedGmm;                  // Loaded GMM for comparison
    wxString gmmFilename;           // Store selected GMM filename

    struct KeyEvent {
        char key;
        std::chrono::steady_clock::time_point press_time;
        std::chrono::steady_clock::time_point release_time;
    };
    std::vector<KeyEvent> key_events;
    std::vector<wxString> textSamples; // List of texts to cycle through
    size_t currentTextIndex;           // Index of current text
    static const size_t MIN_SAMPLES = 20; // Minimum keystrokes for GMM

    void OnBackEnter(wxMouseEvent& event);
    void OnBackLeave(wxMouseEvent& event);
    void OnBackClicked(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnEnterPressed(wxCommandEvent& event); // Check text and authenticate
    void UpdateText();                          // Switch to next text

    DECLARE_EVENT_TABLE()
};