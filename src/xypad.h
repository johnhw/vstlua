#ifndef __xypad__
#define __xypad__
#include "public.sdk/source/vst2.x/aeffeditor.h"

#ifndef __vstgui__
#include "vstgui.sf/vstgui/vstgui.h"
#include "vstgui.sf/vstgui/ctabview.h"
#include "vstgui.sf/vstgui/cfileselector.h"
#endif
class CXYPad : public CMovieBitmap
{
    public:
        CXYPad(const CRect &size, CControlListener *listener, long tag, CBitmap *background, CPoint &offset) : CMovieBitmap(size, listener, tag, background, offset) { setMouseEnabled(true); x = 0; y = 0; }
        
      int getX();
      int getY();
        
      virtual void onDragMove (CDrawContext *context, CDragContainer *drag, const CPoint &where);      
      virtual void 	mouse (CDrawContext *pContext, CPoint &where, long button=-1);      
      virtual void draw(CDrawContext *pContext);      
      protected:
      int x, y;
        

};
      
#endif
