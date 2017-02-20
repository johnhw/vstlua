#include "vstlua.h"
#include <stdio.h>


 /* Read the program string */
 int readProgramDataLua(lua_State *state)
 {
    VstLua *instance = getInstance(state);
    lua_pushlstring(state, instance->getCurrentProgram()->dataString, instance->getCurrentProgram()->dataStringLen);
    return 1;           
 }
 
 
 /* write the program string */
 int writeProgramDataLua(lua_State *state)
 {  
    VstLua *instance = getInstance(state);
    
    if(!lua_isstring(state,-1))
        {
            instance->fatal_error("Called writeProgramData with non-string argument!\n");
            return 0;
        }
        
    size_t len;
    const char *str;
    
    str = lua_tolstring(state, -1, &len);
    instance->getCurrentProgram()->setDataString((char*)str, len);
    return 1;           
 
 }
 

 
 
 /* Lock the process shared space */
 int sharedLockLua(lua_State *state)
 {
      VstLua *instance = getInstance(state);
      if(!instance->memoryChannel)
        return 0;
      int result = instance->memoryChannel->lockShared();
      lua_pushnumber(state, result);
      return 1; 
 }
 
 
 /* Unlock the process shared space. Always suceeds */
 int sharedUnlockLua(lua_State *state)
 {
      VstLua *instance = getInstance(state);
      if(!instance->memoryChannel)
        return 0;
      int result = instance->memoryChannel->unlockShared();
      lua_pushnumber(state, result);
      return 1; 
 
 }
 
 
 /* Copy a string into the shared space. Better have locked it first! */
 int sharedWriteLua(lua_State *state)
 {
      VstLua *instance = getInstance(state);
      if(!instance->memoryChannel)
        return 0;
        
      const char *str = lua_tostring(state,-1);
      if(str)
      {
        int result = instance->memoryChannel->writeToShared((char *)str);
        lua_pushnumber(state, result);
        return 1; 
      }
      else
      {
        instance->fatal_error("Called sharedWrite with non-string argument");
        return 0;    
      }
 }
 
 /* Read a string from the shared space. */
 int sharedReadLua(lua_State *state)
 {
       
      VstLua *instance = getInstance(state);
      if(!instance->memoryChannel)
        return 0;        
      
        char *result = instance->memoryChannel->readFromShared();        
        if(result)  
        {
            lua_pushstring(state, result);
            return 1;                  
        }
      else
        return 0;    
     
 }


