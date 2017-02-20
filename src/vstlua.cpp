
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

//#include <lua.hpp>


extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "luasocket-2.0.2/src/socket.h"
#include "luasocket-2.0.2/src/auxiliar.h"
#include "luasocket-2.0.2/src/inet.h"
#include "luasocket-2.0.2/src/options.h"
#include "luasocket-2.0.2/src/udp.h"

#include "luasocket.h"
#include "lbitlib.h"
}
#include "luahash.h"
#include "osclua.h"
#include "VstLua.h"


enum
{
msgNormal=0,
msgError=1
};

/*
extern "C" 
{
FILE *lf=NULL;
static int processID = 202021;

void __cyg_profile_func_enter (void *, void *) __attribute__((no_instrument_function));
void __cyg_profile_func_exit (void *, void *) __attribute__((no_instrument_function));

int depth = -1;

void __cyg_profile_func_enter (void *func,  void *caller)
{
 int n;
  if(!lf)
  {
    lf = fopen("debuglog.txt", "w");
  }  
  
 depth++;
 //for (n = 0; n < depth; n++)
 //  fprintf (lf, " ");
 fprintf (lf,"%p\n", func);
 
 fflush(lf);
}


void __cyg_profile_func_exit (void *func, void *caller)
{
 int n;

if(!lf)
    lf = fopen("debuglog.txt", "w");

// for (n = 0; n < depth; n++)
//   fprintf (lf," ");
 
 //fprintf (lf,"<- %p\n", func);
 
 depth--;
 fflush(lf);
}

}
*/


//halt macros
#define HALT {halted++;if(console)console->setHalt(halted);}
#define UNHALT {halted=0;if(console)console->setHalt(halted);}


#define LUA_LOCK exlock->Lock()
#define LUA_UNLOCK exlock->Unlock()

#define HOST_LOCK hostLock->Lock()
#define HOST_UNLOCK hostLock->Unlock()


VstLua *getInstance(lua_State *state)
{
    lua_pushstring(state, "VSTLUA_INSTANCE");
    lua_gettable(state, LUA_REGISTRYINDEX);
    VstLua *instance = (VstLua*) lua_touserdata(state,-1);
    if(!instance)
    {
        //oh dear, time to die
        RaiseException(0x00771111, 0, 0, NULL);
    }
    lua_pop(state,1);
    return instance;
}


void VstLua::normal_output(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  char line[1024];
  vsnprintf(line, 1024, fmt, args);
  if(editor)
    editor->addLine(line, msgNormal);  
    
  if(console)
    console->addLine(line, msgNormal);
    
  if(logFile)
  {
    vfprintf(logFile, fmt, args);
    fflush(logFile);
  }
  va_end(args);
}


void VstLua::cprint(char *fmt,...)
{
 va_list args;
  va_start(args, fmt);
  char line[1024];
  vsnprintf(line, 1024, fmt, args);
    
  if(console)
    console->addLine(line, msgNormal);
  va_end(args);
}


void VstLua::error_output(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  
  char line[1024];
  vsnprintf(line, sizeof(line),fmt, args);
  
  if(editor)  
    editor->addLine(line, msgError);    
    
  if(console)
    console->addLine(line, msgError);
  
  
  if(logFile)
  {
    fprintf(logFile, "ERROR: ");
    vfprintf(logFile, fmt, args);
  }
  
  fflush(logFile);
  va_end(args);
}

void VstLua::verror_output(char *line)
{

  
  if(editor) 
    editor->addLine(line, msgError);    
  
  
  if(console)
    console->addLine(line, msgError);
  
  
  if(logFile)
  {
    fprintf(logFile, "FATAL ERROR: ");
    fprintf(logFile, line); 
    fflush(logFile);  
   }
  
}


void VstLua::fatal_error(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);  
    char line[1024];
    vsnprintf(line, sizeof(line), fmt, args);  
    va_end(args);
    
    HALT;
    
    if(editor) editor->tripReset(halted);
    
        
    verror_output(line);        
            
}



//copy out a string with zeros replace with escaped versions
void escape(char *withZeros, int len, char *buf, int buflen)
{
   
    int ptr = 0;
    
    for(int i=0;i<len;i++)
    {
        if(ptr<buflen-1)
        {
            if(withZeros[i]=='\\')
            {
                    buf[ptr++]='\\';
                    buf[ptr++]='\\';
            }           
            else if(withZeros[i]=='\0')
            {
                buf[ptr++]='\\';
                buf[ptr++]='0';
            }
            else        
            {
                buf[ptr++]=withZeros[i];                
            }
        }
    }
    
    //terminate string
    buf[ptr] = '\0';
    
}


//unescape a string
int unescape(char *in, char *out, int buflen)
{
    int ptr = 0;    
    int len = strlen(in);    
    int i = 0;
        
    while(i<len && ptr<buflen)
    {
        if(ptr<buflen)
        {
            if(in[i]=='\\' && in[i+1]=='0')
            {
                    out[ptr]='\0';                    
                    i+=2;
            }
            else if(in[i]=='\\' && in[i+1]=='\\')
            {
                out[ptr]='\\';
                i+=2;
                
            }           
            else        
            {
                out[ptr]=in[i];                
                i++;
            }        
        }        
        ptr++;
    }    
    return ptr;
}



//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{

	return new VstLua (audioMaster);
}


//Plugin States
void VstLua::open()
{
    
}
    void VstLua::resume()
    {
    UNHALT;
    }
    void VstLua::suspend()
    {
    HALT;
    }
    void VstLua::close()
    {
    
    }
    
//end plugin states


//request a rest on the next processing frame (since this can't be called from lua, as the interpreter gets closed during the call)
void VstLua::preloadScript(char *name)
{
    if(scriptToLoad)
        delete [] scriptToLoad;
        
    scriptToLoad = new char[strlen(name)+1];
	if (scriptToLoad)
      strcpy(scriptToLoad, name);    
}


void VstLua::initInterpreter(void)
{
  reInit();  
}

#ifdef WINDOWS

struct ReceiveThreadData
{
    char buffer[UDP_DATAGRAMSIZE];
    p_udp udp;
    size_t count, got;      
    int networkToken;
    VstLua *instance;
};

