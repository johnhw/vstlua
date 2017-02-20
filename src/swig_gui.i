%module vstgui
%{
#include "public.sdk/source/vst2.x/aeffeditor.h"

#ifndef __vstgui__
#include "vstgui.sf/vstgui/vstgui.h"
#include "vstgui.sf/vstgui/ctabview.h"
#include "vstgui.sf/vstgui/vstcontrols.h"
#include "vstgui.sf/vstgui/cfileselector.h"
#endif
#include "cbitmapex.h"


//wrapper to allow pushing of CViewContainer's (so the script can access the main GUI panel)
int pushCViewContainerLua(lua_State *state, CViewContainer *cont)
{
    //make sure object is not owned b the interpreter
    SWIG_NewPointerObj(state,cont,SWIGTYPE_p_CViewContainer,0);  
    return 1;
    
}
%}

/*

%include "vstgui.sf/vstgui/ctabview.h"
*/


%include "vstgui.sf/vstgui/vstgui.h"
%include "vstgui.sf/vstgui/vstkeycode.h"
%include "vstgui.sf/vstgui/vstcontrols.h"

%include "cbitmapex.h"
