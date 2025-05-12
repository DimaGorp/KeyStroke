#include <wx/wx.h>
#include "HomePage/HomePage.hpp"
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    HomePage *frame = new HomePage(nullptr,              // parent
        wxID_ANY,             // id
        "Hello Everyone!",    // title
        wxDefaultPosition,    // pos (specific x,y coordinates)
        wxDefaultSize,     // size (specific width,height)
        wxDEFAULT_FRAME_STYLE // style
       );
    frame->Show(true);
    /*
    std::string username = "Dima";
std::vector<Vec2> keystrokes = { {120.0, 80.0}, {130.0, -50.0} }; // Example keystroke data
double likelihood = 0.85; // Example likelihood value

WelcomePage* frame = new WelcomePage(nullptr,
    wxID_ANY,
    "Welcome!",
    username,
    keystrokes,
    likelihood,
    wxDefaultPosition,
    wxDefaultSize,
    wxDEFAULT_FRAME_STYLE
);
frame->Show(true);*/
    return true;
}
