

#include "console.h"
#include "vstlua.h"



 Console::Console(VstLua *instance)
  {
    consoleLock = new LuaLock();
  
  }
  
  Console::~Console()
    {    
           
    }
  
    void Console::setHalt(int halt)
      {
       
      }
      
      
    
  
    void Console::show(int w, int h, char *fontpath)
    {
      
        
    }

    void Console::hide()
    {
      
    }
    
    //print something out...
    void Console::addLine(char *line, int type)
    {
    
        
    }
    
    
    //update events and draw. Returns 0 if window is to be removed.
    int Console::update()    
    {
       
        return 1;
    }

    