DWORD WINAPI ReceiveLoop(LPVOID buf)
{
    ReceiveThreadData *rtd = (ReceiveThreadData *)buf;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);    
    int err;        
     
    int receiving = 1;
    
    while(receiving)
    {           
        //do the receive
        p_timeout tm = &(rtd->udp)->tm;
        timeout_markstart(tm);
        
        
        err = socket_recvfrom(&(rtd->udp)->sock, rtd->buffer, rtd->count, &(rtd->got), 
                (SA *) &addr, &addr_len, tm);
         
        
        //check if it worked 
        if (err == IO_DONE && rtd->instance->getNetworkToken() == rtd->networkToken) {        
            if(!rtd->instance->halted)
            {
                
        
                rtd->instance->Lock();
                lua_State *L = rtd->instance->getState();
        
        
                //push the data
                lua_getglobal(L, "_networkReceiveCb");
                lua_pushlstring(L, rtd->buffer, rtd->got);
                lua_pushstring(L, inet_ntoa(addr.sin_addr));
                lua_pushnumber(L, ntohs(addr.sin_port));
                
        
                //call the callback
                if(lua_pcall(L,3,0,0))
                {
                    rtd->instance->fatal_error("Error calling _networkReceiveCb %s\n", lua_tostring(L,-1));                
                    receiving = 0;
                }
                else
                {
        
                    //check if we want to stop or not
                    if(lua_isnumber(L,-1))
                        receiving = (int)(lua_tonumber(L,-1));
                }                
        
                rtd->instance->Unlock();
        
            }
            
        } else {
        
        
            if(rtd->instance->getNetworkToken() == rtd->networkToken)
            {
                //stop on error
                rtd->instance->error_output("Network error: %s\n", socket_strerror(err));
            }
        receiving = 0;        
        
        }  
    }   
    
    
    //free the memory
    delete rtd;
    
    return 0;
}
#endif

//start a network receive loop
void VstLua::ReceiveFromListenLoop(lua_State *L)
{
    ReceiveThreadData *rtd = new ReceiveThreadData();    
    
    rtd->udp = (p_udp) auxiliar_checkclass(L, "udp{unconnected}", 1);   
    
    if(!rtd->udp)
    {
        fatal_error("networkReceiveLoop called with a non-UDP object!");
        return;
    }
    rtd->count = (size_t) luaL_optnumber(L, 2, sizeof(rtd->buffer));
    rtd->got = rtd->count;        
    rtd->count = min(rtd->count, sizeof(rtd->buffer));
    rtd->networkToken = getNetworkToken(); // the token forces the loop to stop if we reinit
    rtd->instance = this;
        
    //start the listen thread
    #ifdef WINDOWS
    HANDLE handle = CreateThread(0, 0, ReceiveLoop, rtd, 0, 0);
    addHandleToClose(config_state, handle);
    #endif    
    
}


   void VstLua::lockHost()
   {
    hostLock->Lock();   
   }
   
   
   void VstLua::unlockHost()
   {
    hostLock->Unlock();   
   }

void VstLua::Lock()
{
    exlock->Lock();
}

void VstLua::Unlock()
{
    exlock->Unlock();
}



const char *VstLua::getFromConfig(char *name, const char* def)
{    
    lua_getglobal(config_state, name);
    if(lua_isstring(config_state, -1))
    {
        const char *str = lua_tostring(config_state,-1);        
        lua_pop(config_state, 1);
        return str;
    }
    return def;   
}


double VstLua::getFromConfig(char *name, double def)
{    
    lua_getglobal(config_state, name);
    if(lua_isnumber(config_state, -1))
    {
        double val =  lua_tonumber(config_state, -1);
        lua_pop(config_state, 1);
        return val;
    }
    return def;   
}


int VstLua::getFromConfig(char *name, int def)
{    
    lua_getglobal(config_state, name);
    if(lua_isnumber(config_state, -1))
    {
        int val =  (int)lua_tonumber(config_state, -1);        
        lua_pop(config_state, 1);
        return val;
    }
    return def;   
}





//Handle tab expansion from the console
char *VstLua::consoleTabExpand(char *tab)
{
    LUA_LOCK;
    char *ret = NULL;
    lua_getglobal(lua_state, "_expandConsole");
    lua_pushstring(lua_state, tab);

    if(lua_pcall(lua_state,1,1,0))
    {        
        
    }
    else if(lua_isstring(lua_state,-1))
    {
        ret = tab;
        strncpy(ret, lua_tostring(lua_state,-1),  1024);        
    }
    lua_pop(lua_state,1);
    
    LUA_UNLOCK;
    return ret;
}

//handle a command from the console
void VstLua::consoleCommand(char *cmd)
{
    LUA_LOCK;
    
 
    lua_getglobal(lua_state, "_parseConsole");
    lua_pushstring(lua_state, cmd);
    if(lua_pcall(lua_state,1,1,0))
    {
        cprint("Error: %s\n", lua_tostring(lua_state,-1));
        lua_pop(lua_state,1);    
    }
    
    //print the return value
    if(lua_isstring(lua_state,-1))
    {
        cprint("%s\n", lua_tostring(lua_state,-1));
         lua_pop(lua_state,1);
    }
    LUA_UNLOCK;
     
}


//-------------------------------------------------------------------------------------------------------
VstLua::VstLua (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, NUM_PROGRAMS, MAX_PARMS)	
{
    console = NULL;
    editor = NULL;
    networkToken = 0;    
    insideReInit = 0;
    scriptToLoad = NULL;
    
     //Find the path of this module
    findVSTPath();  

    char path[1024];
    snprintf(path, sizeof(path), "%s\\log.txt", base_path);    
    logFile = fopen(path, "w");             
    
    
    //Create the sync object    
    exlock = new LuaLock();        
    hostLock = new LuaLock();
    //start with callbacks disabled..
    lua_state = NULL;    
    HALT;
    strncpy(current_script, "", sizeof(current_script));    
    

    //now open the config file
    config_state = lua_open();
    luaL_openlibs(config_state);
    
    //try and load the config file
    char cfg_path[512];
    sprintf(cfg_path, "%s\\lua\\config.lua", base_path);
    if(luaL_dofile(config_state, cfg_path))
    {
        normal_output("Could not load the config file: %s\n", lua_tostring(config_state,-1));
    }
    
    programSet = new VstLuaProgramSet();
  
	setNumInputs (getFromConfig("numInputs", 2));		// stereo in
	setNumOutputs (getFromConfig("numOutputs", 2));		// stereo out
	setUniqueID ('lua1');	// identify
    setInitialDelay(0);
    
    if(getFromConfig("processReplacing", 1))
        canProcessReplacing ();
    
    if(getFromConfig("isSynth", 0))
        isSynth ();
        
           
    memoryChannel = new SharedMem(this);
    editor = new LuaEditor(this);
    setEditor((AEffEditor*)editor);       
   console = new Console(this);
   programsAreChunks(true);
 
    setProgram(0);     
    initInterpreter(); 
    
}





