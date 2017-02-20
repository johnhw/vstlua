/* JHW 2007 */

#include <string.h>
// #include <lua.hpp>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "lookup3.h"

#include "luahash.h"


//mainly useful for getting hashvalues of tables
int hashLua(lua_State *state)
{

    //create a anti-infinite loop table
    lua_newtable(state);

    int ref = luaL_ref(state, LUA_REGISTRYINDEX);
    
    uint32_t a=0,b=0;
    
    hashLuaRecurse(state,0,ref, &a, &b);
    
    lua_pushnumber(state, int64todouble(&a,&b));
    
    luaL_unref(state, LUA_REGISTRYINDEX, ref);
    return 1;
}




double int64todouble(uint32_t *a, uint32_t *b)
{
    double d;
    uint32_t *ptr;
    ptr = (uint32_t *)&d;
    ptr[0] = *a;
    ptr[1] = *b;    
    ptr[1] &= 0xbfffffff; //clear high bit of exponent
    return d;
}

void doubletoint64(double d, uint32_t *a, uint32_t *b)
{
    uint32_t *ptr = (uint32_t *)&d;
    *a = ptr[0];
    *b = ptr[1];    
}


//return a unique hash code for a given value. Hashes values not addresses, so
//hash({1,"hello"}) == hash({1,"hello"}) even if this is compared in a different lua interpreter instance
 
void hashLuaRecurse(lua_State *state, int depth, int ref, uint32_t*a,  uint32_t*b)
{ 

    
    
    int type = lua_type(state,-1);
    hashword2((uint32_t *)&type, 1, a, b);
    
    switch(type)
    {
        
        //treat a double as a 64 bit value ...             
        case LUA_TNUMBER:
        {
            double n = lua_tonumber(state,-1);            
            hashword2((uint32_t *)&n, 2, a, b);
            break;
        }
        
        case LUA_TBOOLEAN:
        {
            uint32_t i = lua_toboolean(state,-1);
            hashword2(&i,1,a,b);
            break;        
        }
               
        //recursively hash the elements
        case LUA_TTABLE:
        {         
                
                int ptr=(int)lua_topointer(state,-1);
                
                //first, check if we've seen this table already
                lua_rawgeti(state, LUA_REGISTRYINDEX, ref);                
                lua_pushnumber(state, ptr); //duplicate the passed in table reference...
                lua_gettable(state,-2);
                
                
                //get the original hash value if it's there
                if(lua_isnumber(state,-1))
                {
                    double oldValue = lua_tonumber(state,-1);
                    
                    //return the old version
                    doubletoint64(oldValue, a, b);                    
                    lua_pop(state,2); //pop off the result                                        
                }
                else
                {
                    lua_pop(state,2); //pop off nil and table
                }
                
                                
                
        
                lua_pushnil(state);                   
                uint32_t startoftable = 0xD117A788+depth;                                                  
                hashword2(&startoftable,1, a, b);
                                
                
                
                //(equivalent to opening bracket if tables were written like [a, b, [c, [d]]]
                while (lua_next(state, -2) != 0) 
                {
                    //get the value hash;
                    
                    
                    hashLuaRecurse(state,depth+1, ref, a,b );
                    lua_pop(state,1);                    
                    

                    //get the key hash
                    int kh1=0, kh2=0;
                    hashLuaRecurse(state,depth+1, ref, a,b);                        
                    
                                                                
                }
                     

                
                
                uint32_t endoftable = 0x193ACC0+depth;                                
                hashword2(&endoftable,1, a, b); //end of table (closing bracket)                
                
                //store this table...
                lua_rawgeti(state, LUA_REGISTRYINDEX, ref);               
                lua_pushnumber(state,ptr); //duplicate the reference
                lua_pushnumber(state, int64todouble(a,b));
                lua_settable(state,-3);
                lua_pop(state,1); //pop the table                                               
                break;
        }
        
        
        case LUA_TSTRING:
        {
            char *ptr = (char *)lua_tostring(state,-1);
            int len = lua_strlen(state,-1);
            hashlittle2((void *)(ptr), len, a, b);        
            break;
        }
        
        

        
        case LUA_TFUNCTION:
        {
            lua_Debug ar;
            lua_getinfo(state, ">S", &ar); //get function info
            
            //hash name and source definition
            
            hashlittle2((void*)&ar.linedefined,1, a, b);
            hashlittle2((void*)ar.source, strlen(ar.source), a,b);                                               
            break;
        }
        
        case LUA_TLIGHTUSERDATA:
            break;
            
        //everything else, just anidentifier        
        case LUA_TTHREAD:
            break;
        
        case LUA_TNIL:
            break;
            
        case LUA_TUSERDATA:
            break;
    
    
    }
   
   


}
