#ifndef __LuaGui__
#define __LuaGui__

#include "public.sdk/source/vst2.x/aeffeditor.h"

#ifndef __vstgui__
#include "vstgui.sf/vstgui/vstgui.h"
#include "vstgui.sf/vstgui/ctabview.h"
#include "vstgui.sf/vstgui/cfileselector.h"
#endif



enum {	
    kKnob = 134,
    kHsliderBack,
    kHsliderKnob,
    kVsliderBack,
    kVsliderKnob,
    kLed,
    kMenu,
    kMenu2,
    kTextBox,
    kXYPad,   
    kButton,  
    kVuOn,
    kVuOff    
};

class VstLua;


class MenuEntry
{
    public:
    char *txt;
    MenuEntry *sub;
    MenuEntry *next;
        
    MenuEntry() { txt=NULL; sub=NULL; next=NULL; }
    MenuEntry(char *txt, MenuEntry *sub, MenuEntry *next) { this->txt=txt; this->sub=sub; this->next=next;}
    ~MenuEntry()
    {
        if(sub)
            delete sub;
        if(next)
            delete next;
    }
        
};


enum 
{
   kGUIOnOffButton=0,
   kGUIKickButton,
   kGUITransientLed,
   kGUIStickLed,
   kGUIKnob,
   kGUIHSlider,
   kGUIVSlider,
   kGUILabel,
   kGUIMenu,
   kGUIXYPad,
   kGUITextBox,
   kGUIVuMeter,
   kNone,
};

#define MAX_CONTROLS 8192


class CBitmapEx;
class LuaGui : CControlListener
{
    public:
    LuaGui(VstLua *instance);
    virtual ~LuaGui();
    
    
  
    void close();
    void open(CViewContainer *parent);
    
    int addControlLua(int x, int y, int type, char *label);

    void addTreeToMenu(int item, MenuEntry *entries);
    
    COptionMenu *constructSubMenu(MenuEntry *entries, int baseTag);

    
    void setValueLua(int control, float value);
    float getValueLua(int control);
    void setLabelLua(int control, char *label, int highlighted);    
    
    void redraw(int control);
    virtual void valueChanged(CDrawContext *context, CControl *control);
    void removeControlLua(int tag);
    
    void setGUISizes();
    
    protected:
    int controlTag;
    
    VstLua *instance;
    
    CViewContainer *parent;
    
    CBitmapEx *onOffBitmap;
    CBitmapEx *knobBitmap;
    CBitmapEx *hSliderBackBitmap;
    CBitmapEx *hSliderKnobBitmap;
    CBitmapEx *vSliderBackBitmap;
    CBitmapEx *vSliderKnobBitmap;
    CBitmapEx *ledBitmap;
    CBitmapEx *menuBitmap;
    CBitmapEx *menu2Bitmap;
    CBitmapEx *textBoxBitmap;
    CBitmapEx *xyPadBitmap;
    CBitmapEx *vuOnBitmap, *vuOffBitmap;
    
    
    int types[MAX_CONTROLS];
    CControl *controls[MAX_CONTROLS];
    CTextLabel *labels[MAX_CONTROLS];
    
    lua_State *mainState;
    int guiRef;
    
    
};

#endif