VstLua::~VstLua ()
{
    
    closeInterpreter();    
    HALT;
    delete memoryChannel;   
    if(programSet)
        delete programSet;    
        
    
    //Close the global (between scripts) lua state
    freeHandles(config_state);
    lua_close(config_state);

    
    
    if(logFile)
        fclose(logFile);    
    if(console)
        delete console;
    delete exlock;
    delete hostLock;
}

//return the current value
int VstLua::getNetworkToken()
{
    return networkToken;
}

void VstLua::newNetworkToken()
{
    networkToken++;
}

lua_State *VstLua::getState()
{
    return lua_state;
}

  //push the instance pointer
void VstLua::setInstance()
{
    lua_pushstring(lua_state, "VSTLUA_INSTANCE");
    lua_pushlightuserdata(lua_state, (void *)this);
    lua_settable(lua_state, LUA_REGISTRYINDEX);        
}


//Register the C functions Lua can call
void VstLua::registerFunctions()
{
 //Register the functions
    
    lua_pushcfunction(lua_state, printLua);
    lua_setglobal(lua_state, "vprint");
    
    lua_pushcfunction(lua_state, errorPrintLua);
    lua_setglobal(lua_state, "verror");
        
    lua_pushcfunction(lua_state, sendMidiEventLua);
    lua_setglobal(lua_state, "_sendMidi");
                

    
    lua_pushcfunction(lua_state, setParameterLua);
    lua_setglobal(lua_state, "_setVSTParameter");
    
    lua_pushcfunction(lua_state, getParameterLua);
    lua_setglobal(lua_state, "_getVSTParameter");    
        
  
    lua_pushcfunction(lua_state, getVSTPathLua);
    lua_setglobal(lua_state, "getVSTPath");    

    lua_pushcfunction(lua_state, getHostTimeLua);
    lua_setglobal(lua_state, "getHostTime");
    
    lua_pushcfunction(lua_state, addControlLua);
    lua_setglobal(lua_state, "guiAddControl");
    
    
    lua_pushcfunction(lua_state, getValueLua);
    lua_setglobal(lua_state, "guiGetValue");
    
    lua_pushcfunction(lua_state, setValueLua);
    lua_setglobal(lua_state, "guiSetValue");
    
    lua_pushcfunction(lua_state, setLabelLua);
    lua_setglobal(lua_state, "guiSetLabel");
    
    lua_pushcfunction(lua_state, getBarStartLua);
    lua_setglobal(lua_state, "getBarStart");
    
    lua_pushcfunction(lua_state, getCycleDataLua);
    lua_setglobal(lua_state, "getCycle");
    
     lua_pushcfunction(lua_state, openFileSelectorLua);
    lua_setglobal(lua_state, "_openFileSelector");
    
    lua_pushcfunction(lua_state, getTimeUsLua);
    lua_setglobal(lua_state, "getTimeUs");    
        
    lua_pushcfunction(lua_state, guiRedrawLua);
    lua_setglobal(lua_state, "guiRedraw");
    
    lua_pushcfunction(lua_state, networkReceiveLoopLua);
    lua_setglobal(lua_state, "networkReceiveLoop");
    
    lua_pushcfunction(lua_state, toOSCLua);
    lua_setglobal(lua_state, "toOSC");
    
    lua_pushcfunction(lua_state, fromOSCLua);
    lua_setglobal(lua_state, "fromOSC");
    
    lua_pushcfunction(lua_state, getSizeLua);
    lua_setglobal(lua_state, "guiGetSize");
    
    lua_pushcfunction(lua_state, setSizeLua);
    lua_setglobal(lua_state, "guiSetSize");
    
    lua_pushcfunction(lua_state, setHostTimeLua);
    lua_setglobal(lua_state, "setHostTime");
    
    
    lua_pushcfunction(lua_state, resetLua);
    lua_setglobal(lua_state, "reset");    
    
    lua_pushcfunction(lua_state, getInstancesLua);
    lua_setglobal(lua_state, "getInstances");    
    
    lua_pushcfunction(lua_state, sendToInstanceLua);
    lua_setglobal(lua_state, "sendToInstance");    
    
  
	lua_pushcfunction(lua_state, sharedLockLua);
    lua_setglobal(lua_state, "_sharedLock");    
    
    lua_pushcfunction(lua_state, sharedUnlockLua);
    lua_setglobal(lua_state, "_sharedUnlock");    
    
    lua_pushcfunction(lua_state, sharedWriteLua);
    lua_setglobal(lua_state, "_sharedWrite");    
    
    lua_pushcfunction(lua_state, sharedReadLua);
    lua_setglobal(lua_state, "_sharedRead");    
       
     lua_pushcfunction(lua_state, escapeLua);
    lua_setglobal(lua_state, "escape");   
     
    lua_pushcfunction(lua_state, unescapeLua);
    lua_setglobal(lua_state, "unescape");   
    
    lua_pushcfunction(lua_state, hashLua);
    lua_setglobal(lua_state, "hash");   
  
    lua_pushcfunction(lua_state, readProgramDataLua);
    lua_setglobal(lua_state, "readProgramData");   
    
    lua_pushcfunction(lua_state, writeProgramDataLua);
    lua_setglobal(lua_state, "writeProgramData");   
    
    
    //bitmaps stuff
    lua_pushcfunction(lua_state, loadBitmapLua);
    lua_setglobal(lua_state, "loadBitmap");   
    
    lua_pushcfunction(lua_state, getBitmapSizeLua);
    lua_setglobal(lua_state, "getBitmapSize");   
    
    //native midi access
    lua_pushcfunction(lua_state, getMidiDeviceNamesLua);
    lua_setglobal(lua_state, "_getMidiDeviceNames");
    
    lua_pushcfunction(lua_state, openMidiDeviceLua);
    lua_setglobal(lua_state, "_openMidiDevice");
    
    lua_pushcfunction(lua_state, closeMidiDeviceLua);
    lua_setglobal(lua_state, "_closeMidiDevice");
    
    lua_pushcfunction(lua_state, cleanUpMidiLua);
    lua_setglobal(lua_state, "_cleanUpMidi");
    
	lua_pushcfunction(lua_state, sendNativeMidiLua);
    lua_setglobal(lua_state, "_sendNativeMidi");
    
	lua_pushcfunction(lua_state, sendNativeSysexLua);
    lua_setglobal(lua_state, "_sendNativeSysex");
    	
        
                
}