//return the size of the window
int getSizeLua(lua_State *state)
{
    VstLua *instance = getInstance(state);
    int w = instance->editor->getWidth();
    int h = instance->editor->getHeight();
    
    lua_newtable(state);
    
    setfieldi(state, "width", w);
    setfieldi(state, "height", h);
    
    
    return 1;
}


 int setSizeLua(lua_State *state)
 {
    VstLua *instance = getInstance(state);
    
    if(!lua_istable(state,-1))
        instance->fatal_error("Called guiSetSize with non-table argument!\n");
    else
    {
    
        int w = getfield(state, "width", 0);
        int h = getfield(state, "height", 0);        
        
        instance->editor->setGUISize(w,h);                
    }
    
    return 0;
 
 }
 

 
 
 //replace the passed string with a version with embedded zeros escaped (and embedded \'s)
 int escapeLua(lua_State *state)
 {
    size_t len;
    char *in = (char *)lua_tolstring(state,-1, &len);
    
    if(in)
    {
        len = lua_strlen(state,-1);
        lua_pop(state,1);
        char *out = new char[len*2+1];
        escape(in, len, out, len*2+1);
        lua_pushstring(state, out);
        delete [] out;
        return 1;
    }
    return 0;
 }
 
 //replace the passed string with a version with escaped zeros replaced with embedded ones
 int unescapeLua(lua_State *state)
 {
    char *in = (char *)lua_tostring(state,-1);    
    if(in)
    {
        int len = lua_strlen(state,-1);
        lua_pop(state,1);
        char *out = new char[len+1];
        int realLen = unescape(in, out, len+1);
        lua_pushlstring(state, out, realLen);
        delete [] out;
        return 1;
    }     
    return 0;
 }
 


 
 
  //get this list of remote instances
   int getInstancesLua(lua_State *state)
   {
    VstLua *instance = getInstance(state);
    instance->memoryChannel->pushInstances(state);   
    return 1;
   }
   
   
  //send a message to a remote instance (or indeed this one)
  int sendToInstanceLua(lua_State *state)
  {
    VstLua *instance = getInstance(state);
    if(!lua_isstring(state,1) || !lua_isstring(state,2))
    {
        instance->fatal_error("Called sendToInstance without two strings!\n");
    }
    else
    {        
        instance->memoryChannel->sendToInstance((char*)lua_tostring(state,1),(char*)lua_tostring(state,2),lua_strlen(state,2));        
    }
    
    return 0;
  }
 
 
 //reset the interpreter, optionally take a file to reset to...
 int resetLua(lua_State *state)
 {
    VstLua *instance = getInstance(state);
    if(instance->isInsideReInit())
    {
        instance->fatal_error("Tried to call reset() from inside reset()!\n");
    }
    else
    {
        
        if(lua_isstring(state,-1))        
                instance->preloadScript((char *)lua_tostring(state,-1)) ;       
        else
                instance->preloadScript("");
    }
    return 0;
 }

 

 const int audioMasterSetTime = 9;
 
 
 
 //Set the host time, for those hosts that support it...
 int setHostTimeLua(lua_State *state)
 {
    VstLua *instance = getInstance(state);
    VstTimeInfo info;
    int mask = 0;
    
    
    //no idea if this is even used?
    info.samplePos = getfield(state, "samplePos", 0);
    
    
    //tempo
    info.tempo =  getfieldf(state, "tempo", -1);
    if(info.tempo>0)
        mask |= kVstTempoValid;
    
    
    //time signature
    info.timeSigNumerator =  getfield(state, "timeSigNumerator", -1);
    info.timeSigDenominator =  getfield(state, "timeSigDenominator", -1);
    if(info.timeSigNumerator>0 && info.timeSigDenominator>0)
        mask |= kVstTimeSigValid;
        
    //cycle
    info.cycleStartPos =  getfieldf(state, "cycleStartPos", -1);
    info.cycleEndPos =  getfieldf(state, "cycleEndPos", -1);
    if(info.cycleStartPos>0 && info.cycleEndPos>0)
        mask |= kVstCyclePosValid;
   
    //ppqPos
    info.ppqPos =  getfieldf(state, "ppqPos", -1);    
    if(info.ppqPos>0)
        mask |= kVstPpqPosValid;
        
        
    
    //dispatch    
    int result = instance->dispatcher(audioMasterSetTime, 0, mask, &info, 0.0);
    lua_pushnumber(state, result);
    return 1; 
 }

//Return the host time
int getHostTimeLua(lua_State *state)
{            
        VstTimeInfo *time;
        time = getInstance(state)->getTimeInfo(kVstPpqPosValid | kVstTempoValid | kVstTimeSigValid | kVstNanosValid);            
        lua_newtable(state);
        setfieldf(state, "ppqPos", time->ppqPos);  
        setfieldf(state, "samplePos", time->samplePos);        
        setfieldf(state, "timeSigNumerator", time->timeSigNumerator);
        setfieldf(state, "timeSigDenominator", time->timeSigDenominator);
        setfieldf(state, "tempo", time->tempo);        
        setfieldf(state, "sampleRate", time->sampleRate);        
        setfieldf(state, "nanoSeconds", time->nanoSeconds);                
        setfieldi(state, "playing", time->flags & kVstTransportPlaying);
        setfieldi(state, "recording", time->flags & kVstTransportRecording);                        
        return 1;
}



#ifdef WINDOWS

struct openFileCallbackParms
{
    VstLua *instance;
    lua_State *state;
    CFileSelector *fileSelector;
    int token;
    int uid;
    VstFileSelect selectStruct;
};

    
    //callback executed in another thread for return...
    DWORD WINAPI openFileCallback(LPVOID params)
    {
        openFileCallbackParms *par = (openFileCallbackParms *)params;        
        
        //run the selector
        par->fileSelector->run(&(par->selectStruct));            
        
        //check it's the same instance of lua
        if(par->instance->getNetworkToken()==par->token)
        {                           
                //retrieve the id;
                char uid[512];                
                snprintf(uid, sizeof(uid), "openFileCallback%d", par->uid);
                
                
                par->instance->Lock();
                lua_pushstring(par->state, uid); //push the uid
                lua_gettable(par->state, LUA_REGISTRYINDEX); //get the stored callback
                
                //push the return value
                if(par->selectStruct.nbReturnPath>0)
                {                                                   
                lua_pushstring(par->state, par->selectStruct.returnPath);                                                              
                }
                
                //call the callback
                if(lua_pcall(par->state, 1,0,0))
                {
                    par->instance->fatal_error("Callback from openFileSelector failed!\n");                    
                }             
                
                par->instance->Unlock();
                
        }
        delete par->fileSelector;
        delete par;
        return 0;
    }

