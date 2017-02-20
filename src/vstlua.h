
#ifndef __VstLua__
#define __VstLua__

#include "public.sdk/source/vst2.x/audioeffectx.h"


#define NUM_PROGRAMS 64
//#include <lua.hpp>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

//prototypes for external libraries
LUALIB_API int luaopen_random(lua_State *L);
LUALIB_API int luaopen_struct (lua_State *L) ;

#include "luahash.h"
}
#include <windows.h>
#include "sysfuncs.h"
#include "sharedmem.h"
#include "luaeditor.h"
#include "luautils.h"
#include "luacalls.h"
#include "customcontrol.h"
#include "console.h"




#include "nativemidi.h"


#define ENTER_REINIT {insideReInit=1;}
#define LEAVE_REINIT {insideReInit=0;}


#define VERSION "0.06"


#define MAX_PARMS 256

#ifdef _MSC_VER
#define BREAKPOINT  __asm int 3;
#elif defined(linux)
#define BREAKPOINT asm("int $3;")
#endif

#define TRACE {normal_output("%s() Line: %d of %s\n", __func__,  __LINE__, __FILE__);}




//get the instance from the lua_state without disturbing it
VstLua *getInstance(lua_State *state);



//Opens the swig wrapped functions
extern "C" 
{
extern int luaopen_vstgui(lua_State *state);
}



//"push gui component function"
extern int pushCViewContainerLua(lua_State *state, CViewContainer *cont);

//magic number for program data
#define PROGRAM_MAGIC 0xb0ad
#define BANK_MAGIC 0x10bad

class VstLuaProgram
{

    
    public:
    int version;
    float VSTLUA[MAX_PARMS];          
    char name[128];
    char *dataString;
    int dataStringLen;
    
    VstLuaProgram()
    {
        for(int i=0;i<MAX_PARMS;i++)
            VSTLUA[i] = 0.0;
        strcpy(name, "Initial");
        version = PROGRAM_MAGIC;
        dataString = NULL;
        setDataString("", 1); //one for zero!
    }
    
    void setDataString(char *str, int len)
    {    
        if(dataString)
            free(dataString);
            
        dataString = (char*)malloc(len);
        memcpy(dataString, str, len);    
        dataStringLen = len;
    }
    
    
};

    
class VstLuaProgramSet
{
    public:
    int version;
    VstLuaProgram programs[NUM_PROGRAMS];    
   
};
    
    
//return a copy of a string with zeros escaped    
void escape(char *withZeros, int len, char *buf, int buflen);

//unescape a string, return its length
int unescape(char *in, char *out, int buflen);

    
//-------------------------------------------------------------------------------------------------------
class VstLua : public AudioEffectX
{
public:
	VstLua (audioMasterCallback audioMaster);
	~VstLua ();

    virtual void open();
    virtual void resume();
    virtual void suspend();
    virtual void close();
   
	// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	

	// Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);
    virtual void setProgram(int program);
    virtual int getProgram();
    virtual bool getProgramNameIndexed(int category, int index, char *name);
    
    

	// Parameters
	virtual void setParameter (VstInt32 index, float value);
    virtual void setParameterNoCallback (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
    
    
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
    virtual VstInt32 getNumMidiInputChannels();
    virtual VstInt32 getNumMidiOutputChannels();
    virtual VstInt32 canDo(char *text);
    virtual VstInt32 processEvents(VstEvents *events);
    virtual VstInt32 processEvent(VstEvent *events);
    void recvMidiEvent(VstInt32 delta, VstInt32 len, VstInt32 offset, 
char data1, char data2, char data3, char data4, char detune, char noteOff, int sysexBytes, char *sysexData);
	void VstLua::recvNativeMidiEvent(int port_id, unsigned char data1,unsigned char data2, unsigned char data3, int sysexLen, char *sysexData);	
    void sendMidiEvent(VstInt32 delta, VstInt32 len, VstInt32 offset, 
char data1, char data2, char data3, char data4, char detune, char noteOff);
    virtual void setSampleRate (float sampleRate);
    virtual void setBlockSize (VstInt32 blockSize);
    virtual bool getOutputProperties (VstInt32 index, VstPinProperties* properties);
    void setProgramNoSave(int program);
    void setScript(char *path);
    
    int getNetworkToken();

    void newNetworkToken();
    
    void Lock();
    void Unlock();
   
    void checkLuaCall(lua_State *state, int in, int out, int err);
    void ReceiveFromListenLoop(lua_State *state);
    void initInterpreter();
   void onOpened(CViewContainer *container);
   void onClosed();
   void onSaveProgram();
   void onLoadProgram();
   void onReset();
   char *onHelp();
   void onValueChanged(int tag, float value, char *str);
   
   
   void sendSysex(char *str, int len, int delta);
   
   
   void messageCallback(char *name, char *msg, int len);
   
   void closeInterpreter();
   
   VstLuaProgram *getCurrentProgram();


   //chunk usage
   virtual VstInt32 setChunk (void* data, VstInt32 byteSize, bool isPreset=false);
   virtual VstInt32 getChunk (void** data, bool isPreset=false);
   
   void preloadScript(char *name);
   char *scriptToLoad;
   
   void setGUISize(int x, int y, int guiID);
   
    void onFrame(int sampleFrames);
   long long sampleCtr;
   void reInit(void);
   lua_State *getState();
   
   
   
   void lockHost();
   void unlockHost();
   
   
   char base_path[512]; // Base path of the vst
   int halted; // Is the interpreter stopped?
   
   
   int getFromConfig(char *name, int def);
   const char *getFromConfig(char *name, const char *def);
   double getFromConfig(char *name, double def);
    
    
    void consoleCommand(char *cmd);
    char *consoleTabExpand(char *cmd);
    
    
    
    /* output functions */
    void normal_output(char *fmt, ...);
    void error_output(char *fmt, ...);
    void verror_output(char *line);
    void fatal_error(char *fmt, ...);
    void cprint(char *fmt,...);
    
    bool onKey(VstKeyCode &code, int isUp);
    void onDrop(CDragContainer *drag);
    
    int updateConsole();
    
    LuaLock *exlock;
    LuaLock *hostLock;
    
    
    
    LuaEditor *editor;
    
    int isInsideReInit();
    
    SharedMem *memoryChannel;
    
	Console *console;
	
    lua_State *config_state;
    
    
    
protected:	
    float params[MAX_PARMS];
    int curProgram;
    
    VstLuaProgramSet *programSet;
    
    int insideReInit;
    
    
    lua_State *lua_state;
    

    
    FILE *logFile;
    char current_script[512];
 
	char programName[kVstMaxProgNameLen+1];
    int networkToken;
    void setInstance();
    void registerFunctions();
    void findVSTPath();
    void setLuaPath();
    void initScript();
    
    
    
    
};

#endif


