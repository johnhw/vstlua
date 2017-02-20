extern "C"
{
#include "lua.h"
}
#include "math.h"
#include "vstlua.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "osc/OscOutboundPacketStream.h"
#include "osclua.h"
#define OUTPUT_BUFFER_SIZE 65535




//push a value with a special type tag. the format is a table, with the first element being the type, and the second the value.
void pushSpecial(lua_State *state, osc::OutboundPacketStream &p)
{
    //get the type tag
    lua_pushnumber(state, 1);
    lua_gettable(state, -2);
 
    //check it's a string
    if(!lua_isstring(state,-1))
    {        
        getInstance(state)->fatal_error("Special tables must have a type tag as the first element\n");
        lua_pop(state,1);
        return;    
    }
    else
    {
        char typenam[512];
        strncpy(typenam, lua_tostring(state,-1), sizeof(typenam));
        int bad_type = 0; // bad type flag
    
        lua_pop(state,1); // pop the name
        
        //get the next value
        lua_pushnumber(state, 2);
        lua_gettable(state, -2);    
            
        
        //now check how to format this...
        //we can explicitly specify types if we want...
        if(!strcmp(typenam, "float"))
        {
            if(lua_isnumber(state,-1))
            {
                float f = lua_tonumber(state,-1);
                p<<f;
            }
            else
                bad_type=1;            
        }
        
        if(!strcmp(typenam, "double"))
        {
            if(lua_isnumber(state,-1))
            {
                double f = lua_tonumber(state,-1);
                p<<f;
            }
                else
                bad_type=1;
            
        }
        
        if(!strcmp(typenam, "string"))
        {
            if(lua_isstring(state,-1))            
                p<<lua_tostring(state,-1);
            else
                bad_type=1;
            
        }
        
        if(!strcmp(typenam, "symbol"))
        {
            if(lua_isstring(state,-1))
                p<<osc::Symbol(lua_tostring(state,-1));
            else
                bad_type=1;
            
        }
        
        
        if(!strcmp(typenam, "blob"))
        {
            if(lua_isstring(state,-1))
                p<<osc::Blob(lua_tostring(state,-1), lua_strlen(state,-1));
            else
                bad_type=1;
            
        }        
        
        if(!strcmp(typenam, "infinite"))
        {
            p<<osc::Infinitum;            
        }        
        
        if(!strcmp(typenam, "nil"))
        {
            p<<osc::Nil;            
        }        
        
                                
        if(!strcmp(typenam, "rgbacolor"))
        {
            if(lua_isnumber(state,-1))                
                p<< osc::RgbaColor((int)lua_tonumber(state,-1));
            else bad_type=1;
            
        }
        
        if(!strcmp(typenam, "int32"))
        {
            if(lua_isnumber(state,-1))                            
                p<<(int)lua_tonumber(state,-1);
            else bad_type=1;
            
        }
        
        if(!strcmp(typenam, "char"))
        {
            char str[2];
            //special exception for strings
            if(lua_isnumber(state,-1))                                            
                p<<(char)lua_tonumber(state,-1);
            else if(lua_isstring(state,-1))
            {   
                strncpy(str, lua_tostring(state,-1), 2);
                p<<str[0];
            }
            else bad_type = 1;                
            
        }
            
        
        if(!strcmp(typenam, "int64"))
        {
            if(lua_isnumber(state,-1))                                                        
                p<<(osc::int64)lua_tonumber(state,-1);
            else bad_type=1;            
            
        }
        
        if(!strcmp(typenam, "midi"))
        {
            if(lua_isnumber(state,-1))                                                        
                p<<osc::MidiMessage((int)(lua_tonumber(state,-1)));
            else bad_type=1;            
            
        }
        
        if(!strcmp(typenam, "boolean"))
        {
            if(lua_isboolean(state,-1))                                                        
                p<< (bool)(lua_toboolean(state,-1));
            else bad_type=1;            
            
        }
        
        
        
        if(!strcmp(typenam, "timetag"))
        {
            if(lua_isnumber(state,-1))                                                        
                p<<osc::TimeTag((osc::uint64)lua_tonumber(state,-1));
            else bad_type=1;
            
        }
        
        lua_pop(state,1);
        
        //warn the user if a tag was invalid and therefore skipped
        if(bad_type)    
                getInstance(state)->error_output("Warning: bad type for second element for OSC type %s\n", typenam);                                                                              
    }        

}


int pushValuesLua(lua_State *state, osc::OutboundPacketStream &p)
{
    
    int index = 1;
    int going = 1;
    while(going)
    {
    
   
        lua_pushnumber(state, index);
        lua_gettable(state, -2);
        
        //do the right thing with the type
        switch(lua_type(state, -1))
        {
            case LUA_TNIL:
                going  = 0;
                break;
                
            case LUA_TBOOLEAN:
                p << lua_toboolean(state, -1);
                break;
                
            case LUA_TNUMBER:
            {
                double val = lua_tonumber(state,-1);
                
                //push integers as integers
                if(val==floor(val))
                    p<<(int)val;
                else
                    p<<val;                               
                    
            }
                break;
                                
            case LUA_TSTRING:
                p<<lua_tostring(state,-1);
                break;       
                
            case LUA_TTABLE:
                pushSpecial(state, p);
                break;

            default:
                getInstance(state)->fatal_error("Invalid entry in an OSC packet. Must be string, boolean, special table or number.\n");
                return 0;
                break;
        }
        index++;
        lua_pop(state, 1);    
    }
    
    
    return 1;
}

