#ifndef __nativemidi__
#define __nativemidi__


#include <lua.h>


int getMidiDeviceNamesLua(lua_State *state);
int openMidiDeviceLua(lua_State *state);
int sendMidiDeviceLua(lua_State *state);
int recvMidiCallback(LPVOID *instance);
int closeMidiDeviceLua(lua_State *state);
int sendNativeMidiLua(lua_State *state);
int sendNativeSysexLua(lua_State *state);
int cleanUpMidiLua(lua_State *state);



#endif

