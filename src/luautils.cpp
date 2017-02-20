#include "vstlua.h"

//#include "debug_new.h"

/* assume that table is at the top */
    void setfieldi (lua_State *state, const char *index, int value) {
      lua_pushstring(state, index);
      lua_pushnumber(state, value);
      lua_settable(state, -3);
    }

    /* assume that table is at the top */
    void setfieldf (lua_State *state, const char *index, double value) {
      lua_pushstring(state, index);
      lua_pushnumber(state, value);
      lua_settable(state, -3);
    }

    /* assume that table is at the top */
     void setfields (lua_State *state, const char *index, char * value)
     {
       lua_pushstring(state, index);
       lua_pushstring(state, value);
       lua_settable(state, -3);         
     }
     
    
 int getfield (lua_State *state, const char *key, int def) {
      int result;
      lua_pushstring(state, key);
      lua_gettable(state, -2);  /* get background[key] */
      if (!lua_isnumber(state, -1))
      {        
        lua_pop(state, 1);  
        return def;
      }
      result = (int)lua_tonumber(state, -1);
      lua_pop(state, 1);  
      return result;
    }

    

    
 double getfieldf (lua_State *state, const char *key, double def) {
      double result;
      lua_pushstring(state, key);
      lua_gettable(state, -2);  /* get background[key] */
      if (!lua_isnumber(state, -1))
      {        
        lua_pop(state, 1);  
        return def;
      }
      result = (double)lua_tonumber(state, -1);
      lua_pop(state, 1);  
      return result;
    }


    void stackDump (lua_State *L) {

       VstLua *instance = getInstance(L);
      int i;
      int top = lua_gettop(L);
      for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {    
          case LUA_TSTRING:  /* strings */
            
            instance->normal_output("`%s'", lua_tostring(L, i));            
            break;
    
          case LUA_TBOOLEAN:  /* booleans */
            if(lua_toboolean(L, i))             
                instance->normal_output("true");
            else
                instance->normal_output("false");            
            break;
    
          case LUA_TNUMBER:  /* numbers */
            instance->normal_output("%g", lua_tonumber(L, i));            
            break;
    
          default:  /* other values */
            instance->normal_output("%s", lua_typename(L, t));            
            break;
    
        }
        instance->normal_output("  ");  /* put a separator */
      }
      instance->normal_output("\n");  /* end the listing */
      
    }
    
    
    void addPointerToFree(lua_State *L, void *ptr)
    {
        
        lua_pushstring(L, "_pointerIndex");
        lua_gettable(L, LUA_REGISTRYINDEX);
        
        //create it if it doesn't exist
        if(!lua_istable(L,-1))
        {
            lua_pop(L,1); //pop the nil
            lua_pushstring(L, "_pointerIndex");
            lua_newtable(L);
            lua_settable(L, LUA_REGISTRYINDEX);                
            lua_pushstring(L, "_pointerIndex");
            lua_gettable(L, LUA_REGISTRYINDEX);            
        }
        
        //NB: ptr is stored in key!
        lua_pushlightuserdata(L, ptr);
        lua_pushnumber(L, 1);
        
        //call table.insert
        lua_settable(L,-3);
        
        lua_pop(L,1); // pop the table
    }
    
    void freePointers(lua_State *L)
    {    
        lua_pushstring(L, "_pointerIndex");
        lua_gettable(L, LUA_REGISTRYINDEX);
        
        if(lua_istable(L,-1))
        {
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) 
            {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                void *ptr = lua_touserdata(L,-2);
                free(ptr);
              
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(L, 1);
            }
        
        }
        
        lua_pop(L,1); //pop the table / nil    
    }

    
    int copyRecursive(lua_State *L1, lua_State *L2, int recurseLevel)
        {
        
                if(recurseLevel>32)
                {
                    
                    return 0;            
                }
                
                if(!(lua_isstring(L1, -1) || lua_istable(L1,-1) || lua_isboolean(L1,-1) || lua_isnumber(L1,-1)))
                    {                     
                        return 0;
                    }
                    
                    
                if(lua_isstring(L1,-1))
                {
                    lua_pushstring(L2, lua_tostring(L1,-1));            
                }
                
                if(lua_isnumber(L1,-1))
                {
                    lua_pushnumber(L2, lua_tonumber(L1,-1));            
                }
                
                if(lua_isboolean(L1,-1))
                {
                    lua_pushboolean(L2, lua_toboolean(L1,-1));            
                }
                
                
                //recursive...
                if(lua_istable(L1,-1))
                {
                    //create the table
                    lua_newtable(L2);
                    
                    //get the first value
                    
                    lua_pushnil(L1); 
                    while (lua_next(L1, -1) != 0) 
                    {
                    
                        //push the key
                        if(lua_isnumber(L1,-2))
                            lua_pushnumber(L2, lua_tonumber(L1,-2));
                        else if(lua_isstring(L1,-2))
                            lua_pushstring(L2, lua_tostring(L1,-2));
                        else if(lua_isboolean(L1,-2))
                            lua_pushboolean(L2, lua_toboolean(L1,-2));
                        else 
                            return 0;
                            
                        
                        //now, do a copy, with value on top of the stack...
                        //L2 should now be -2=key, -1=value
                        int copyResult = copyRecursive(L1, L2, recurseLevel+1);
                        
                        //return immediately if the copy fails
                        if(!copyResult)
                            return 0;
                        
                        //set this entry in the second table
                        lua_settable(L2, -3);
                        
                        /* removes 'value'; keeps 'key' for next iteration */
                        lua_pop(L1, 1);                
                    }
                    
                                                   
            }
            return 1;                                      
        }
        
        
/* Return a table from the registry given by name, creating it if necessary */
void getRegistryTable(lua_State *L, char *name)
{
     lua_pushstring(L, name);
     lua_gettable(L, LUA_REGISTRYINDEX);
        if(lua_isnil(L,-1))
        {
            lua_pushstring(L, name);
            lua_newtable(L);
            lua_settable(L, LUA_REGISTRYINDEX);
            lua_pushstring(L, name);
            lua_gettable(L, LUA_REGISTRYINDEX);                    
        }

}