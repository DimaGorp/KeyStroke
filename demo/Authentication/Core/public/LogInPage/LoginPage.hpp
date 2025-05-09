#pragma once

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/log.h>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "Core/GMM/GMM.hpp"
#include "KeyEvent.hpp"

class Login : public wxFrame {
public:
    Login(wxWindow* parent, wxWindowID id, const wxString& title, 
          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
          long style = wxDEFAULT_FRAME_STYLE);
    ~Login();

private:
    static const size_t MIN_SAMPLES = 50;

    wxButton* Back;
    wxStaticText* HelpingText;
    wxStaticText* Text;
    wxTextCtrl* usernameInput;
    wxTextCtrl* EnterArea;
    std::vector<wxString> textSamples;
    size_t currentTextIndex;
    size_t testSectionId;
    std::vector<KeyEvent> key_events;
    GMM storedGmm;
    wxString gmmFilename;
    std::map<std::string, std::pair<unsigned long long, std::string>> userMap; // Store PARTICIPANT_ID and SENTENCES

    bool LoadUserMap(const std::string& filename);
    std::string EscapeCSVField(const std::string& field) const;
    void SaveKeystrokesToCSV(const std::string& filename, const std::vector<KeyEvent>& events, 
                            unsigned long long participantId, size_t testSectionId, bool firstWrite);
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