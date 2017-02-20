
#ifndef __SysFuncs__
#define __SysFuncs__




#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>


#ifdef WINDOWS

#include <windows.h>




class LuaLock
{
public:
LuaLock();
~LuaLock();
int Lock();
void Unlock();
protected:


long count;
	DWORD threadID;
	HANDLE eventHandle;
	long recursivecount;

CRITICAL_SECTION cs;
};


class VstLua;



DWORD WINAPI receivePoll(LPVOID parameter);



void sysSleep(int ms);
double sysGetTimeUs();
HMODULE sysGetModule();
void sysCreateHelpWindow(char *msg);
void sysGetCurrentPath(char *buf);
void addHandleToClose(lua_State *L, HANDLE h);
void freeHandles(lua_State *L);


#endif


#ifdef VS2005 // C99 function not implemented in Win32
extern int snprintf(char* str, size_t size, const char* format, ...);
#endif



#endif
