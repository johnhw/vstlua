#ifndef _LUAUTILS_
#define _LUAUTILS_
extern "C"
{
#include <lua.h>
void setfieldi (lua_State *state, const char *index, int value);
    void setfieldf (lua_State *state, const char *index, double value);      
    void setfields (lua_State *state, const char *index, char * value);      
    double getfieldf (lua_State *state, const char *key, double def);
    int getfield (lua_State *state, const char *key, int def);      
    void stackDump (lua_State *L);
    int copyRecursive(lua_State *L1, lua_State *L2, int recurseLevel);
    
    void addPointerToFree(lua_State *L, void *ptr);
    void freePointers(lua_State *L);
    void getRegistryTable(lua_State *L, char *name);
}

    
#endif      
