#include "luaeditor.h"
#include "vstlua.h"
#include "customcontrol.h"


int getBitmapSizeLua(lua_State *state)
{
    if(lua_isuserdata(state, -1))
    {
        CBitmapEx *bmp = (CBitmapEx *)lua_touserdata(state,-1);
        lua_newtable(state);
        lua_pushstring(state, "width");
        lua_pushnumber(state, bmp->getWidth());
        
        lua_settable(state,-3);
        
        lua_pushstring(state, "height");
        lua_pushnumber(state, bmp->getHeight());
        
        lua_settable(state,-3);
        
        return 1;    
    }
    else
    {    
        return 0;
    }

}


//must pass in filename, resource number
int loadBitmapLua(lua_State *state)
{
    VstLua *instance = getInstance(state);
    
    if(!lua_isstring(state,1))
    {
        instance->fatal_error("Called loadBitmap without a string filename!\n");
        return 0;
    }
    
    CBitmapEx *bmp;
    
    
    //load the file
    
    bmp = loadBitmap((int)lua_tonumber(state,2), (char*)lua_tostring(state,1));
    
    lua_pop(state,2);
            
    
    if(bmp)
    {
        //register the bitmap for destruction
        lua_getglobal(state, "_bitmapsLoaded");
        
        //create it if it doesn't exist/is invalid
        if(!lua_istable(state,-1))
        {
            lua_newtable(state);
            lua_setglobal(state, "_bitmapsLoaded");
            lua_getglobal(state, "_bitmapsLoaded");        
        }
        
        //insert a new entry
        int entries = lua_objlen(state,-1);
        lua_pushnumber(state, entries+1);
        lua_pushlightuserdata(state, bmp);
        lua_settable(state,-3);
        lua_pop(state,1);
        //push a copy for the return
        lua_pushlightuserdata(state, bmp);
        return 1;                                
    }
    else
    {
        instance->normal_output("Could not load bitmap %s\n", lua_tostring(state,-1));
        return 0;   
    }

}



int freeBitmapsLua(lua_State *state)
{
        //get the bitmap table
        lua_getglobal(state, "_bitmapsLoaded");
        
        //loop through the table
        if(lua_istable(state,-1))
        {
             lua_pushnil(state);  /* first key */
            while (lua_next(state, -2) != 0) 
            {
                    /* uses 'key' (at index -2) and 'value' (at index -1) */
                    
                    //free the bitmap
                    CBitmapEx *bmp = (CBitmapEx*) lua_touserdata(state,-1);
                    if(bmp)                   
                        bmp->forget();                    
                
                    /* removes 'value'; keeps 'key' for next iteration */
                    lua_pop(state, 1);               
            }
        }
        return 0;
}



CCustomControl::CCustomControl(VstLua *instance, const CRect &size, CControlListener *listener, long tag, CBitmap *pBackground) : CControl(size, listener, tag,pBackground){}

CCustomControl::~CCustomControl() {}
void 	CCustomControl::draw (CDrawContext *pContext) {}

void 	CCustomControl::mouse (CDrawContext *pContext, CPoint &where, long buttons) {}

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