#endif


int openFileSelectorLua(lua_State *state)
{
   
    VstLua *instance = getInstance(state);
    
    int callback = 0;
    unsigned int callbackId = rand();
    
    //check if we got a callback
    if(lua_isfunction(state,2))
    {       
        //generate a uniquie id
        char uid[512];                
        snprintf(uid, sizeof(uid), "openFileCallback%d", callbackId);
        lua_pushstring(state, uid);
        lua_pushvalue(state,2); //duplicate the function on top of the stack
        lua_settable(state, LUA_REGISTRYINDEX); //store in the registry
        
        callback = 1;    
    }
    
     //prepare the fileselect structure
     VstFileSelect selectStruct;
    
     selectStruct.command=kVstFileLoad;
     selectStruct.returnPath = NULL;
     
     selectStruct.nbFileTypes=1; 
     
     VstFileType lua;
     if(lua_isstring(state,1))
        {
            const char *str = lua_tostring(state, 1);
            VstFileType custom(str,str,str);
            lua = custom;
        }
        else
        {
            VstFileType def("*.*", "*.*", "*.*");
            lua = def;
        }
     
     
     
     selectStruct.fileTypes=&lua; 
    snprintf(selectStruct.title,  64, "Load...");
    char init_path[512];
    snprintf(init_path, sizeof(init_path), "%s\\*.*",  instance->base_path);    
    selectStruct.initialPath = init_path;
    
    
    //select the file
    CFileSelector *fileSelector = new CFileSelector(instance);
    
    //do it and block
    if(!callback)
    {
     
        fileSelector->run(&selectStruct);    
        if(selectStruct.nbReturnPath>0)
        {           
            lua_pushstring(state, selectStruct.returnPath);     
            delete fileSelector;
         
            return 1;
        }
        else
        {
            instance->normal_output("No path");
            delete fileSelector;
            return 0;
        }
    }
    else
    {
        
        #ifdef WINDOWS
            openFileCallbackParms *par= new openFileCallbackParms();
            par->instance = instance;
            par->state = state;
            par->token = instance->getNetworkToken();
            par->selectStruct = selectStruct;                        
            par->fileSelector = fileSelector;
            par->uid = callbackId;
            HANDLE threadHandle = CreateThread(NULL, 0, openFileCallback, par, 0, 0);
            addHandleToClose(instance->config_state, threadHandle);
        #endif
    
        return 0;
    }
    
}


int networkReceiveLoopLua(lua_State *state)
{
    getInstance(state)->ReceiveFromListenLoop(state);
    return 0;
}


int guiRedrawLua(lua_State *state)
{
       
    VstLua *instance = getInstance(state);
    if(lua_isnumber(state,-1))
    {
        int controlID = (int)lua_tonumber(state, -1);
        instance->editor->luaGUI->redraw(controlID);
    }
    instance->editor->luaGUI->redraw(-1);
    return 0;
}


/*
int recvFromWithListenerLua(lua_State *state)
{
    
    #ifdef WINDOWS
    CreateThread(NULL, 0, recvThread, NULL, 0, 0);
    
    #endif
        
}*/


int getTimeUsLua(lua_State *state)
{
    double usTime = sysGetTimeUs();
    lua_pushnumber(state, usTime);
    return 1;
}


 int getCycleDataLua(lua_State *state)
 {
    
     VstTimeInfo *time;
     
     VstLua *instance = getInstance(state);
     instance->hostLock->Lock();
     
     time = instance->getTimeInfo(kVstCyclePosValid);            
     lua_newtable(state);
      setfieldf(state, "start", time->cycleStartPos);  
      setfieldf(state, "end", time->cycleEndPos);              
    
    instance->hostLock->Unlock();
      return 1; 
 }
 
 int getBarStartLua(lua_State *state)
 {
     VstTimeInfo *time;
     VstLua *instance = getInstance(state);
     instance->hostLock->Lock();
     
     time = instance->getTimeInfo(kVstBarsValid);            
     lua_pushnumber(state, time->barStartPos);        
     instance->hostLock->Unlock();
     return 1;  
 }  

