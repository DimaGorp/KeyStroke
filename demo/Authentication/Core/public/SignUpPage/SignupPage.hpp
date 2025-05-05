#pragma once

#include <wx/artprov.h>
#include <wx/wx.h>
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
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "Core/GMM/GMM.hpp"


class SignUp : public wxFrame {
public:
    SignUp(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);
    ~SignUp();

private:
    static const size_t MIN_SAMPLES = 50;

    wxStaticText* NameLabel;
    wxTextCtrl* m_textCtrl2;
    wxStaticText* HelpingText;
    wxStaticText* Text;
    wxTextCtrl* EnterArea;
    std::vector<wxString> textSamples;
    size_t currentTextIndex;

    std::vector<KeyEvent> all_key_events;
    std::vector<KeyEvent> key_events;
    std::vector<Vec2> features;

    void OnTextCtrlFocus(wxFocusEvent& event);
    void OnTextCtrlEnter(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnEnterPressed(wxCommandEvent& event);
    void UpdateText();
    void SaveFeaturesToCSV(const std::string& filename, const std::vector<KeyEvent>& events, bool firstWrite);
    std::string EscapeCSVField(const std::string& field) const;

    DECLARE_EVENT_TABLE()
};