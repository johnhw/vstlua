#ifndef __customcontrol__
#define __customcontrol__
#include "public.sdk/source/vst2.x/aeffeditor.h"

#ifndef __vstgui__
#include "vstgui.sf/vstgui/vstgui.h"
#include "vstgui.sf/vstgui/ctabview.h"
#include "vstgui.sf/vstgui/cfileselector.h"
#endif


#include <lua.h>



int loadBitmapLua(lua_State *state);
int freeBitmapsLua(lua_State *state);
int getBitmapSizeLua(lua_State *state);





class CCustomControl : public CControl
{


CCustomControl(VstLua *instance, const CRect &size, CControlListener *listener=0, long tag=0, CBitmap *pBackground=0);// : CControl(size, listener, tag,pBackground);

~CCustomControl();
virtual void 	draw (CDrawContext *pContext);

virtual void 	mouse (CDrawContext *pContext, CPoint &where, long buttons=-1);

/*
virtual long 	onKeyDown (VstKeyCode &keyCode);
virtual long 	onKeyUp (VstKeyCode &keyCode);

virtual bool 	onWheel (CDrawContext *pContext, const CPoint &where, float distance);
virtual bool 	onWheel (CDrawContext *pContext, const CPoint &where, const CMouseWheelAxis axis, float distance);

virtual bool 	onDrop (CDrawContext *context, CDragContainer *drag, const CPoint &where);
virtual void 	onDragEnter (CDrawContext *context, CDragContainer *drag, const CPoint &where);
virtual void 	onDragLeave (CDrawContext *context, CDragContainer *drag, const CPoint &where);
virtual void 	onDragMove (CDrawContext *context, CDragContainer *drag, const CPoint &where);

virtual void 	setValue (float val);
virtual float 	getValue () const;
*/

};


#endif
