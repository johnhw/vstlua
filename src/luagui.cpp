extern "C" {
#include <lua.h>
}
#include "luagui.h"
#include "luaeditor.h"
#include "luautils.h"
#include "vstlua.h"
#include "customcontrol.h"

//#include "debug_new.h"

    void LuaGui::redraw(int control)
    {
        if(parent)
        {
        
            //force redraw of whole window if we get a -1
            if(control==-1)            
                parent->setDirty();
                
            //otherwise just do one control
            else if (control>=0 && control<controlTag && controls[control])
            {
                controls[control]->setDirty();            
            }                                    
        }    
    }
    
    
    void LuaGui::close()
    {
        //remove everything from the view
        if(parent)
        {
            for(int i=0;i<controlTag;i++)
            {
                if(controls[i] && parent->isChild(controls[i]))
                {
                    parent->removeView(controls[i]);
                    controls[i] = NULL;                    
                    types[i] = kNone;
                }
                if(labels[i] && parent->isChild(labels[i]))
                {
                    parent->removeView(labels[i]);                                                                                            
                    labels[i] = NULL;
                }
            }    
        }
        controlTag = 0;
        parent = NULL;                                
    }
    
    
    void LuaGui::open(CViewContainer *parent)
    {
        this->parent = parent;
        controlTag = 0;
        
              
        
        
    }
    
    
    

    LuaGui::LuaGui(VstLua *instance)
    {
     
        this->instance = instance;
        
        
        
        
        for(int i=0;i<MAX_CONTROLS;i++)
        {
            controls[i]=NULL;
            types[i] = kNone;
            labels[i] = NULL;        
        }
        
        //see luaeditor.h for bitmap resource id's        
        onOffBitmap = loadBitmap(kButton, "gui\\button.bmp");        
        ledBitmap = loadBitmap(kLed, "gui\\led.bmp");              
        knobBitmap = loadBitmap(kKnob, "gui\\knob.bmp");                
        hSliderBackBitmap = loadBitmap(kHsliderBack, "gui\\hslider.bmp");                         
        hSliderKnobBitmap = loadBitmap(kHsliderKnob, "gui\\hslider_handle.bmp");    

        
        vSliderBackBitmap = loadBitmap(kVsliderBack, "gui\\vslider.bmp");                
        vSliderKnobBitmap = loadBitmap(kVsliderKnob, "gui\\vslider_handle.bmp");                              
        menuBitmap = loadBitmap(kMenu, "gui\\menu_1.bmp");                                
        menu2Bitmap = loadBitmap(kMenu2, "gui\\menu_2.bmp");                
        
        xyPadBitmap = loadBitmap(kXYPad, "gui\\xypad.bmp");                                
        textBoxBitmap = loadBitmap(kTextBox, "gui\\textbox.bmp");                
        
        vuOnBitmap = loadBitmap(kVuOn, "gui\\vu_on.bmp");
        vuOffBitmap = loadBitmap(kVuOff, "gui\\vu_off.bmp");
        
        
        parent=NULL;
        controlTag = 0;        
    }
    
    
    void LuaGui::setGUISizes()
    {
        instance->setGUISize(onOffBitmap->getWidth(),onOffBitmap->getHeight()/2 , kGUIOnOffButton);
        instance->setGUISize(onOffBitmap->getWidth(), onOffBitmap->getHeight()/2,kGUIKickButton);
        instance->setGUISize(ledBitmap->getWidth(), ledBitmap->getHeight()/2,kGUIStickLed);
        instance->setGUISize(ledBitmap->getWidth(),ledBitmap->getHeight()/2, kGUIStickLed);
        instance->setGUISize(hSliderBackBitmap->getWidth(), hSliderBackBitmap->getHeight(),kGUIHSlider);
        instance->setGUISize(vSliderBackBitmap->getWidth(), vSliderBackBitmap->getHeight(),kGUIVSlider);
        instance->setGUISize(knobBitmap->getWidth(), knobBitmap->getHeight()/31,kGUIKnob);
        instance->setGUISize(menuBitmap->getWidth(), menuBitmap->getHeight(),kGUIMenu);
        instance->setGUISize(menuBitmap->getWidth(), menuBitmap->getHeight(),kGUILabel);
        instance->setGUISize(xyPadBitmap->getWidth(), xyPadBitmap->getHeight(),kGUIXYPad);
        instance->setGUISize(textBoxBitmap->getWidth(), textBoxBitmap->getHeight(),kGUITextBox);
        instance->setGUISize(vuOnBitmap->getWidth(), vuOnBitmap->getHeight(),kGUIVuMeter);
    
    }
    
    
    LuaGui::~LuaGui()
    {
    
   
        onOffBitmap->forget();
        knobBitmap->forget();
        hSliderBackBitmap->forget();
        hSliderKnobBitmap->forget();
        vSliderBackBitmap->forget();
        vSliderKnobBitmap->forget();
        ledBitmap->forget();        
        menuBitmap->forget();
        menu2Bitmap->forget();
        xyPadBitmap->forget();
        textBoxBitmap->forget();
        vuOnBitmap->forget();
        vuOffBitmap->forget();
    }
    
    
    
    //remove a control
    void LuaGui::removeControlLua(int tag)
    {
        if(parent && controls[tag] && parent->isChild(controls[tag]))
        {
            parent->removeView(controls[tag]);
            controls[tag]=NULL;                    
            types[tag] = kNone;
        }        
    }

    int LuaGui::addControlLua(int x, int y, int type, char *label)
    {    
                
        if(parent)
        {
        CControl *control=NULL;
                        
        CRect size, textsize;
        size.top = y;
        size.left = x;
                 
        int w=0, h=0;
        
        CPoint pt;
                   
        int created = 1;
        switch(type)
        {        
            //toggle button
            case kGUIOnOffButton:
            {
                w = onOffBitmap->getWidth();
                h = onOffBitmap->getHeight()/2;
                size.right = size.x + w;
                size.bottom = size.y + h;
                COnOffButton *obutton = new COnOffButton(size, this, controlTag, onOffBitmap, 0);
                control=obutton;                             
                break;
                }   
                
            //kick button
            case kGUIKickButton:
            {
                w = onOffBitmap->getWidth();
                h = onOffBitmap->getHeight()/2;
                size.right = size.x + w;
                size.bottom = size.y + h;
                
                pt(0,0);
                CKickButton *kbutton = new CKickButton(size, this, controlTag, onOffBitmap, pt);
                control=kbutton;
            
                textsize(x+w/2-40, y+h+3, x+w/2+40, y+h+24);                
                
                break;
                }
                
            //just a label
            case kGUILabel:
            {
                //don't need a control...                
                w = 0;
                h = 0;                 
                control = NULL;
                break;
                
              }
                
            //flicker led
            case kGUITransientLed:
            {
                w = ledBitmap->getWidth();
                h = ledBitmap->getHeight()/2;
                size.right = size.x + w;
                size.bottom = size.y + h;
                CTransientButton *tbutton = new CTransientButton(size, this, controlTag, ledBitmap, 1, 0);
                control=tbutton;               
                break;
            }
                
            //plain led
            case kGUIStickLed:
            {
                w = ledBitmap->getWidth();
                h = ledBitmap->getHeight()/2;
                size.right = size.x + w;
                size.bottom = size.y + h;
                CStickButton *sbutton = new CStickButton(size, this, controlTag, ledBitmap,0);
                control=sbutton;                           
                break;
            }
                
            //rotary knob
            case kGUIKnob:
            {
                w = knobBitmap->getWidth();
                h = knobBitmap->getHeight()/31;
                size.right = size.x + w;
                size.bottom = size.y + h;                
                pt(0,0);                
                CAnimKnob *knob= new CAnimKnob(size, this, controlTag, knobBitmap,pt);
                control=knob;                     
                break;
            }
            //rotary knob
            case kGUITextBox:
            {
                w = textBoxBitmap->getWidth();
                h = textBoxBitmap->getHeight();
                size.right = size.x + w;
                size.bottom = size.y + h;               
                pt(0,0);                
                CTextEdit *tedit= new CTextEdit(size, this, controlTag, "", textBoxBitmap,kShadowText);
                tedit->setShadowColor(kBlackCColor);
                control=tedit;
             }          
                break;
                
            //Xy pad
            case kGUIXYPad:
            {
                w = xyPadBitmap->getWidth();
                h = xyPadBitmap->getHeight();
                size.right = size.x + w;
                size.bottom = size.y + h;                
                pt(0,0);                
                CXYPad *xypad= new CXYPad(size, this, controlTag, xyPadBitmap,pt);
                control=xypad;                       
                break;       
            }
       
            //Horizontal slider
            case kGUIHSlider:
            {
                w = hSliderBackBitmap->getWidth();
                h = hSliderBackBitmap->getHeight();
                size.right = size.x + w;
                size.bottom = size.y + h;                
                pt(0,0);                
                CHorizontalSlider *hslider= new CHorizontalSlider(size, this, controlTag, x+6, x+w-6, hSliderKnobBitmap, hSliderBackBitmap,pt, kLeft);
                control=hslider;                     
                break;
             }      
            //vertical slider
            case kGUIVSlider:
            {
                w = vSliderBackBitmap->getWidth();
                h = vSliderBackBitmap->getHeight();
                size.right = size.x + w;
                size.bottom = size.y + h;                
                pt(0,0);                
                CVerticalSlider *vslider= new CVerticalSlider(size, this, controlTag, y+6, y+h-6, vSliderKnobBitmap, vSliderBackBitmap,pt, kBottom);
                control=vslider;                       
                break;    
            }
            
            //vu meter
            case kGUIVuMeter:
            {
                w = vuOnBitmap->getWidth();
                h = vuOnBitmap->getHeight();
                size.right = size.x + w;
                size.bottom = size.y + h;                                
                CVuMeter *vumeter= new CVuMeter(size, vuOnBitmap, vuOffBitmap, 20, kVertical);
                vumeter->setDecreaseStepValue(1.0);
                control=vumeter;                       
                break;    
            }
            
            
            case kGUIMenu:
            {
                w = menuBitmap->getWidth();
                h = menuBitmap->getHeight();
                size.right = size.x + w;
                size.bottom = size.y + h;                
                COptionMenu *menu = new COptionMenu(size, this, controlTag, menuBitmap, menu2Bitmap, 0);
                menu->setFont(kNormalFontVerySmall);
                control=menu;                        
                break;
            }
            
            default:
                created = 0;
                break;
        }
        
        
        if(created)
        
        {
            //Create the label for the control
            textsize(x+w/2-100, y+h+3, x+w/2+100, y+h+24);                
            CTextLabel *tlabel = new CTextLabel(textsize,label, 0, kShadowText);
            tlabel->setTransparency(true);
            tlabel->setBackColor(kTransparentCColor);
            tlabel->setShadowColor(kBlackCColor);
            tlabel->setTransparency(true);                        
            tlabel->setFont(kNormalFontVerySmall);                 
                           
            
            int insertPoint=-1;
            
            //find a space for the control
            for(int i=0;i<controlTag;i++)
            {
                if(types[i]==kNone)
                {
                    insertPoint = i;
                }            
            }
            
            if(insertPoint<0)
            {
                insertPoint = controlTag;
                controlTag++;
            }
            
            
            //register the control
            controls[insertPoint] = control;
            labels[insertPoint]=tlabel;
            types[insertPoint] = type;
            
            //show them, if we're open
            parent->addView(controls[insertPoint]);
            parent->addView(labels[insertPoint]);
                    
            
            return insertPoint;
        }
        else
             return -1;
        
        }
        return -1;
        
    }
    
    
    //Set the label of a control
      void LuaGui::setLabelLua(int tag, char *string, int highlight)
      {
        if(tag>=0 && tag<controlTag)
        {
            labels[tag]->setText(string);
            
            //set properties 
            if(highlight)
            {
                labels[tag]->setBackColor(kWhiteCColor);
                labels[tag]->setFontColor(kBlackCColor);
            }
            else
            {
                labels[tag]->setBackColor(kTransparentCColor);
                labels[tag]->setFontColor(kWhiteCColor);
            }                    
            labels[tag]->setDirty(true);
        }
        
      
      }
        
       

    //construct a submenu...
    COptionMenu *LuaGui::constructSubMenu(MenuEntry *entries, int baseTag)
    {
        CRect size;
        size(0,0,0,0);        
        COptionMenu *newMenu = new COptionMenu(size, NULL, baseTag, 0, 0);
        while(entries!=NULL)
        {
                if(entries->sub==NULL)
                {
                   newMenu->addEntry(entries->txt);                
                }
                else
                {
                    COptionMenu *subMenu = constructSubMenu(entries->sub, baseTag);
                    newMenu->addEntry(subMenu, entries->txt);                                
                }
                entries = entries->next;                
         }       
        return newMenu;
    
    }
    
    //Construct a complete menu....
    void LuaGui::addTreeToMenu(int item, MenuEntry *entries)
    {
        if(types[item]==kGUIMenu)
        {            
            while(entries!=NULL)
            {
                if(entries->sub==NULL)
                {
                    ((COptionMenu*)(controls[item]))->addEntry(entries->txt);                
                }
                else
                {
                    COptionMenu *subMenu = constructSubMenu(entries->sub, item);
                    ((COptionMenu*)(controls[item]))->addEntry(subMenu, entries->txt);                                
                }
                entries = entries->next;
        }
      }   
    }
    
    //Callback when user changes control value
    void LuaGui::valueChanged(CDrawContext *context, CControl *control)
    {
   
        int tag = control->getTag();   
        
        char str[128];
        if(types[tag]==kGUIMenu)
            {
                ((COptionMenu *)control)->getCurrent(str);
                instance->onValueChanged(tag, control->getValue(), str);
            }
        else 
        if(types[tag]==kGUITextBox)
            {
                ((CTextEdit *)control)->getText(str);
                instance->onValueChanged(tag, control->getValue(), str);
            }
        else 
        if(types[tag]==kGUIXYPad)
            {
                int x = ((CXYPad *)control)->getX();
                int y = ((CXYPad *)control)->getY();               
                snprintf(str, sizeof(str), "{%d, %d}",  x, y);
                instance->onValueChanged(tag, 0.0, str);
            }
        
         else
            instance->onValueChanged(tag, control->getValue(), NULL);
        
        
    }
    
    
    
    //Set the value of a control
    void LuaGui::setValueLua(int tag, float value)
    {        
        if(tag>=0 && tag<controlTag && controls[tag]!=NULL)        
        {
            controls[tag]->setValue(value);               
            
           
            
        }
    }
    
    //Get the value of a control
    float LuaGui::getValueLua(int tag)
    {
        if(tag>=0 && tag<controlTag && controls[tag]!=NULL)        
            return controls[tag]->getValue();                   
        else
            return 0;            
    }