void VstLua::closeInterpreter()
{
    
    LUA_LOCK;
    if(lua_state!=NULL)   
    {    
    
        
        //allow the previous instance to clean up
        onReset();
        
        freePointers(lua_state);
        
        
        //make sure the midi devices are all closed.
        cleanUpMidiLua(lua_state);
        
        //give network thread chance to close down...
        sysSleep(30);
        lua_close(lua_state);                
    }    
    lua_state = NULL;
    LUA_UNLOCK;
}

//Find the VSTpath and copy it into base_path
void VstLua::findVSTPath()
{

    sysGetCurrentPath(base_path);
    
}



VstLuaProgram *VstLua::getCurrentProgram()
{

    return &(programSet->programs[curProgram]);
}

void VstLua::setLuaPath(void)
{

     //Set the LUA_PATH
    char pathString[2048];
    snprintf(pathString,sizeof(pathString), "?;?.lua;%s\\lua\\?;%s\\lua\\?.lua;%s\\lua\\lib\\?;%s\\lua\\lib\\?.lua;%s\\lua\\lib\\stdlib\\modules\\?.lua;%s\\scripts\\?.lua;%s\\scripts\\?;%s\\lua\\lib\\sockets\\?;%s\\lua\\lib\\sockets\\?.lua",base_path, base_path, base_path, base_path, base_path, base_path, base_path, base_path, base_path);
    
    lua_pushstring(lua_state, pathString);
    lua_setglobal(lua_state, "LUA_PATH");        
    
}

void VstLua::initScript(void)
{

    char lua_init_file[1024];           
    //Find the initialisation script
    snprintf(lua_init_file, sizeof(lua_init_file), "%s\\lua\\init.lua",  base_path);    
    
    //Initialise the script
    if(luaL_dofile(lua_state, lua_init_file))    
    {
        if(lua_isstring(lua_state, -1))
        {            
            fatal_error("Failure in initialisation script:%s\n" , lua_tostring(lua_state, -1));                      
        }        
    }
        
    //Set up globals
    lua_pushstring(lua_state, current_script);
    lua_setglobal(lua_state, "ScriptName");       
    
    lua_pushstring(lua_state, programSet->programs[curProgram].name);
    lua_setglobal(lua_state, "VSTProgramName");    
    
                         
    
    if(strlen(current_script)>0)
    {
        if(luaL_dofile(lua_state, current_script))    
            fatal_error("Failure in loading %s: %s\n", current_script, lua_tostring(lua_state, -1));            
        else
        {
            
            editor->setScriptName(current_script);
        }
        
    }
    else    
    {
        editor->setScriptName("No script");        
    }
    
             
    
    
}




VstInt32 VstLua::getChunk (void** data, bool isPreset)
{   

    HOST_LOCK;
    //make sure program is up to date
    

    if(!halted)
        onSaveProgram();

    
    if (isPreset)
    {   
        
        
        
        //Include the length of the string block
        int len = sizeof (VstLuaProgram)+programSet->programs[curProgram].dataStringLen;
        int offset=sizeof (VstLuaProgram);
        *data = malloc(len);
        
        //clean up later
        LUA_LOCK;
        addPointerToFree(lua_state, *data);
        LUA_UNLOCK;
        
        memcpy(*data, &(programSet->programs[curProgram]), offset);
        //copy, with the zero on
        memcpy((char*)(*data)+offset, programSet->programs[curProgram].dataString, programSet->programs[curProgram].dataStringLen);        
        HOST_UNLOCK;
        return len;
    }
    else
    {   
        int block_len = 0;
        //compute the size of the trailing block;
        for(int i=0;i<NUM_PROGRAMS;i++)
            block_len += programSet->programs[i].dataStringLen;
        
        //make sure the magic number is set
        programSet->version = BANK_MAGIC;
        
        //allocate big enough block
        int total_len = sizeof(VstLuaProgramSet)+block_len;            
        *data = malloc(total_len);
        
        
        if(!(*data))
        {        
        HOST_UNLOCK;
         return 0;
        }

        LUA_LOCK;
        addPointerToFree(lua_state, *data);
        LUA_UNLOCK;
        
        
        //copy in the static data
        memcpy(*data, programSet, sizeof(VstLuaProgramSet));
        
        
        //then the dynamic strings
        int offset = sizeof(VstLuaProgramSet);
        //append each of the data strings
        for(int i=0;i<NUM_PROGRAMS;i++)
        {            
            memcpy((char*)(*data)+offset, programSet->programs[i].dataString, programSet->programs[i].dataStringLen);            
            offset += programSet->programs[i].dataStringLen;
        }
               
        HOST_UNLOCK;
        return total_len;
    }
}


VstInt32 VstLua::setChunk (void* data, VstInt32 byteSize, bool isPreset)
{

    
    if (isPreset)
    {
        //check it's a possible match...
        if(byteSize<sizeof(VstLuaProgram))
        {
                
            return 0;   
            
        }
        VstLuaProgram prog = *(VstLuaProgram*)data;               
        //Check the magic number
        if(prog.version!=PROGRAM_MAGIC)
        {
            fatal_error("Bad magic number in program\n");
            return 0;
        }
                
        programSet->programs[curProgram] = *(VstLuaProgram*)data;        
                        
        //get string remainder
        int rem = byteSize -sizeof(VstLuaProgram);
        if(rem!=programSet->programs[curProgram].dataStringLen)
            {
            fatal_error("Failure in program setting; chunk sizes don't match! Have %d bytes remaining, but dataStringLen is %d\n", rem, programSet->programs[curProgram].dataStringLen);       
            return 0;
            }
        programSet->programs[curProgram].setDataString((char *)data+sizeof(VstLuaProgram), rem);                
    }
    else
    {
        
        if (byteSize < sizeof (VstLuaProgramSet))
            return 0;
        
        //first part is fixed...
        VstLuaProgramSet *pSet = new VstLuaProgramSet(*(VstLuaProgramSet*)data);
        
        
        //check this is the right magic number
        if(pSet->version!=BANK_MAGIC)
        {
            fatal_error("Bad magic number in bank setting\n");
            delete pSet;
            return 0;
        }
            
        programSet = pSet;
        
        //clear pointers
        for(int i=0;i<NUM_PROGRAMS;i++)
            programSet->programs[i].dataString = NULL;
        
        
        int block_len = 0;
        //compute the size of the trailing block;
        for(int i=0;i<NUM_PROGRAMS;i++)
            block_len += programSet->programs[i].dataStringLen;
                
        if(byteSize-sizeof(VstLuaProgramSet)!=block_len)
        {
            fatal_error("Failure in bank setting; chunk sizes don't match!\n");
            return 0;
        }
        
        //then the dynamic strings
        int offset = sizeof(VstLuaProgramSet);
        //append each of the data strings
        for(int i=0;i<NUM_PROGRAMS;i++)
        {
            //Copy it in  (the zero is taken care of automatically, hence no + 1)
            programSet->programs[i].setDataString((char*)data+offset, programSet->programs[i].dataStringLen);            
            offset += programSet->programs[i].dataStringLen;
        }            
                
    }    
   
    setProgramNoSave(curProgram);
    return 1;
}