int toOSCBundle(lua_State *state, osc::OutboundPacketStream &p)
{
    int going = 1;
    int index = 2; // start _after_ the time tag
    
    //timetag is at top of stack    
    double tag = lua_tonumber(state, -1);
    lua_pop(state,1);
    
    p << osc::BeginBundle((osc::uint64)tag);
    while(going)
    {
        //get the next message
        lua_pushnumber(state, index);
        lua_gettable(state, -2);
        
        if(lua_istable(state, -1))
        {        
            //check if this is a nested bundle, or a packet                                              
           lua_pushnumber(state, 1);
           lua_gettable(state, -2);
           
           //message if it begins with a string
           if(lua_isstring(state,-1))
            toOSCMessage(state, p);
           else if(lua_isnumber(state,-1))
            toOSCBundle(state, p);
          else
          {
            getInstance(state)->fatal_error("Start of a nested table must be an address (for messages) or a timetag (for bundles)\n");
            going = 0; 
           }
        }
        else
            going = 0;             
        lua_pop(state, 1);        
        index++;        
    }    
    p << osc::EndBundle;  
   
   
    
    return 1;
   
}


int toOSCMessage(lua_State *state, osc::OutboundPacketStream &p)
{
        char addr[512];

           if(lua_isstring(state, -1))
           {
                strncpy(addr, lua_tostring(state, -1), 512);
                lua_pop(state, 1);
                
   
                //begin the message
                p << osc::BeginMessage(addr);
                
                //get the values
                lua_pushnumber(state, 2);
                lua_gettable(state, -2);
                
                //push all the values; error if it returns 0;
                if(!pushValuesLua(state, p))
                {
   
                    getInstance(state)->fatal_error("Error in message components!\n");
                    return 0;                
                }                           
                lua_pop(state, 1);
                
                //end the message
                p << osc::EndMessage;
                return 1;
           }
           else
           {
            getInstance(state)->fatal_error("First element of an OSC message should be the address!");
            return 0;
           }                          
           
}

//take an osc structure, return the string all packed up
int toOSCLua(lua_State *state)
{
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

    // got to have a table!    
    if(!lua_istable(state, -1))
        getInstance(state)->fatal_error("Non-table argument to toOSC!");
        
       
    lua_pushnumber(state, 1);
    lua_gettable(state,-2);
    
    //if this begins with a number, it's the time tag, and we're doing a bundle.
    if(lua_isnumber(state,-1))
    {                
        toOSCBundle(state, p);
    }
    //if it's a string it's the address and we're doing a message
    else if(lua_isstring(state,-1))
    {        
        toOSCMessage(state, p);
    }
    //otherwise we didn't call it right
    else
    {
        getInstance(state)->fatal_error("first element of toOSC must be a time (for bundles) or an address (for messages)");
        return 0;
    }
    lua_pushlstring(state, p.Data(), p.Size());
    return 1;
}


void fromOSCBundle(lua_State *state, osc::ReceivedPacket &p, int fulltype)
{
    osc::ReceivedBundle b(p);    
    fromOSCParseArgs(state, b, fulltype)  ;
}


void fromOSCBundle(lua_State *state, const osc::ReceivedBundleElement &p, int fulltype)
{
    osc::ReceivedBundle b(p);    
    fromOSCParseArgs(state, b, fulltype);    
}


void fromOSCParseArgs(lua_State *state, osc::ReceivedBundle b, int fulltype)
{
    osc::ReceivedBundle::const_iterator arg = b.ElementsBegin();
        
    //first value is the time tag
    lua_pushnumber(state, 1);
    lua_pushnumber(state, b.TimeTag());
    lua_settable(state, -3);
            
    int index = 2;
    while(arg!=b.ElementsEnd())
    {            
        
        //second value is the table
        lua_pushnumber(state, index);
    
        lua_newtable(state);
        if(arg->IsBundle())
        {                
            //we probably don't ever need to do this...
            fromOSCBundle(state,  *arg, fulltype);
        }
        else
        {
    
            fromOSCMessage(state, *arg, fulltype);        
        }    
        arg++;
        index++;
        lua_settable(state, -3);
    }        
    
}


void fromOSCMessage(lua_State *state, const osc::ReceivedBundleElement &p, int fulltype)
{
    osc::ReceivedMessage m(p);
    parseOSCMessage(state, m, fulltype);
}

void fromOSCMessage(lua_State *state, osc::ReceivedPacket &p, int fulltype)
{
        osc::ReceivedMessage m(p);
        parseOSCMessage(state, m, fulltype);
                              
}



