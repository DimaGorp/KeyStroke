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
    return true;
}