void VstLua::reInit(void)
{    
    LUA_LOCK;        
    ENTER_REINIT;
    HALT;


    //make sure previous network threads don't try and call this callback...
    newNetworkToken();
   
    closeInterpreter();
    
    //set halt counter to one
    halted = 1;           
    //Create the LUA state
    lua_state = lua_open();    
    

    setInstance();    

    editor->tripReset(0);            
    setLuaPath();
    //Open the libraries
    luaL_openlibs(lua_state);
    
    luaopen_socket_core(lua_state);
    luaopen_bit(lua_state);          
    luaopen_random(lua_state);
    luaopen_struct(lua_state);
    luaopen_vstgui(lua_state);
    registerFunctions();                                  
    normal_output("--== VstLua %s / tzec 2007 ==--\n", VERSION);    
    
    
    
    if(editor)
        editor->luaGUI->setGUISizes();
    initScript();
    //reset the GUI manager
    
    
    if(halted)
    {
        if(editor)
            editor->resetGUI();
        
        if(halted)
        {
            //Call the reinit function
            lua_getglobal(lua_state, "_initCb");     
                  
            if(lua_pcall(lua_state, 0, 0, 0))        
                fatal_error("Init callback failed: %s\n", lua_tostring(lua_state,-1));                                
        }
        
        
       //restart the interpreter , but only if the preceding didn't halt
        if(halted)
        {
            UNHALT;           
        }
        else
        {
            //make sure the failure is visible
            editor->tripReset(1);        
        }
    }
    
    LEAVE_REINIT;   
    LUA_UNLOCK;       
}


void VstLua::onReset()
{

    LUA_LOCK;
    
    lua_getglobal(lua_state, "_resetCb");
    if(lua_pcall(lua_state,0,0,0))
    {
        fatal_error("_resetCb failed: %s\n", lua_tostring(lua_state, -1));    
    }
    
    LUA_UNLOCK;
    
}



/* Set the current script in use */
void VstLua::setScript(char *script)
{
    FILE *input;
    int exists = 0;
    HALT;
    
    //Check the file is there
    input = fopen(script, "r");
    if(input)
    {
        exists = 1;
        fclose(input);
    }
    
    if(exists)
    {
        strncpy(current_script, script, sizeof(current_script));
    }      
    else
    {
        strncpy(current_script, "", sizeof(current_script));
    }
    
    reInit();                   
}

//return true if the system is currently inside a reinit (to avoid recursive calls to reinit which will crash everything!)
int VstLua::isInsideReInit()
{
    return insideReInit;
}


//Update the console
int VstLua::updateConsole()
{
    if(console)
    {    
      return  console->update();
    }
    else
        return 1;

}



//Set the size of gui component
void VstLua::setGUISize(int x, int y, int guiID)
{


    LUA_LOCK;
    lua_getglobal(lua_state, "GUISizes");
    
 
    //create it if it doesn't exist
    if(!lua_istable(lua_state,-1))
    {
        lua_pop(lua_state,1);
        lua_newtable(lua_state);        
    }
 
    //index
    lua_pushnumber(lua_state, guiID);
  
    
    
    //create the x, y table
    lua_newtable(lua_state);
    setfieldi(lua_state, "width", x);
    setfieldi(lua_state, "height", y);
        
    //set the x,y
    lua_settable(lua_state, -3);
    
    lua_setglobal(lua_state, "GUISizes");
    
    LUA_UNLOCK;
    
    
}

/* Lua callbacks */


//Help button pressed
char *VstLua::onHelp()
{
    LUA_LOCK;
    if(!halted)
    {
        lua_getglobal(lua_state, "_helpCb");
        if(lua_pcall(lua_state, 0,1,0))
        {
            fatal_error("Failure in helpCb: %s\n", lua_tostring(lua_state,-1));
            LUA_UNLOCK;
            return NULL;
        }   
        
        if(!lua_isstring(lua_state,-1))
        {
            fatal_error("helpCb should return a string!\n", lua_tostring(lua_state,-1));
            LUA_UNLOCK;
            return NULL;
        }
        
        LUA_UNLOCK;
        return (char *)lua_tostring(lua_state,-1);
        
    }
    LUA_UNLOCK;
    return NULL;
}

//Shared memory message arrived
void VstLua::messageCallback(char *name, char *msg, int len)
{
    LUA_LOCK;
    if(!halted && lua_state)
    {
        
        lua_getglobal(lua_state, "_messageCb");
        lua_pushstring(lua_state, name);
        lua_pushlstring(lua_state, msg, len);
        if(lua_pcall(lua_state, 2,0,0))
        {
            fatal_error("Failure in messageCb: %s\n", lua_tostring(lua_state,-1));
        }            
    }
    LUA_UNLOCK;
}