void parseOSCMessage(lua_State *state, osc::ReceivedMessage &m, int fulltype)
{

        char typestr[512];
    
        
        
        
        //element 1 = address
        lua_pushnumber(state, 1);
        lua_pushstring(state, m.AddressPattern());
        lua_settable(state, -3);        
                
        
        
        //element 2 = OSC table
        lua_pushnumber(state, 2);
        lua_newtable(state);
        
        osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
        
        int index = 1;
        
        while(arg!=m.ArgumentsEnd())
        {
            lua_pushnumber(state, index);
            
            
            //set the second element, if we're writing out full types
            if(fulltype)
            {
                lua_newtable(state);                
                lua_pushnumber(state,2);
            }                        
                
                
                //parse the tag
                switch(arg->TypeTag())
                {
                    case osc::INT64_TYPE_TAG:                                            
                        strncpy(typestr, "int64", sizeof(typestr));
                        lua_pushnumber(state, (double)arg->AsInt64());
                        break;
                    case osc::TRUE_TYPE_TAG:
                        strncpy(typestr, "boolean", sizeof(typestr));
                        lua_pushboolean(state, 1);
                        break;
                    case osc::FALSE_TYPE_TAG:    
                        strncpy(typestr, "boolean", sizeof(typestr));
                        lua_pushboolean(state, 0);
                        break;
                    case osc::NIL_TYPE_TAG:
                        strncpy(typestr, "nil", sizeof(typestr));
                        lua_pushstring(state, "nil");
                        break;
                    case osc::INFINITUM_TYPE_TAG:
                        strncpy(typestr, "infinity", sizeof(typestr));
                        lua_pushstring(state, "infinity");
                        break;
                    case osc::INT32_TYPE_TAG:
                        strncpy(typestr, "int32", sizeof(typestr));
                        lua_pushnumber(state, arg->AsInt32());
                        break;
                    case osc::FLOAT_TYPE_TAG:
                        strncpy(typestr, "float", sizeof(typestr));
                        lua_pushnumber(state, arg->AsFloat());
                        break;
                        
                    case osc::DOUBLE_TYPE_TAG:
                        strncpy(typestr, "double", sizeof(typestr));
                        lua_pushnumber(state, arg->AsDouble());
                        break;
                        
                    case osc::CHAR_TYPE_TAG:
                        strncpy(typestr, "char", sizeof(typestr));
                        lua_pushnumber(state, arg->AsChar());
                        break;
                        
                     case osc::RGBA_COLOR_TYPE_TAG:
                        strncpy(typestr, "rgbacolor", sizeof(typestr));
                        lua_pushnumber(state, arg->AsRgbaColor());
                        break;
                     
                    case osc::MIDI_MESSAGE_TYPE_TAG:
                        strncpy(typestr, "midi", sizeof(typestr));
                        lua_pushnumber(state, arg->AsMidiMessage());
                        break;
                    
                    case osc::TIME_TAG_TYPE_TAG:
                        strncpy(typestr, "timetag", sizeof(typestr));
                        lua_pushnumber(state, arg->AsTimeTag());
                        break;
                                        
                    case osc::STRING_TYPE_TAG:
                        strncpy(typestr, "string", sizeof(typestr));
                        lua_pushstring(state, arg->AsString());
                        break;
                        
                    case osc::SYMBOL_TYPE_TAG:
                        strncpy(typestr, "symbol", sizeof(typestr));
                        lua_pushstring(state, arg->AsSymbol());
                        break;
                        
                    case osc::BLOB_TYPE_TAG:
                        strncpy(typestr, "blob", sizeof(typestr));
                        const void *ptr;
                        long unsigned int len;
                        arg->AsBlob(ptr,len);
                        lua_pushlstring(state, (char*)ptr, len);
                        break;                                                                
                 }
                        
                if(fulltype) 
                {                 
                    //set the value
                    lua_settable(state,-3);
                    //set the name
                    lua_pushnumber(state,1); 
                    lua_pushstring(state, typestr); 
                    lua_settable(state, -3);                      
                }
        
            //set the value of this index
            lua_settable(state, -3);
            index ++;
            arg++;
        
        }        
    
    lua_settable(state, -3);
    
}


int fromOSCLua(lua_State *state)
{
    //push the address...
        if(!lua_isstring(state,1))
        {
            getInstance(state)->fatal_error("fromOSC called without a string argument!");
            return 0;
        }
        
            int fulltype;
        
            //check if we need to use the full type form
            if(lua_gettop(state)==2 && lua_tonumber(state,2))
            {
                fulltype = 1;
                lua_pop(state,1); //make sure top of stack is the string again
            }
            else
                fulltype=0;
            
            
            //get the string to be decoded
            const char *msg = lua_tostring(state, -1);
            int len = lua_strlen(state,-1);
            
            lua_pop(state,1);
            
            //Decode the packet
            osc::ReceivedPacket p(msg, len);        
                       
            lua_newtable(state);            
            if(p.IsBundle())
            {               
                fromOSCBundle(state,p,fulltype);
            }
            else
            {                
                fromOSCMessage(state,p,fulltype);
            }
            //return two values, (1) is this message or a bundle (2) the table data
            return 1;                                                 
}

