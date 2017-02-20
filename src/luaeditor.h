#ifndef __LuaEditor__
#define __LuaEditor__

#include "public.sdk/source/vst2.x/aeffeditor.h"

#ifndef __vstgui__
#include "vstgui.sf/vstgui/vstgui.h"
#include "vstgui.sf/vstgui/ctabview.h"
#include "vstgui.sf/vstgui/cfileselector.h"
#endif


extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "sysfuncs.h"
#include "xypad.h"
#include "linemanager.h"
#include "luagui.h"

enum {
	// bitmaps
	kGui = 128,
    kTab,
    kReset,
    kMidi,
    kGui2,
    kSwitch,   
};


class VstLua;

#include "cbitmapex.h"


/* flashing button */
class CTransientButton : public COnOffButton
{
    public:
        CTransientButton(const CRect &size, CControlListener *listener, long tag, CBitmap *background, int frames, long style=kPreListenerUpdate):COnOffButton(size, listener, tag, background, style) { this->frames = frames; this->frameCtr=0;};        
                
        void trigger(void)
        {            setValue(1.0);         }
                
        virtual void setValue(float value)
        {
             frameCtr = frames;
             COnOffButton::setValue(value);
             setDirty(true);                   
        }        
        virtual void 	mouse (CDrawContext *pContext, CPoint &where, long button=-1) {}        
        virtual void draw(CDrawContext *context)
        {            
            COnOffButton::draw(context);            
             if(frameCtr)
            {
                frameCtr--;
                if(frameCtr)                
                    setValue(1.0);
                else
                    setValue(0.0);                
            }
            setDirty(true);            
        }        
    protected:
        int frames;
        int frameCtr;        
};


/* unclickable button (for led display) */
class CStickButton : public COnOffButton
{
    public:
        CStickButton(const CRect &size, CControlListener *listener, long tag, CBitmap *background, int frames, long style=kPreListenerUpdate):COnOffButton(size, listener, tag, background, style) {};                        
        virtual void 	mouse (CDrawContext *pContext, CPoint &where, long button=-1) {}                       
};



class ListenFrame : public CFrame
{
    public:
    
    ListenFrame (const CRect &size, void *pSystemWindow, void *pEditor) : CFrame (size, pSystemWindow,pEditor){};
    ListenFrame (const CRect &size, const char *pTitle, void *pEditor,  const long style=0) : CFrame(size, pTitle, pEditor,style) {};

    virtual bool 	onDrop (CDrawContext *context, CDragContainer *drag, const CPoint &where);
    
    

    protected:
  
};


class LuaEditor: public AEffGUIEditor, public CControlListener
{
public:

    LuaEditor(AudioEffect *effect);
	virtual ~LuaEditor();
	virtual void valueChanged(CDrawContext* context, CControl* control);
    void tripReset(double val);
    void addLine(char *str, int iserror);
    void clearListing();
    void addListing(char *str);
    void resetGUI();
    void tripMidiIn(void);
    void tripMidiOut(void);
    LuaGui *luaGUI;
    void setScriptName(char *str);
        int getWidth();
    int getHeight();
    void setGUISize(int w, int h);
    
    virtual bool onKeyUp(VstKeyCode &code);
    virtual bool onKeyDown(VstKeyCode &code);
    
    virtual void idle();
    void switchTab();
    void onDrop (CDragContainer *drag);
    
    
    
    
protected:
	virtual bool open(void *ptr);
	virtual void close();
    
    

    
    
    
protected:
int frameCtr;
int curTab;
VstLua *instance;
int guiWidth, guiHeight;


void updateSize();

CBitmapEx *backBitmap;
CBitmapEx *back2Bitmap;
CBitmapEx *tabBitmap;
CBitmapEx *resetBitmap;
CBitmapEx *midiBitmap;
CBitmapEx *switchBitmap;
CBitmapEx *guiBitmap;
void loadScript(void);
double resetState;
LineManager *mainLineManager;
LineManager *errorLineManager;
LineManager *listingLineManager;
CTabView *tabView;
CTextLabel *scriptLabel;
CViewContainer *messages, *gui;
CKickButton *loadButton;
CKickButton *helpButton;
CKickButton *haltButton;
COnOffButton *consoleButton;
COnOffButton *guiButton;
COnOffButton *resetButton;
COnOffButton *switchButton;
char scriptName[512];
CTransientButton *midiInButton, *midiOutButton;


};


#endif
