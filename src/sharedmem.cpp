#include "luaeditor.h"
#include "vstlua.h"
#include "sysfuncs.h"


#include <stdlib.h>
#include <stdio.h>
#include <lua.h>



#ifdef WINDOWS


       //users must lock before writing and release afterwards!
       //completely overwrites the shared space. Normally this should be used for storing a serialized table...
      int SharedMem::writeToShared(char *str)
      {
            if(WaitForSingleObject(globalMutex, 50)!=WAIT_OBJECT_0 || mapped==NULL)
            {
                ReleaseMutex(globalMutex);
                return 0;
            }
                
            strncpy(mapped->sharedLua, str,  SHARED_LUA_SPACE);
            ReleaseMutex(globalMutex);
            return 1;
      
      }
      
      
    //return a pointer to the shared memory. This should be _copied_ on return, as otherwise the string may change
    //unexpectedly
    char *SharedMem::readFromShared()
    {
            if(WaitForSingleObject(globalMutex, 50)!=WAIT_OBJECT_0 || mapped==NULL)
            {
                ReleaseMutex(globalMutex);
                return NULL;
            }
                
            ReleaseMutex(globalMutex);
            return mapped->sharedLua;        
    
    }
     

    
        SharedMem::SharedMem(VstLua *instance)
        {                
                created = false;
                mapFile = NULL;                
                mapped = NULL;
                localMapFile = NULL;
                mappedData = NULL;
                
                this->instance = instance;
                
                
                //create the mutex, or obtain a handle for the current one
                globalMutex = CreateMutex(NULL, FALSE, "VstLuaGlobalLock");                                                                                                                  
                int result = WaitForSingleObject(globalMutex, 500);                
                
                if(result!=WAIT_OBJECT_0)
                {
                    instance->fatal_error("Could not get global mutex!");
                    ReleaseMutex(globalMutex);
                    return;                    
                }
                
                
                mapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEM_NAME);
                if(mapFile==NULL)
                {
                    //need to create it...
                    mapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_MEM_SIZE, SHARED_MEM_NAME);
                    
                    created=true;
                    if(mapFile==NULL)                                           
                    {
                        ReleaseMutex(globalMutex);
                        return;                                            
                    }
                }
                                
                
                
                mapped=(SharedData *)MapViewOfFile(mapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEM_SIZE);                
                if(!mapped)
                {
                    ReleaseMutex(globalMutex);
                    return;
                }
                
                
                    
                //initialise, if this is the first instance
                if(created)
                {
                    memset(mapped, 0, SHARED_MEM_SIZE);
                    mapped->nInstances = 0;
                    for(int i=0;i<MAX_INSTANCES;i++)
                    {
                        strcpy(mapped->instances[i], "");                
                        mapped->alive[i] = 0;
                     }
                     
                                       
                    
                
                }
                
                
                
                
                
                //now, we need to register this instance
                thisInstance = mapped->nInstances;
                
                mapped->nInstances++; 
                
                snprintf(mapped->instances[thisInstance], sizeof(mapped->instances[thisInstance]), "VSTLUA%d", mapped->nInstances);
                
                
                
                //copy nInstances while we're synchronized
                char localMutexName[512], localEventName[512], localFinishedEventName[512];
                snprintf(localMutexName,sizeof(localMutexName), "VstLuaLock%s", mapped->instances[thisInstance]);                
                snprintf(localEventName,sizeof(localEventName), "VstLuaEvent%s", mapped->instances[thisInstance]);                
                snprintf(localFinishedEventName,sizeof(localFinishedEventName), "VstLuaFinished%s", mapped->instances[thisInstance]);                

                               
                sharedLuaLock = CreateMutex(NULL, FALSE, "SharedLuaLock");
                
                
                //Obtain our own mutex
                localMutex = CreateMutex(NULL, TRUE, localMutexName);
                //and the signalling event for new messages (which is auto-resetting)
                localEvent = CreateEvent(NULL, FALSE, FALSE, localEventName);
                localFinishedEvent = CreateEvent(NULL, FALSE, FALSE, localFinishedEventName);
                                
                                  
                 //now create the map file for this objects receive....
                 localMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_BUFFER_SIZE, mapped->instances[thisInstance]);
                 if(!localMapFile)
                 {                 
                    ReleaseMutex(localMutex);                
                    ReleaseMutex(globalMutex);                
                    return;                    
                 }
                 
                 
                 //create the map view and clear it
                 mappedData=(char *)MapViewOfFile(localMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_BUFFER_SIZE);
                 memset(mappedData, 0, SHARED_BUFFER_SIZE);
                 
                 mapped->alive[thisInstance]=1;
                 //release the lock
                 ReleaseMutex(localMutex);                
                 ReleaseMutex(globalMutex);                
                
                
                 alive = 1;
                 //start the receive loop
                 receiving = 1;
                 
                 CreateThread(NULL, 0, receivePoll, (void *)this, 0, 0);                                                                    
        }
        
        
        //return 0 if the lock cannot be obtained, 1 if it can
        int SharedMem::lockShared()
        {
        
                
            //return 0 immediately if someone else owns it
            if(WaitForSingleObject(sharedLuaLock, 0))
                return 0;
                
                            
            return 1;
                                                            
        }
        
        //always succeeds
        int SharedMem::unlockShared()
        {                
            ReleaseMutex(sharedLuaLock);
            return 1;                 
        }
        
        void SharedMem::pushInstances(lua_State *state)
        {
            //get the lock
            int result = WaitForSingleObject(globalMutex, 150);
            
            if(result==WAIT_OBJECT_0 && mapped)
            {
                //push all the instance names
                lua_newtable(state);
                
                //make sure we store this instances ID
                lua_pushstring(state, "this");
                lua_pushstring(state, mapped->instances[thisInstance]);
                lua_settable(state, -3);
                
                int index = 1;
                for(int i=0;i<mapped->nInstances;i++)
                {
                    if(mapped->alive[i])
                    {
                        lua_pushnumber(state, index);
                        lua_pushstring(state, mapped->instances[i]);                    
                        lua_settable(state,-3);                        
                        index++;
                    }
                }            
            }
            ReleaseMutex(globalMutex);
        }
        
        
        void SharedMem::sendToInstance(char *name, char *msg, int msglen)
        {
                               
            int len;      
            
            int result;
            
            
            //check if the given name is valid
            result = WaitForSingleObject(globalMutex, 150);
            
            if(result!=WAIT_OBJECT_0)
            {
                 ReleaseMutex(globalMutex);
                return;
            }
            
            int valid = 0;
            for(int i=0;i<mapped->nInstances;i++)
            {
                if(mapped->alive[i]&&!strcmp(name, mapped->instances[i]))
                {
                    valid = 1;
                }
            }
            
            ReleaseMutex(globalMutex);
            
            if(!valid)
                return;
                
            

            //escape the zeros
            char *escaped = new char[msglen*2+1];
            escape(msg, msglen, escaped, msglen*2+1);
            
            char *from = mapped->instances[thisInstance];
            
            
            len =strlen(escaped)+strlen(from)+2;
            
            //make sure we don't overrun the boundary
            if(len>SHARED_BUFFER_SIZE)
            {
                delete [] escaped;
                return;
            }
                
                
            
            //create the packet
            char *msgBlock = new char[len];
            strcpy(msgBlock, from);
            strcat(msgBlock, ":");
            strcat(msgBlock, escaped);
            delete [] escaped;
                        
            
            
            //create the mutex + event names
            char remoteMutexName[512], remoteEventName[512],remoteFinishedEventName[512] ;
            snprintf(remoteMutexName,sizeof(remoteMutexName), "VstLuaLock%s", name);                
            snprintf(remoteEventName,sizeof(remoteEventName), "VstLuaEvent%s", name);                            
            snprintf(remoteFinishedEventName,sizeof(remoteFinishedEventName), "VstLuaFinished%s", name);                            
            
            //get the mutex + event handles
            HANDLE remoteMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, remoteMutexName);
            HANDLE remoteEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE,remoteEventName);
            HANDLE remoteFinishedEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, remoteFinishedEventName);
            
            
                                               
            if(remoteMutex && remoteEvent && remoteFinishedEvent)
            {
            
            
                //obtain the lock
                result = WaitForSingleObject(remoteMutex, 100);
                
                if(result==WAIT_OBJECT_0)
                {    
                    
            
                    //map the file
                    char remotePageName[512];
                    snprintf(remotePageName, sizeof(remotePageName), "%s", name);
                    HANDLE remoteMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, remotePageName);                
                    
                    
                    if(remoteMapFile)
                    {
            
                        char *remoteMap = (char *)MapViewOfFile(remoteMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_BUFFER_SIZE);
                        
                        if(remoteMap)
                        {
                            //write in the message
                            strcpy(remoteMap, msgBlock);
                            //remove the view
                            UnmapViewOfFile(remoteMap);
                        }                
                        CloseHandle(remoteMapFile);                                
                    }
                    
                    ResetEvent(remoteFinishedEvent);
                    //allow the receiving thread to continue
                    SetEvent(remoteEvent);
                    ReleaseMutex(remoteMutex);     
                    //wait for receiving thread to finish
                
                    WaitForSingleObject(remoteFinishedEvent, 150);
                
                }
                //close the handles we obtained
                CloseHandle(remoteMutex);
                CloseHandle(remoteEvent);
                CloseHandle(remoteFinishedEvent);
                
            }
            delete [] msgBlock;
            
        }
        
        
        //the receive loop
        DWORD WINAPI receivePoll(LPVOID parameter)
        {
            //get the instance
            SharedMem *instance = (SharedMem*)parameter;
            
            while(instance->receiving)
            {
            
                //block until we get signalled by a remote process
                
                WaitForSingleObject(instance->localEvent, INFINITE);
                
            
                if(instance->receiving)
                {
                    
                    WaitForSingleObject(instance->localMutex, INFINITE);
                    //copy any new messages                                        
                    if(instance->alive && strlen(instance->mappedData)>0)
                    {
                        int len = strlen(instance->mappedData);
                        char *msg = new char[len+1];
                        strcpy(msg, instance->mappedData);
                                                
                        instance->mappedData[0] = '\0';
                        SetEvent(instance->localFinishedEvent);
                        ReleaseMutex(instance->localMutex);
                     
                    
                        
                        //find the name:msg break and split on it
                        char *message="", *name="";
                        
                        for(int i=0;i<len;i++)
                        {
                            if(msg[i]==':')
                            {
                                msg[i]='\0';
                                name = msg;
                                message = &(msg[i+1]);
                                break;
                            }                    
                        }
                        
                        char *unescaped = new char[len+1];
                        int realLen = unescape(message, unescaped, len+1);
                        
                        
                        
                        instance->messageCallback(name, unescaped, realLen);
                        delete [] unescaped;
                        delete [] msg;                    
                    }
                    else
                    {
                            instance->instance->normal_output("********** NULL MESSAGE! ! ************\n");
                            SetEvent(instance->localFinishedEvent);
                            ReleaseMutex(instance->localMutex);                     
                    }

                    
                }       
            
            }
            return 0;
        }
        
        
        void SharedMem::messageCallback(char *name, char *msg, int len)
        {
            instance->messageCallback(name, msg, len);        
        }
        
        SharedMem::~SharedMem()
        {
            //Allow the wait loop to exit...
            alive = 0;
            receiving = false;
            mapped->alive[thisInstance] = 0;
            SetEvent(localEvent);
            Sleep(100);
            

            if(mapped)
                UnmapViewOfFile(mapped);
            if(mappedData)
                UnmapViewOfFile(mappedData);
            if(mapFile)
                CloseHandle(mapFile);
            if(localMapFile)
                CloseHandle(localMapFile);   
            if(localMutex)
                CloseHandle(localMutex);
            if(globalMutex)
                CloseHandle(globalMutex);
        }
   




#endif
