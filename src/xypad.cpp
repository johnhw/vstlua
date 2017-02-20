
#include "xypad.h"




        
      int CXYPad::getX(){ return x; }
      int CXYPad::getY() { return y; }
        
       void CXYPad::onDragMove (CDrawContext *context, CDragContainer *drag, const CPoint &where)
      {
          CPoint realWhere = where;
          CPoint localWhere = frameToLocal(realWhere);
          x = localWhere.x;
          y = localWhere.y;     
          if(listener)
            listener->valueChanged(context, this);      
      }
      
       void 	CXYPad::mouse (CDrawContext *pContext, CPoint &where, long button) {
      if (button==-1) button = pContext->getMouseButtons ();
      CPoint realWhere = where;
      CPoint localWhere = frameToLocal(realWhere);      
      x = localWhere.x;
      y = localWhere.y;
      if(listener)
        listener->valueChanged(pContext, this);      
      
      }   
      
       void CXYPad::draw(CDrawContext *pContext)
      {
        CMovieBitmap::draw(pContext);
        CRect knob;
        knob(x-3,y-3,x+3,y+3);
        pContext->fillEllipse(knob);
        
          
      }
      
