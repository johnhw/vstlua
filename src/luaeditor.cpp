
#include "luaeditor.h"
#include "vstlua.h"
#include "guidefs.h"

#include <stdio.h>

enum 
{
 kResetTag = 40,
 kMidiTag,
 kLoadTag,
 kHaltTag,
 kSwitchTag,
 kHelpTag,
 kConsoleTag
};




 
     bool 	ListenFrame::onDrop (CDrawContext *context, CDragContainer *drag, const CPoint &where)
    {
        ((LuaEditor*)pEditor)->onDrop(drag);            
        return true;
    }


    
    bool LuaEditor::onKeyDown(VstKeyCode &keyCode)
    {     
        return instance->onKey(keyCode, 1);        
    }
    
    bool LuaEditor::onKeyUp(VstKeyCode &keyCode)
    {     
        return instance->onKey(keyCode, 0);        
    }
    
    
    void LuaEditor::onDrop(CDragContainer *drag)
    {
        instance->onDrop(drag);        
    }
    
    
    

LuaEditor::LuaEditor(AudioEffect *effect)
 : AEffGUIEditor(effect) 
{
    
    scriptLabel = 0;
    
    resetState = 0;
	
    frame = 0;
	backBitmap = 0;
    resetBitmap = 0;
	midiBitmap = 0;
    switchBitmap = 0;
    resetButton = 0;
    messages =0;
    gui = 0;
    tabView =0;
    luaGUI = 0;
    loadButton = 0;
    haltButton = 0;
    resetButton = 0;
    switchButton = 0;
    consoleButton = 0;
    midiInButton = 0;
    midiOutButton = 0;
    frameCtr = 0;
        
    
    instance = (VstLua*)effect;  

    
    strncpy(scriptName, "No script", sizeof(scriptName));
    
    

    backBitmap = loadBitmap(kGui);
    back2Bitmap = loadBitmap(kGui2);
    tabBitmap = loadBitmap(kTab);
    resetBitmap=loadBitmap(kReset);
    midiBitmap=loadBitmap(kMidi);
    switchBitmap=loadBitmap(kSwitch);
    guiBitmap = loadBitmap(kButton);
    
	       
           
    //set the default size
    guiWidth = back2Bitmap->getWidth();
    guiHeight = back2Bitmap->getHeight();
    
    //Create the linemanagers
    mainLineManager = new LineManager(128, 100);
    errorLineManager = new LineManager(128, 100);
    listingLineManager = new LineManager(1024, 50);
        
        
    luaGUI = new LuaGui(instance);    
    
    
	// init the size of the plugin
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = (short)backBitmap->getWidth();
	rect.bottom = (short)backBitmap->getHeight();
    
}


void LuaEditor::addLine(char *str, int iserror)
{
 

   if(!iserror)
   {
    if(mainLineManager)
        mainLineManager->addLine(str, 0);
   }
   else
   {
   if(errorLineManager)
    errorLineManager->addLine(str, 1);
   }
   
   if(frame)
    frame->setDirty(true);
    

}




LuaEditor::~LuaEditor()
{

	// free background bitmap
	if(backBitmap)
		backBitmap->forget();
        
    if(back2Bitmap)
        back2Bitmap->forget();
        
        
    tabBitmap->forget();
    resetBitmap->forget();
    midiBitmap->forget();
    switchBitmap->forget();
    guiBitmap->forget();
    
               
    if(luaGUI)
    {
            delete luaGUI;
            luaGUI=0;
    }
    
    delete mainLineManager;
    mainLineManager = NULL;
    delete errorLineManager;
    errorLineManager=NULL;
    delete listingLineManager;
    listingLineManager=NULL;
           
	backBitmap = 0;
    back2Bitmap = 0;
    
}


//Reset the gui. Close it, then reopen it, to reset all controls
void LuaEditor::resetGUI()
{
    
    
    instance->onClosed();
    if(tabView)
        tabView->selectTab(0);        
    luaGUI->close();    
    
    setGUISize(0,0); //reset gui size
    if(gui)
        luaGUI->open(gui);        
    instance->onOpened(gui);    
    
}


void LuaEditor::setScriptName(char *name)
{
    int last = strlen(name)-2;
    
    //chop off the path
    char *basename = name;
    while(last>=0)
    {
        if(name[last]=='\\')
        {
            basename = &(name[last+1]);                
            break;
        }
        last--;
    }    
    strncpy(scriptName, basename, sizeof(scriptName));
    if(scriptLabel)
    {
        scriptLabel->setText(scriptName);    
        scriptLabel->setDirty(true);
    }
}


//get custom gui width
int LuaEditor::getWidth()
{    
    return guiWidth;

}

//get custom gui height
int LuaEditor::getHeight()
{
    return guiHeight;
}


//set the size of the custom gui
void LuaEditor::setGUISize(int w, int h)
{
    
    guiWidth = w;
    guiHeight = h;
   
    //if we pass 0, use the default size
    if(guiWidth==0)    
        guiWidth = back2Bitmap->getWidth();
    if(guiHeight==0)
        guiHeight = back2Bitmap->getHeight();
            
    updateSize();    
}


