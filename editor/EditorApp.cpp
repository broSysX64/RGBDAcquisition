/***************************************************************
 * Name:      EditorApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Ammar Qammaz (ammarkov+rgbd@gmail.com)
 * Created:   2013-10-22
 * Copyright: Ammar Qammaz (http://ammar.gr)
 * License:
 **************************************************************/

#include "EditorApp.h"

//(*AppHeaders
#include "EditorMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(EditorApp);

bool EditorApp::OnInit()
{

    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	EditorFrame* Frame = new EditorFrame(0);


        //Recover commandline options
        Frame->argc = this->argc;
        Frame->argv = (char**) this->argv;
        std::cout<<"We have "<<this->argc<<" command line arguments \n";

    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}


