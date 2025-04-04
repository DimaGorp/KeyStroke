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
#include <wx/wx.h>
#include "GMM/GMM.hpp"

class Login : public wxFrame {
public:
    Login(wxWindow* parent, wxWindowID id, const wxString& title, 
          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
          long style = wxDEFAULT_FRAME_STYLE);
    ~Login();

private:
    struct KeyEvent {
        int key;
        std::chrono::steady_clock::time_point press_time;
        std::chrono::steady_clock::time_point release_time = std::chrono::steady_clock::time_point();
    };

    wxButton* Back;
    wxStaticText* HelpingText;
    wxStaticText* Text;
    wxTextCtrl* EnterArea;
    wxTextCtrl* usernameInput; // New username input
    std::vector<wxString> textSamples;
    size_t currentTextIndex;
    std::vector<KeyEvent> key_events;
    std::vector<Vec2> features; // Assuming Vec2 is {x: dwell, y: flight}
    wxString gmmFilename;
    GMM storedGmm;

    void OnBackEnter(wxMouseEvent& event);
    void OnBackLeave(wxMouseEvent& event);
    void OnBackClicked(wxCommandEvent& event);
    void OnUsernameEnter(wxCommandEvent& event);
    void LoadGMMFromUsername();
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnEnterPressed(wxCommandEvent& event);
    void UpdateText();

    DECLARE_EVENT_TABLE()
};
