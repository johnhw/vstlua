
#ifndef __SharedMem__
#define __SharedMem__



#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#ifdef WINDOWS

#include <windows.h>


#define SHARED_MEM_NAME "VSTLUA_SHAREDMEM"
#define MAX_INSTANCES 64
#define SHARED_LUA_SPACE 5000000

struct SharedData
{
    lua_State *globalLua;
    int nInstances;
    char instances[MAX_INSTANCES][32];
    int alive[MAX_INSTANCES];       
    char sharedLua[SHARED_LUA_SPACE]; 
    
    
           
};


class SharedMem
{
    public:
        SharedMem(VstLua *instance);
        ~SharedMem();
  
        int lockShared();
        int unlockShared();
        
        int writeToShared(char *str);
        char *readFromShared();
        
        
        void pushInstances(lua_State *state);             
        void sendToInstance(char *name, char *msg);
        char *mappedData;
        void messageCallback(char *name, char *msg, int len);
        HANDLE localMutex;
        HANDLE localFinishedEvent;
        HANDLE globalMutex;
        HANDLE localEvent;
        int receiving;
        int thisInstance;
        void sendToInstance(char *name, char *msgBlock, int len);
        
         VstLua *instance;
         SharedData *mapped;
           HANDLE sharedLuaLock; 
         int luaOffset;
        int alive;  
    protected:  
       
        HANDLE mapFile;
        HANDLE localMapFile;
 
       
        

        bool created;
};




#define SHARED_MEM_SIZE sizeof(SharedData)
//1Mb of shared buffer
#define SHARED_BUFFER_SIZE (1<<20)

#endif

#endif
