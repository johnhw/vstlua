
#ifndef _LUACALLS_
#define _LUACALLS_
#include "lua.h"



int getHostTimeLua(lua_State *state);

int getVSTPathLua(lua_State *state);
int printLua(lua_State *state);
int errorPrintLua(lua_State *state);

int getParameterLua(lua_State *state);
int setParameterLua(lua_State *state);
 int sendMidiEventLua(lua_State *state);
 
 int addControlLua(lua_State *state);
 
 int setValueLua(lua_State *state);
 int getValueLua(lua_State *state);
 int setLabelLua(lua_State *state);
 
 int getCycleDataLua(lua_State *state);
 int getBarStartLua(lua_State *state);
 int openFileSelectorLua(lua_State *state);
 int getTimeUsLua(lua_State *state);
 int guiRedrawLua(lua_State *state);
 int networkReceiveLoopLua(lua_State *state);
 
 int getSizeLua(lua_State *state);
 int setSizeLua(lua_State *state);
 
 int setHostTimeLua(lua_State *state);
  
  int resetLua(lua_State *state);
  
  int getInstancesLua(lua_State *state);
  int sendToInstanceLua(lua_State *state);
 
 int escapeLua(lua_State *state);
 int unescapeLua(lua_State *state);
 

 int sharedLockLua(lua_State *state);
 int sharedUnlockLua(lua_State *state);
 int sharedWriteLua(lua_State *state);
 int sharedReadLua(lua_State *state);
 
 
 int readProgramDataLua(lua_State *state);
 int writeProgramDataLua(lua_State *state);
 

 
 
 MenuEntry *recursiveParseMenu(lua_State *state);
 
 #endif
