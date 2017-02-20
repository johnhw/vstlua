#include "luaeditor.h"
#include "vstlua.h"
#include "nativemidi.h"



#ifdef WINDOWS
#include <mmsystem.h>

/* Push a table mapping names to midi devices */
/* Table is in format: 
"in" = {(name : id, name :id}
"out" = {name:id, name:id}
*/
int getMidiDeviceNamesLua(lua_State *state)
{
    int numDevs = midiOutGetNumDevs();
    MIDIOUTCAPS outCaps;
    MIDIINCAPS inCaps;
    
    lua_newtable(state);
    
    lua_pushstring(state, "out");
    
    //out table
    lua_newtable(state);
    
    for (int i = 0; i < numDevs; i++)
    {    
        if (!midiOutGetDevCaps(i, &outCaps, sizeof(MIDIOUTCAPS)))
        {            
            
            
            //index
            lua_pushnumber(state, i+1);
            
            //{name, id}
            lua_newtable(state);
            lua_pushnumber(state,1);
            lua_pushstring(state, outCaps.szPname);
            lua_settable(state,-3);
            lua_pushnumber(state,2);
            lua_pushnumber(state, i);
            lua_settable(state,-3);
            
            
            lua_settable(state,-3);
            
            
        }
    }

    //set the "out" entry
    lua_settable(state, -3);
    
    lua_pushstring(state, "in");
    
    //in table
    lua_newtable(state);
    
    numDevs = midiInGetNumDevs();
    for (int i = 0; i < numDevs; i++)
    {    
        if (!midiInGetDevCaps(i, &inCaps, sizeof(MIDIINCAPS)))
        {            
            
            
            //index
            lua_pushnumber(state, i+1);
                        
            //{name, id}
            lua_newtable(state);
            lua_pushnumber(state,1);
            lua_pushstring(state, inCaps.szPname);            
            lua_settable(state,-3);
            lua_pushnumber(state,2);
            lua_pushnumber(state, i | 0x8000);            
            lua_settable(state,-3);
            
            
            lua_settable(state,-3);                       
        }
    }
    //set the "in" entry
    lua_settable(state, -3);    
    
    return 1;
}


/* Handle midi input */


void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	VstLua *instance = (VstLua*)dwInstance;
	if(uMsg==MIM_DATA)
	{
		unsigned char type = (dwParam1 >> 16) & 0xff;		
		unsigned char byte2 = (dwParam1 >> 8) & 0xff;
		unsigned char byte3 =  (dwParam1) & 0xff;
		// this should be passed to a thread or something...
		//also, we need to reverse map the port 
		BREAKPOINT;
		UINT id;
		midiInGetID(handle, &id);
		instance->recvNativeMidiEvent(id, type,  byte2, byte3, 0, NULL);
	
	}
	
	return;
}


void CALLBACK midiOutCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	VstLua *instance = (VstLua*)dwInstance;
	
	return;
}


/* Open a midi device, given the id from the table returned by getMidiDeviceNames. Returns nothing if port cannot be opened. or true if it can. */
int openMidiDeviceLua(lua_State *state)
{
    //should be a device number
    if(!lua_isnumber(state,-1))
        return 0;
    int id = (int)(lua_tonumber(state,-1));
    
    VstLua *instance = getInstance(state);
    
    //check if this is an in port
    if(id &0x8000)
    {
        HMIDIIN inHandle;
        int result = midiInOpen(&inHandle, id & 0xff, (DWORD)midiCallback, (DWORD)instance, CALLBACK_FUNCTION);
        if(result!=MMSYSERR_NOERROR)
            return 0;
        
        
        //store the handler
        getRegistryTable(state, "_openInPorts");
            
        lua_pushnumber(state, id);
        lua_pushlightuserdata(state, (void*) inHandle);
        lua_settable(state,-3);                        
        
        lua_pushboolean(state, 1);
        return 1;
    }
    else
    {
        HMIDIOUT outHandle;
        int result = midiOutOpen(&outHandle, id, (DWORD)midiOutCallback, (DWORD)instance, CALLBACK_FUNCTION);
        if(result!=MMSYSERR_NOERROR)
            return 0;
        
        
        //store the handler
        getRegistryTable(state, "_openOutPorts");
                  
        lua_pushnumber(state, id);
        lua_pushlightuserdata(state, (void*) outHandle);
        lua_settable(state,-3);                        
        
        lua_pushboolean(state, 1);
        return 1;    
    }    
}