int getVSTPathLua(lua_State *state)
{
    lua_pushstring(state, getInstance(state)->base_path);
    return 1;
   
}

int printLua(lua_State *state)
{    
  
    getInstance(state)->normal_output("%s",(char *)lua_tostring(state, 1));
    return 0;
}


int errorPrintLua(lua_State *state)
{
   
    getInstance(state)->fatal_error("%s",(char *)lua_tostring(state, 1));
    return 0;
}




int getParameterLua(lua_State *state)
{
   
   int par = (int)(lua_tonumber(state, 1));   
   lua_pop(state, 1);
    if(par>=0 && par<MAX_PARMS)
    {
        lua_pushnumber(state, getInstance(state)->getParameter(par));           
        return 1;   
    }
    return 0;
}


int setParameterLua(lua_State *state)
{  

   VstLua *instance = getInstance(state);
   float val = lua_tonumber(state, 2);
   int par = (int)(lua_tonumber(state, 1));   
   instance->hostLock->Lock();
   //Can only set non-reserved parameters with this
   if(par>=0 && par<MAX_PARMS)
   {
    instance->setParameterNoCallback(par, val);     
    instance->setParameterAutomated(par, val); 
    instance->updateDisplay();     
   }
   
   
   lua_pop(state, 2);   
   instance->hostLock->Unlock();
   return 0;
}

int addControlLua(lua_State *state)
{
    VstLua *instance=getInstance(state);
       //Check this really i s a table...
        if(!lua_istable(state, -1))
        {
            instance->fatal_error("Called guiAddControl with a non-table value\n");
            return 0;
        }
   

    int x = getfield(state, "x", -1);
    int y = getfield(state, "y", -1);                         
    int type = getfield(state, "type", -1);
    
    //check the arguments
    if(x==-1 || y==-1)
    {
        instance->fatal_error("Called guiAddControl with bad (x,y) position\n");
        return 0;
    }
    
    
    if(type==-1)
    {
        instance->fatal_error("Called guiAddControl with bad gui type\n");
        return 0;
    }
    
    //get the label
    char label[512];
    lua_pushstring(state, "label");
    lua_gettable(state, -2);  
    
    if(!lua_isstring(state,-1))
        strncpy(label, "", sizeof(label));
    else
        strncpy(label, lua_tostring(state, -1), sizeof(label));
        
    lua_pop(state, 1);
    
   
    MenuEntry *entries=NULL;
    lua_pushstring(state, "menu");
    lua_gettable(state, -2);
        
    if(lua_istable(state, -1))
    {
   
        entries = recursiveParseMenu(state);        

    }
    lua_pop(state,1);
        
    
   instance->hostLock->Lock();     
   //return the id of this control
   if(instance->editor->luaGUI)
   {
        int tag=instance->editor->luaGUI->addControlLua(x,y,type,label);
        lua_pushnumber(state,tag );
        if(entries)
        {
            
            instance->editor->luaGUI->addTreeToMenu(tag,entries);
            delete entries;
        }
        
    }
   
   instance->hostLock->Unlock();
   return 1;
}