//User pressed a key
bool VstLua::onKey(VstKeyCode &code, int isDown)
{
    LUA_LOCK;
    if(!halted)
    {
    
        //special handler for shift-escape released
        if(code.virt==VKEY_ESCAPE && code.modifier&MODIFIER_SHIFT)
        {
            //only reset when it's released
            if(!isDown)
                reInit();
            return true;
        }
        
        
        lua_getglobal(lua_state, "_keyCb");
        lua_newtable(lua_state);
        
        setfieldi(lua_state, "down", isDown);
        setfieldi(lua_state, "virtual", code.virt);
        setfieldi(lua_state, "character", code.character);
        setfieldi(lua_state, "modifier", code.modifier);
        
        if(lua_pcall(lua_state, 1,1,0))
        {
            fatal_error("Failure in _onKey: %s\n", lua_tostring(lua_state,-1));
            LUA_UNLOCK;
            return false;
        }
        //Function must return false if it didn't use the key, true if it did
        else
        {    
            bool result = (bool)lua_tonumber(lua_state, -1);
            LUA_UNLOCK;
            return result;
        }    
    }
    LUA_UNLOCK;
    return false;
}
   
   
   /* drag drop handler */
   void VstLua::onDrop(CDragContainer *drag)
   {
   LUA_LOCK;
    if(!halted)
    {
        lua_getglobal(lua_state, "_dropCb");
        long int size, type;
        char *str;  

        // get the data 
        str = (char *)drag->first(size, type);
        lua_newtable(lua_state);
        
        int index = 1;
        for(int i=0;i<drag->getCount();i++)
        {                        
            lua_pushnumber(lua_state, index);
            lua_newtable(lua_state);
            setfieldi(lua_state, "type", type);
            setfields(lua_state, "value", str);
            lua_settable(lua_state, -3); //store in main table            
            index++;
                        
            if(i<drag->getCount()-1)
                str = (char *)drag->next(size, type); //get next value
        }
        
        if(lua_pcall(lua_state, 1,0,0))
        {
            fatal_error("Failure in _onDrop: %s\n", lua_tostring(lua_state,-1));
        }
        
     }
     
     LUA_UNLOCK;
   }
    

void VstLua::onValueChanged(int tag, float value, char *str)
{            
        
            LUA_LOCK;
            if(!halted)
            {
                lua_getglobal(lua_state, "_valueChangedCb");
                lua_pushnumber(lua_state, tag);
                lua_pushnumber(lua_state, value);          
                int args;
                if(str)
                {
                    args = 3;
                    lua_pushstring(lua_state, str);
                }
                else
                {
                    args = 2;
                }
                if(lua_pcall(lua_state, args, 0, 0))
                {
                    fatal_error("Failure in valueChangedCb: %s\n", lua_tostring(lua_state, -1));
                
                }
            }
            LUA_UNLOCK;
        
}



//Called just before program saved
void VstLua::onSaveProgram(void)
{
    
    LUA_LOCK;
    lua_getglobal(lua_state, "_saveProgramCb");
    if(lua_pcall(lua_state, 0,0,0))
        fatal_error("Save callback failed: %s\n", lua_tostring(lua_state,-1));                    
    LUA_UNLOCK;
    
}


//Called just after program saved
void VstLua::onLoadProgram(void)
{
    
    
    LUA_LOCK;
  
  
    lua_pushstring(lua_state, programSet->programs[curProgram].name);
    lua_setglobal(lua_state, "VSTProgramName");
    
        lua_getglobal(lua_state, "_loadProgramCb");        
        if(lua_pcall(lua_state, 0,0,0))
            fatal_error("Load program callback failed: %s\n", lua_tostring(lua_state,-1));           
  
    LUA_UNLOCK;
    
    
}

//Called when the window is opened -- all control creation should go here
void VstLua::onOpened(CViewContainer *container)
{
    
    LUA_LOCK;
        
        //set the view container
        pushCViewContainerLua(lua_state, container);
        lua_setglobal(lua_state, "viewContainer");
    
        lua_getglobal(lua_state, "_openCb");
        if(lua_pcall(lua_state, 0, 0, 0))        
                fatal_error("Open callback failed: %s\n", lua_tostring(lua_state,-1));        

    LUA_UNLOCK;
    
    
}

void VstLua::onClosed()
{
    
    LUA_LOCK;
        lua_getglobal(lua_state, "_closeCb");
        if(lua_pcall(lua_state, 0, 0, 0))        
                fatal_error("Close callback failed: %s\n", lua_tostring(lua_state,-1));   
            
    LUA_UNLOCK;
    
}




//Called on each frame
void VstLua::onFrame(int frameLength)
{   
    LUA_LOCK;
    //Load any waiting scripts
    if(scriptToLoad)
    {
        setScript(scriptToLoad);
        delete scriptToLoad;
        scriptToLoad=NULL;        
    }
 
    
    
    if(!halted )
    {
        lua_pushnumber(lua_state, frameLength);
        lua_setglobal(lua_state, "VSTFrameLength");
        
        lua_pushnumber(lua_state, updateSampleRate());
        lua_setglobal(lua_state, "VSTSampleRate");
        
        lua_pushnumber(lua_state, sysGetTimeUs());
        lua_setglobal(lua_state, "VSTFrameStartUs");
        
        
        lua_getglobal(lua_state, "_onFrameCb");        
        
        if(lua_pcall(lua_state, 0, 0, 0))
        {
            fatal_error("onFrame callback failed:%s\n", lua_tostring(lua_state, -1));
        }
     
    }
    LUA_UNLOCK;
    
    
}


//Native Midi event recieved
void VstLua::recvNativeMidiEvent(int port_id, unsigned char data1,unsigned char data2, unsigned char data3, int sysexLen, char *sysexData)
{

	//NB must compute delta!
	int delta = 0;
	
    int channel, type;
    type = (((unsigned char)data1 >> 4)& 0xf);
    channel = ((unsigned char)data1 & 0xf);    
    LUA_LOCK;   
    if(!halted)
    {
    
        editor->tripMidiIn();
        lua_getglobal(lua_state, "_nativeMidiEventCb");
        lua_newtable(lua_state);
          
        setfieldi(lua_state, "delta", delta);
        setfieldi(lua_state, "port_id", port_id);
        setfieldi(lua_state, "noteLength", 0);
        setfieldi(lua_state, "noteOffset", 0);
        setfieldi(lua_state, "type", type);
        setfieldi(lua_state, "channel", channel);
        setfieldi(lua_state, "detune", 0);
        setfieldi(lua_state, "noteOffVelocity", 0);
        setfieldi(lua_state, "byte2", data2);
        setfieldi(lua_state, "byte3", data3);
        setfieldi(lua_state, "byte4", 0);
            
        
        //set the sysex things
        if(sysexLen)
        {
            setfieldi(lua_state, "type", 0xf);
            lua_pushstring(lua_state, "sysex");
            lua_pushlstring(lua_state, sysexData, sysexLen);
            lua_settable(lua_state,-3);            
        }
        else //not a sysex
        {
            setfieldi(lua_state, "type", type);                               
        }
        
        
        //Call the lua function
        if(lua_pcall(lua_state, 1, 0, 0))
        {
            fatal_error("Error in native MIDI callback: %s", lua_tostring(lua_state, -1));       
        }
    }       
    LUA_UNLOCK;
    
}