int sendNativeSysexLua(lua_State *state)
{
	MIDIHDR midiHdr;
	if(!lua_istable(state,-1))
		return 0;
		
	int id = getfield(state, "port_id",0);
	lua_pushstring(state, "sysex");	
	lua_gettable(state,-2);
	size_t len;
	const char *str;
	str = lua_tolstring(state,-1,&len);
	
	if(str && len)
	{
		getRegistryTable(state, "_openOutPorts");
		lua_pushnumber(state, id);
		lua_gettable(state,-2);
		/* NB we need to do this in a timed loop! */
		if(lua_isuserdata(state,-1))
		{
			HMIDIOUT outHandle = (HMIDIOUT) (lua_touserdata(state,-1));		
			midiHdr.lpData = (CHAR *)(&(str[0]));
			midiHdr.dwBufferLength = len;
			midiHdr.dwFlags = 0;
			int err = midiOutPrepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR));
			if(!err)
			{
				err = midiOutLongMsg(outHandle, &midiHdr, sizeof(MIDIHDR));		
				//NB Must unprepare this header in the callback...
				lua_pushboolean(state,1);
				return 1;
			}
		}
	}	
	return 0;
}


int sendNativeMidiLua(lua_State *state)
{
	if(!lua_istable(state,-1))
		return 0;
		
	int id = getfield(state, "port_id",-1);
	int type = getfield(state, "type",0);
	int channel = getfield(state, "channel",0);
	int byte2 = getfield(state, "byte2",0);
	int byte3 = getfield(state, "byte3",0);	
		
	int msg = ((((type & 0xf) << 4) | (channel & 0xf)) << 16) | (byte2<<8) | (byte3);
	
	getRegistryTable(state, "_openOutPorts");
	lua_pushnumber(state, id);
	lua_gettable(state,-2);
	/* NB we need to do this in a timed loop! */
	if(lua_isuserdata(state,-1))
	{
		HMIDIOUT outHandle = (HMIDIOUT) (lua_touserdata(state,-1));		
		midiOutShortMsg(outHandle, msg);	
	}
	return 1;
}


int closeMidiDeviceLua(lua_State *state)
{
    //should be a device number
    if(!lua_isnumber(state,-1))
        return 0;
    int id = (int)(lua_tonumber(state,-1));
    

    
    
    //check if this is an in port
    if(id &0x8000)
    {
        
        getRegistryTable(state, "_openInPorts");        
        lua_pushnumber(state, id);
		lua_gettable(state,-2);
		
		// check if this port really is open
		if(lua_isuserdata(state,-1))
		{			
			HMIDIIN inHandle = (HMIDIIN)lua_touserdata(state,-1);
			midiInClose(inHandle);
			
			//clear the handler
			getRegistryTable(state, "_openInPorts");
            
			lua_pushnumber(state, id);
			lua_pushnil(state);
			lua_settable(state,-3);                                        
		
		}
    }
    else
    {
        HMIDIOUT outHandle;
        getRegistryTable(state, "_openOutPorts");        
        lua_pushnumber(state, id);
		lua_gettable(state,-2);
		
		// check if this port really is open
		if(lua_isuserdata(state,-1))
		{		
			HMIDIOUT outHandle = (HMIDIOUT)lua_touserdata(state,-1);
			midiOutClose(outHandle);
			
			//clear the handler
			getRegistryTable(state, "_openOutPorts");
            
			lua_pushnumber(state, id);
			lua_pushnil(state);
			lua_settable(state,-3);                                        
			
		}
    }    
	return 0;

}

/* Close all ports that have been opened. Must only be called at end of program. */
int cleanUpMidiLua(lua_State *state)
{
    getRegistryTable(state, "_openInPorts");
     if(lua_istable(state,-1))
        {
            lua_pushnil(state);
            while (lua_next(state, -2) != 0) 
            {                
				
                HMIDIIN handle = (HMIDIIN)lua_touserdata(state,-2);
                midiInClose(handle);
                              
                lua_pop(state, 1);
            }        
        }        
        lua_pop(state,1); //pop the table / nil    
        
    getRegistryTable(state, "_openOutPorts");
     if(lua_istable(state,-1))
        {
            lua_pushnil(state);
            while (lua_next(state, -2) != 0) 
            {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                HMIDIOUT handle = (HMIDIOUT)lua_touserdata(state,-2);
                midiOutClose(handle);
              
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(state, 1);
            }        
        }        
        lua_pop(state,1); //pop the table / nil        
		return 0;
		
}

#endif