MenuEntry *recursiveParseMenu(lua_State *state)
{
    MenuEntry *baseEntry = NULL;
    MenuEntry *head = NULL;
    
    
    //now parse the menu...
    int index = 1;
    int going = 1;
    
    
    
    while(going)
    {
        going = 0;
        lua_pushnumber(state, index);
        lua_gettable(state, -2) ;
        const char *txt;
        char *str;
  
        //plain entry
        if(lua_isstring(state,-1))
        {
            txt = lua_tostring(state, -1);
            str = new char[strlen(txt)+1];
            strcpy(str, txt);
            MenuEntry *nEntry = new MenuEntry(str, NULL, NULL);      

            if(baseEntry)
                    baseEntry->next= nEntry;
            else
                head=baseEntry=nEntry;
            baseEntry = nEntry;
            
                        
            going = 1;        
    
        }
        else if(lua_istable(state,-1))
        {
            
            //read the name index, use it as the name
            lua_pushnumber(state,1);//+
            lua_gettable(state,-2);//-

            //OK, now do the recursive add
            if(lua_isstring(state,-1))
            {
             
                txt = lua_tostring(state, -1); //+

                lua_pop(state, 1);  //-
                
                str = new char[strlen(txt)+1];
                strcpy(str, txt);
                //find the second part (hopefully a table)
                lua_pushnumber(state, 2); //+
                lua_gettable(state, -2); //-
                
                MenuEntry *nEntry;
                //recurse if we have a table, don't otherwise
                if(lua_istable(state,-1))                
                {                               
                    nEntry = new MenuEntry(str, recursiveParseMenu(state), NULL);  //-                  
                }
                else
                {                             
                    nEntry = new MenuEntry(str, NULL, NULL);            
                }
        
                if(baseEntry)
                    baseEntry->next= nEntry;
                else
                    head=baseEntry=nEntry;
                
                baseEntry = nEntry;
            
            
                going = 1;
            }
            else
                lua_pop(state,1); //pop string
            
        }     
        lua_pop(state,1); //pop state        
        index = index + 1;           
    }
    
    return head;
}   
 
int setLabelLua(lua_State *state)
{
 
       VstLua *instance = getInstance(state);
   if(!lua_isnumber(state,1) || !lua_isstring(state,2))
   {
    instance->fatal_error("Called guiSetLabel with invalid parameters!\n");
    return 0;
   }  
   
   int tag = (int)lua_tonumber(state, 1);
   char string[512];
   strncpy(string, lua_tostring(state, 2), 511);
   
   int highlight = 0;
   if(lua_isnumber(state,3))
        highlight= (int)lua_tonumber(state, 3);
        
    
   if(instance->editor->luaGUI)
        instance->editor->luaGUI->setLabelLua(tag, string, highlight);   
   return 0;
}




int getValueLua(lua_State *state)
{
  
     VstLua *instance = getInstance(state);
       //Check this really i s a table...
        if(!lua_isnumber(state, -1))
        {
            instance->fatal_error("Called guiGetValue with non-number!\n");
            return 0;
        }
        
    int tag = (int)lua_tonumber(state, 1);
   if(instance->editor->luaGUI)
        lua_pushnumber(state, instance->editor->luaGUI->getValueLua(tag));
   
   return 1;
}

int setValueLua(lua_State *state)
{

      VstLua *instance = getInstance(state);
      if(!lua_isnumber(state,1) || !lua_isnumber(state,2))
    {
        instance->fatal_error("Called guiSetValue with invalid parameters!\n");
        return 0;
     }
    
    int tag = (int)lua_tonumber(state, 1);
    float value = lua_tonumber(state, 2);
           
   if(instance->editor->luaGUI)
        instance->editor->luaGUI->setValueLua(tag, value);
   
   return 0;
}





 int sendMidiEventLua(lua_State *state)
{    
    int delta, len, offset, data1, data2, data3, data4, detune, noteoff;
    int type, channel;
      VstLua *instance = getInstance(state);
    //Table for midi event must be on top of the stack...

        //Check this really i s a table...
        if(!lua_istable(state, -1))
        {
            instance->fatal_error("Called sendMidi with a parameter that is not a table!\n");
            return 0;
        }
        
        
        delta = getfield(state, "delta", 0);
        type = getfield(state, "type", 0);
        
        //check if this is a sysex
        lua_pushstring(state, "sysex");
        lua_gettable(state,-2);
        if(lua_isstring(state,-1) && type==0xf)
        {
            //deal with the sysex
            size_t len;
            const char *str;
            str = lua_tolstring(state,-1,&len);
            lua_pop(state,1);        
            instance->sendSysex((char*)str, len, delta);
        }
        else
        {    
            lua_pop(state,1);
            len = getfield(state, "noteLength", 0);
            offset = getfield(state, "noteOffset", 0);
            detune = getfield(state, "detune", 0); 
            noteoff = getfield(state, "noteOffVelocity", 0);
            
            channel = getfield(state, "channel", 0);
            
            
            data1 = ((type & 0xf) << 4) | (channel & 0xf);
            data2 = getfield(state, "byte2", 0);
            data3 = getfield(state, "byte3", 0);
            data4 = getfield(state, "byte4", 0);
            instance->sendMidiEvent(delta, len, offset, data1, data2, data3, data4, detune, noteoff);       
            
        }
        lua_pop(state, 1);
    return 0;
}