//Midi event received
void VstLua::recvMidiEvent(VstInt32 delta, VstInt32 len, VstInt32 offset, 
char data1, char data2, char data3, char data4, char detune, char noteOff, int sysexLen, char *sysexData)
{

    int channel, type;
    type = (((unsigned char)data1 >> 4)& 0xf);
    channel = ((unsigned char)data1 & 0xf);    
    LUA_LOCK;   
    if(!halted)
    {
    
        editor->tripMidiIn();
        lua_getglobal(lua_state, "_midiEventCb");
        lua_newtable(lua_state);
          
        setfieldi(lua_state, "delta", delta);
        
        setfieldi(lua_state, "noteLength", len);
        setfieldi(lua_state, "noteOffset", offset);
        setfieldi(lua_state, "type", type);
        setfieldi(lua_state, "channel", channel);
        setfieldi(lua_state, "detune", detune);
        setfieldi(lua_state, "noteOffVelocity", noteOff);
        setfieldi(lua_state, "byte2", data2);
        setfieldi(lua_state, "byte3", data3);
        setfieldi(lua_state, "byte4", data4);
    
        
        
        //set the sysex things
        if(sysexLen)
        {
            setfieldi(lua_state, "type", 0xf);
            lua_pushstring(lua_state, "sysex");
            lua_pushlstring(lua_state, sysexData, sysexLen);
            lua_settable(lua_state,-3);            
        }
        else //not a sysex
        {
            setfieldi(lua_state, "type", type);                    
            
        }
        
        
        //Call the lua function
        if(lua_pcall(lua_state, 1, 0, 0))
        {
            fatal_error("Error in MIDI callback: %s", lua_tostring(lua_state, -1));       
        }
    }       
    LUA_UNLOCK;
    
}


void VstLua::setParameterNoCallback(VstInt32 index, float value)
{

   HOST_LOCK;
   if(index<MAX_PARMS)
    {
        params[index] = value;
        programSet->programs[curProgram].VSTLUA[index]=value;
    
    }
    
   HOST_UNLOCK;
 
}


//Parameter changed externally
void VstLua::setParameter (VstInt32 index, float value)
{
    setParameterNoCallback(index, value);
    
    LUA_LOCK;
      if(!halted)
         {
         
            
            if(index<MAX_PARMS)
            {                    
                     //Call the setparameter callback
                     lua_getglobal(lua_state, "_setParameterCb");                            
                     lua_pushnumber(lua_state, index);
            } 
                        
             lua_pushnumber(lua_state, value);
             if(lua_pcall(lua_state, 2, 0, 0))
             {
                fatal_error("setParameter callback failed: %s\n", lua_tostring(lua_state,-1));
             }     
     }         
     LUA_UNLOCK;
}






/* End Lua callbacks */



/* Send a midi event to the host */
void VstLua::sendMidiEvent(VstInt32 delta, VstInt32 len, VstInt32 offset, 
char data1, char data2, char data3, char data4, char detune, char noteOff)
{
        HOST_LOCK;
        VstMidiEvent midiEvent;
        VstEvents eventList;

         editor->tripMidiOut();
         midiEvent.type = kVstMidiType;
         midiEvent.byteSize = sizeof(midiEvent);
         midiEvent.detune = detune;
         midiEvent.flags = 0;
         midiEvent.noteLength = len;
         midiEvent.noteOffset = offset;
         midiEvent.noteOffVelocity = noteOff;
         midiEvent.reserved1 = 0;
         midiEvent.reserved2 = 0;
         midiEvent.deltaFrames = delta;
         midiEvent.midiData[0] = data1; 
         midiEvent.midiData[1] = data2; 
         midiEvent.midiData[2] = data3;         
         midiEvent.midiData[3] = data4;        
         eventList.events[0] =(VstEvent *) &midiEvent; 
         eventList.numEvents = 1;
         
         
         sendVstEventsToHost(&eventList);           
         HOST_UNLOCK;
         
}


/* Send a sysex message to the host */
void VstLua::sendSysex(char *str, int len, int delta)
{


    HOST_LOCK;
    VstMidiSysexEvent event;
    VstEvents eventList;
    
    editor->tripMidiOut();
    event.type=kVstSysExType;
    event.byteSize = sizeof(event);
    event.deltaFrames = delta;
    event.flags = 0;
    event.dumpBytes = len;
    event.resvd1=0;
    event.resvd2=0;    
    
    char *strcopy = (char *)malloc(len);
    memcpy(strcopy, str, len);
    event.sysexDump = strcopy;
    eventList.events[0] = (VstEvent *) &event;
    eventList.numEvents =1 ;
    sendVstEventsToHost(&eventList);
    
    //we can't free strcopy, because it's used later on by receiving events.
    //so we'll just have to store a pointer to it in the lua_state and 
    //free it when we reset         
    addPointerToFree(lua_state, strcopy);        
    HOST_UNLOCK;
}

VstInt32 VstLua::processEvents(VstEvents *events)
{    

    for(int i=0;i<events->numEvents;i++)
    {
        processEvent(events->events[i]);
    }
    
        
    return 1;
}

//Dispatch midi events to the midi handler
VstInt32 VstLua::processEvent(VstEvent *event)
{
    
    
    
    VstInt32 dFrames;
    dFrames = event->deltaFrames;
    
       
    if(event->type==kVstMidiType)
    {
        VstMidiEvent *midiEvent = (VstMidiEvent *) event;
        recvMidiEvent(dFrames, midiEvent->noteLength, midiEvent->noteOffset, midiEvent->midiData[0], 
        midiEvent->midiData[1], midiEvent->midiData[2], midiEvent->midiData[3], 
        midiEvent->detune, midiEvent->noteOffVelocity,0,NULL);
    }

   
    if(event->type==kVstSysExType)
    {
        
		VstMidiSysexEvent *sysexEvent = (VstMidiSysexEvent *)event;
		recvMidiEvent(sysexEvent->deltaFrames, 0, 0, 0, 0, 0, 0,0,0, sysexEvent->dumpBytes, sysexEvent->sysexDump);
		
   
    }  
        
    return 1;
        
}





