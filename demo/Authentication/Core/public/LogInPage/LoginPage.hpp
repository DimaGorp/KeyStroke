#pragma once

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <vector>
#include <chrono>
#include <string>
#include <map>
#include "Core/GMM/GMM.hpp"
#include "Keys/KeyEvent.hpp"

class Login : public wxFrame {
public:
    Login(wxWindow* parent, wxWindowID id, const wxString& title, 
          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
          long style = wxDEFAULT_FRAME_STYLE);
    ~Login();

private:
    static const size_t MIN_SAMPLES = 50; // Match SignUp

    wxButton* Back;
    wxStaticText* HelpingText;
    wxStaticText* Text;
    wxTextCtrl* EnterArea;
    wxTextCtrl* usernameInput;
    std::vector<wxString> textSamples;
    size_t currentTextIndex;
    wxString gmmFilename;
    size_t testSectionId; // Track TEST_SECTION_ID

    std::vector<KeyEvent> key_events;
    GMM storedGmm;
    std::map<std::string, size_t> userMap; // NAME to PARTICIPANT_ID

    void OnBackEnter(wxMouseEvent& event);
    void OnBackLeave(wxMouseEvent& event);
    void OnBackClicked(wxCommandEvent& event);
    void OnUsernameEnter(wxCommandEvent& event);
    void LoadGMMFromUsername();
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnEnterPressed(wxCommandEvent& event);
    void UpdateText();
    bool LoadUserMap(const std::string& filename);
    void SaveKeystrokesToCSV(const std::string& filename, const std::vector<KeyEvent>& events, 
                            size_t participantId, size_t testSectionId, bool firstWrite);
    std::string EscapeCSVField(const std::string& field) const;

    wxDECLARE_EVENT_TABLE();
};