//update the console window
void LuaEditor::idle()
{
    int keepWindow = instance->updateConsole();
    
    if(!keepWindow && instance->console)
    {
        instance->console->hide();
        
        //reset the button
        if(consoleButton)
        {
                consoleButton->setValue(0.0);
        }
    
    }
    
    AEffGUIEditor::idle();

}


//Update the size so the custom gui is the appropriate size
void LuaEditor::updateSize()
{

    if(frame)
        {   
            int w,h;
            //message window size
            if(!curTab)
            {
                w = backBitmap->getWidth();
                h = backBitmap->getHeight();
            }
            else //custom size
            {
                w = guiWidth;
                h = guiHeight;
            }
            
            CRect size;
            rect.right = rect.left + w;
            rect.bottom = rect.top + h;
            frame->setSize(w,h);
            frame->getViewSize(size);
            size.right = (size.right - size.width()) + w;
            size.bottom = (size.bottom - size.height()) + h;
            frame->setViewSize(size);
            frame->setDirty(true);
        }       
}
    


#define MAKE_SIZE(X,Y,BMP,DIV) size(X,Y,X+BMP->getWidth(), Y+BMP->getHeight()/DIV)
bool LuaEditor::open(void *ptr)
{
    
	CPoint point;
	CRect size;        
	AEffGUIEditor::open(ptr);
    point(0,0);    
	//--init background frame-------------------------------------------------
	size(0, 0, backBitmap->getWidth(), backBitmap->getHeight());
	frame = new ListenFrame(size, ptr, this);    
    
    //Create the tab view
    tabView = new CTabView(size, frame, NULL, NULL);                
    messages = new CViewContainer(size, frame, backBitmap); 
    gui = new CViewContainer(size, frame, back2Bitmap);    
    
    

    
    //add the tabs
    tabView->addTab(messages, "Messages", NULL);
    tabView->addTab(gui, "GUI", NULL);           
	tabView->setBackground(backBitmap);    
    frame->addView(tabView);
    
    curTab = 0;
    
    
    size(GUI_LABEL_X1, GUI_LABEL_Y1, GUI_LABEL_X2, GUI_LABEL_Y2);
    scriptLabel = new CTextLabel(size, scriptName, 0, kShadowText);
    scriptLabel->setTransparency(true);
    scriptLabel->setBackColor(kTransparentCColor);
    scriptLabel->setShadowColor(kBlackCColor);
    scriptLabel->setHoriAlign(kRightText);
    scriptLabel->setTransparency(true);                        
    scriptLabel->setFont(kNormalFontSmall);  
    messages->addView(scriptLabel);

    //Create the reset button            
    MAKE_SIZE(GUI_RESET_X, GUI_RESET_Y, resetBitmap, 2);
    resetButton = new COnOffButton(size, this, kResetTag, resetBitmap, 0);
    resetButton->setValue(resetState);
    messages->addView(resetButton);
    
    //Create the kick button
    MAKE_SIZE(GUI_LOAD_X, GUI_LOAD_Y, resetBitmap, 2);                
    loadButton = new CKickButton(size, this, kLoadTag, resetBitmap, point);    
    messages->addView(loadButton);
        
    //Create the kick button    
    MAKE_SIZE(GUI_HALT_X, GUI_HALT_Y, resetBitmap, 2);    
    haltButton = new CKickButton(size, this, kHaltTag, resetBitmap, point);    
    messages->addView(haltButton);
    
    //Create the kick button    
    MAKE_SIZE(GUI_HELP_X, GUI_HELP_Y, resetBitmap, 2);    
    helpButton = new CKickButton(size, this, kHelpTag, resetBitmap, point);    
    messages->addView(helpButton);
    
    
    MAKE_SIZE(GUI_CONSOLE_X, GUI_CONSOLE_Y, resetBitmap, 2);    
    consoleButton = new COnOffButton(size, this, kConsoleTag, resetBitmap, 0);    
    messages->addView(consoleButton);
    
    
          
    //Create the midi buttons        
    MAKE_SIZE(GUI_MIDI_IN_X, GUI_MIDI_IN_Y, midiBitmap, 2);
    midiInButton = new CTransientButton(size, this, kMidiTag, midiBitmap, 1, 0);    
    messages->addView(midiInButton);    
    
    MAKE_SIZE(GUI_MIDI_OUT_X, GUI_MIDI_OUT_Y, midiBitmap, 2);
    midiOutButton = new CTransientButton(size, this, kMidiTag, midiBitmap, 1, 0);    
    messages->addView(midiOutButton);
        
    //Create the switch view button
    MAKE_SIZE(GUI_TAB_BUTTON_X, GUI_TAB_BUTTON_Y, switchBitmap, 2);    
    switchButton = new COnOffButton(size, this, kSwitchTag, switchBitmap, 0);
    messages->addView(switchButton);
    
    //need to add one to the GUI panel as well!
    MAKE_SIZE(GUI_TAB2_BUTTON_X, GUI_TAB2_BUTTON_Y, guiBitmap, 2);    
    guiButton = new COnOffButton(size, this, kSwitchTag, guiBitmap, 0);
    gui->addView(guiButton);
    
    
    
    //Create the message window
    size(GUI_ERROR_X1,GUI_ERROR_Y1,GUI_ERROR_X2,GUI_ERROR_Y2);
    errorLineManager->show(size, GUI_ERROR_LINES, 10, backBitmap, frame, messages);
        
    size(GUI_MSG_X1,GUI_MSG_Y1,GUI_MSG_X2,GUI_MSG_Y2);
    mainLineManager->show(size, GUI_MSG_LINES, 16,  backBitmap, frame, messages);
    
    
    
    //Show the gui manager
    luaGUI->open(gui);    
    instance->onOpened(gui);
                
    
	return true;

}