/* VST boilerplate ... */

VstInt32 VstLua::getNumMidiInputChannels()
{
    return getFromConfig("midiInChannels", 16);
    
}


VstInt32 VstLua::getNumMidiOutputChannels()
{
    return getFromConfig("midiOutChannels", 16);    
}


VstInt32 VstLua::canDo(char *text)
{
 
    lua_getglobal(config_state, "canDo");
    
    
    //default
    if(!lua_istable(config_state, -1))
    {
        lua_pop(config_state, 1);
        if(!strcmp(text,"receiveVstMidiEvent"))
            return 1;
        if(!strcmp(text,"receiveVstMidiEvents"))
            return 1;
        
        if(!strcmp(text,"receiveVstEvents"))
            return 1;
            
        if(!strcmp(text,"receiveVstEvent"))
            return 1;
        
        if(!strcmp(text,"receiveVstTimeInfo"))
            return 1;
        
        if(!strcmp(text,"sendVstTimeInfo"))
            return 1;
        
        if(!strcmp(text,"sendVstEvents"))
            return 1;
            
        if(!strcmp(text,"sendVstEvent"))
            return 1;
        
            
        if(!strcmp(text,"sendVstMidiEvent"))
            return 1;    

        if(!strcmp(text,"sendVstMidiEvents"))
            return 1;     
    }
    else
    {
        int index = 1;
        const char *str;
        do
        {
            //get the subscript    
            lua_pushnumber(config_state, index);
            lua_gettable(config_state, -2);
            str = lua_tostring(config_state, -1);            
            //check if it matches the input
            if(str)
            {                
                if(!strcmp(text, str))
                {
                    //pop table and string
                    lua_pop(config_state,2);                    
                    return 1;
                }                    
            }            
            lua_pop(config_state,1);
            index++;            
        } while(str!=NULL);                
    }            
    return -1;
}


float VstLua::getParameter (VstInt32 index)
{
    
	return params[index];
}




void VstLua::getParameterName (VstInt32 index, char* label)
{
    
    char label_text[64];
    if(index<MAX_PARMS)
        snprintf(label_text, 64, "LUA %03d",  index);
        
	vst_strncpy (label, label_text, kVstMaxParamStrLen);
    
}

void VstLua::getParameterDisplay (VstInt32 index, char* text)
{
    
	float2string ((float)(params[index]), text, kVstMaxParamStrLen);
}


void VstLua::getParameterLabel (VstInt32 index, char* label)
{
    
	vst_strncpy (label, "", kVstMaxParamStrLen);
}


bool VstLua::getEffectName (char* name)
{
    
	vst_strncpy (name, "VST MIDI LUA", kVstMaxEffectNameLen);
	return true;
}


bool VstLua::getProductString (char* text)
{
    
	vst_strncpy (text, "LUA", kVstMaxProductStrLen);
	return true;
}


bool VstLua::getVendorString (char* text)
{
    
	vst_strncpy (text, "tzec Productions", kVstMaxVendorStrLen);
	return true;
}

VstInt32 VstLua::getVendorVersion ()
{ 
    
	return 1000; 
}


void VstLua::setSampleRate (float sampleRate)
{
    
    HOST_LOCK;
	AudioEffectX::setSampleRate (sampleRate);
    HOST_UNLOCK;
    
}


void VstLua::setBlockSize (VstInt32 blockSize)
{
    
    HOST_LOCK;
	AudioEffectX::setBlockSize (blockSize);
    HOST_UNLOCK;
}


bool VstLua::getOutputProperties (VstInt32 index, VstPinProperties* properties)
{

    
    if (index < 2)
	{
		vst_strncpy (properties->label, "LUA ", 63);
		char temp[11] = {0};
		int2string (index + 1, temp, 10);
		vst_strncat (properties->label, temp, 63);

		properties->flags = kVstPinIsActive;
		if (index < 2)
			properties->flags |= kVstPinIsStereo;	// make channel 1+2 stereo
            
    
		return true;
	}
        
    
	return false;
}

void VstLua::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{    
    
    onFrame(sampleFrames);
        
}

/* Program stuff */
void VstLua::setProgramName (char* name)
{	   
    
    vst_strncpy (programSet->programs[curProgram].name, name, kVstMaxProgNameLen);
    
}

void VstLua::getProgramName (char* name)
{
    
	vst_strncpy (name, programSet->programs[curProgram].name, kVstMaxProgNameLen);
    
}


bool VstLua::getProgramNameIndexed(int category, int index, char *name)
{
    
    
    if (index>=0 && index < NUM_PROGRAMS)
	{
		vst_strncpy(name, programSet->programs[index].name, kVstMaxProgNameLen); 
    
		return true;
	}
    
    
    return false;
}


int VstLua::getProgram()
{
    
    return curProgram;
}



void VstLua::setProgramNoSave(int program)
{
    HOST_LOCK;
    if(program<0 || program>=NUM_PROGRAMS)
    {
        HOST_UNLOCK;
        return;        
    }
        
        
    VstLuaProgram *p = &(programSet->programs[program]);
    curProgram = program;
            
    for(int i=0;i<MAX_PARMS;i++)
    {
        setParameter(i, p->VSTLUA[i]);        
        
    }
        
    if(!halted)
    {
        
        LUA_LOCK;
        lua_pushstring(lua_state, programSet->programs[curProgram].name);
        lua_setglobal(lua_state, "VSTProgramName");    
        LUA_UNLOCK;
        onLoadProgram();      
    }
    
    HOST_UNLOCK;
}




void VstLua::setProgram(int program)
{
    HOST_LOCK;
    if(program<0 || program>=NUM_PROGRAMS)
    {
        HOST_UNLOCK;
        return;        
    }
        
    
    if(!halted)
        onSaveProgram();
        
    
    VstLuaProgram *p = &(programSet->programs[program]);
    curProgram = program;
            
    for(int i=0;i<MAX_PARMS;i++)
    {
        setParameter(i, p->VSTLUA[i]);        
        
    }
        
    if(!halted)
    {
        
        LUA_LOCK;
        lua_pushstring(lua_state, programSet->programs[curProgram].name);
        lua_setglobal(lua_state, "VSTProgramName");    
        LUA_UNLOCK;
        onLoadProgram();      
    }
    
    HOST_UNLOCK;
}



/* end program stuff */

