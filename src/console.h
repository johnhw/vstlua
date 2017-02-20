
#ifndef __console__
#define __console__

#include <stdlib.h>
#include <string.h>


#include "sysfuncs.h"

class VstLua;


#define CONSOLE_LOCK { consoleLock->Lock(); }
#define CONSOLE_UNLOCK { consoleLock->Unlock(); }


class Console
{
    public:
    Console(VstLua *instance);
    ~Console();
    void setHalt(int halt);
    void show(int w, int h, char *fontpath);
    void hide();
    void addLine(char *line, int type);
    
    int update();        
    VstLua *instance;
    protected:
    	
 
    LuaLock *consoleLock;
};


#endif