//-----------------------------------------------------------------------------
void LuaEditor::close()
{

    
    mainLineManager->hide();
    errorLineManager->hide();
    //listingLineManager->hide();
    
    
    instance->onClosed();
    luaGUI->close();
       
 	delete frame;    
      
       
    gui = 0;    
    midiInButton = midiOutButton = 0;    
    resetButton=0;
	frame = 0;
    tabView = 0;	      
    scriptLabel = 0;
}

void LuaEditor::tripMidiIn()
{

    if(midiInButton)
        midiInButton->trigger();
        
}

void LuaEditor::tripMidiOut()
{

    if(midiOutButton)
        midiOutButton->trigger();    
        
}




void LuaEditor::tripReset(double val)
{
    
    resetState=val;    
    if(resetState==0.0)
    {                
        errorLineManager->clear();
        mainLineManager->clear();    
    }        
    if(resetButton)
        resetButton->setValue(resetState);       
    
    //must redraw now
    if(frame)
        frame->setDirty(true);
}



void LuaEditor::clearListing()
{

    listingLineManager->clear();
}

void LuaEditor::addListing(char *str)
{    

    listingLineManager->addLine(str, 0);
}


    


void LuaEditor::loadScript(void)
{

    //prepare the fileselect structure
     VstFileSelect selectStruct;
    
     selectStruct.command=kVstFileLoad;
     selectStruct.returnPath = NULL;
     
     selectStruct.nbFileTypes=1; 
     VstFileType lua(".lua","lua","lua");
     selectStruct.fileTypes=&lua; 
    snprintf(selectStruct.title, 256, "Load LUA script...");
    char init_path[512];
    snprintf(init_path, sizeof(init_path),"%s\\scripts\\*.lua",  instance->base_path);    
    selectStruct.initialPath = init_path;
    
    
    //select the file
    CFileSelector *fileSelector = new CFileSelector(instance);
    fileSelector->run(&selectStruct);
    
    if(selectStruct.nbReturnPath>0)
    {
        instance->setScript(selectStruct.returnPath);
        
        
        
    }
    
}


void LuaEditor::switchTab()
{
curTab=!curTab;
if(tabView)
    tabView->selectTab(curTab);
updateSize(); //update the window size as neeeded


}

//-----------------------------------------------------------------------------
void LuaEditor::valueChanged(CDrawContext* context, CControl* control)
{
	long tag = control->getTag();

	switch(tag)
	{	            
        case kResetTag:
            instance->reInit();           
            break; 
            
        case kHaltTag:
            if(control->getValue()==1.0)
                instance->fatal_error("User halted!\n");
            break;
            
        case kSwitchTag:
            switchTab();
            control->setValue(0.0); //make sure the switch doesn't get stuck
            frame->setDirty(true); //force redraw
            
            
            break;
            
        case kHelpTag:
            if(control->getValue()==0.0)
            {
        
            char *help = instance->onHelp();
            //we don't own this string, must copy it
            
            if(help)
            {
                char *helpCopy = new char[strlen(help)+1];
                strcpy(helpCopy, help);
                sysCreateHelpWindow(helpCopy);
                delete helpCopy;
            }
            
            }
            break;
            
        case kConsoleTag:
            {
                if(instance->console)
                    {
                        if(control->getValue()==0.0)
                            instance->console->hide();
                        else
                        {
                            int w, h;        
                            w = instance->getFromConfig("console_width", 300);
                            h = instance->getFromConfig("console_height", 700);        
                            const char *path = instance->getFromConfig("console_font_path", "consolefont\\ConsoleFont.bmp");
                            char fontPath[1024];
                            snprintf(fontPath, sizeof(fontPath), "%s\\%s", instance->base_path, path);        
                            instance->console->show(w,h, fontPath);    
                        }
                    }   
            }
            break;
            
        case kLoadTag:
            if(control->getValue()==1.0)
                loadScript();
            break;
	}
    
    
}



