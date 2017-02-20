



help = {}
--callback wrappers

function _helpCb()

    
    
    if helpCb then
        return helpCb()
    else
        hs = ""
    
        if help then 
            if help.title then
                hs = "-- "..help.title.." --".."\n\n"
            end
            
            if help.description then
                hs = hs..help.description
            end             
                
            if help.author then
                hs = hs.."\n\nAuthor: "..help.author
            end
        end
        
        if hs=="" then
            return "No help for this script. Complain to the author!"
        else
            return hs
        end
    end
    
    
end

function setVSTParameter(par, value)
    _setVSTParameter(par, value)
end

function getVSTParameter(par)
    return _getVSTParameter(par)
end






function sendMidi(event)    

    
    
    --record which notes are currently down
    if event.type==midi.noteOn then
        outputNotesDown[event.byte2+128*event.channel] = event.byte3
        
    end
    
    
    if event.type==midi.noteOff then
        outputNotesDown[event.byte2+128*event.channel] = nil
    end
    
    
    --record cc values
    if event.type==midi.cc then
        outputCC[event.byte2+128*event.channel] = event.byte3
    end
    
    
    
    
    return _sendMidi(event)
    
end



windowOpen = false

function _openCb()
	windowOpen = true
    
    
	
	--add all controls waiting to be added
	for i,v in pairs(controlsToAdd) do
		doAddControl(v)
	end
	
    if openCb then
        openCb()
    end
end

function _closeCb()
	windowOpen = false
    if closeCb then
        closeCb()
    end
end

function _valueChangedCb(tag, value, str)
    
    --look for matching tags and execute the right callbacks
    v = guiControlTable[tag]
    if v then                
        if v.callback then
            v.callback(guiScaleValue(value, v), str)
        end        
        if v.vstparameter then
            setVSTParameter(v.vstparameter, value)
        end       
    end
    
    if valueChangedCb then
        valueChangedCb(tag, value, str)
    end
end


_keysDown = {}
function _keyCb(key)  
    
    code = key.character + 256 * key.virtual + 65536*key.modifier    
    
    if keyCb then            
        if key.down==1 then         
            if _keysDown[code] then 
                -- do nothing if it's already down
            else
                keyCb(key)
                _keysDown[code] = 1
            end            
        else --key up                
            
            if _keysDown[code] then
                _keysDown[code] = nil
                keyCb(key)                
            else
                -- do nothing if it's already up
            end                        
        end    
    end
end

function _dropCb(drop)
    if dropCb then
        dropCb(drop)
    end
end


function _onFrameCb()


    setMaxExecute(_frameAbortLimit)    
    curHostTime = getHostTime()
    if onFrameCb then
        onFrameCb()
    end
    doSchedule(VSTFrameLength)
    
    setMaxExecute(_abortLimit)
end


function newMidiEvent(midiEvent)


    --add the aliased accessors
    addMidiMetaTable(midiEvent)
    
    --zero velocity to note off mapping
    if midiEvent.type==midi.noteOn and midiEvent.velocity==0 then
        midiEvent.type = midi.noteOff    
    end
    
   

    --record which notes are currently down
    if midiEvent.type==midi.noteOn then
        inputNotesDown[midiEvent.byte2+128*midiEvent.channel] = midiEvent.byte3
    end
    
    if midiEvent.type==midi.noteOff then
        inputNotesDown[midiEvent.byte2+128*midiEvent.channel] = nil
    end
    
    --record cc values
    if midiEvent.type==midi.cc then
        inputCC[midiEvent.byte2+128*midiEvent.channel] = midiEvent.byte3
    end
       
    
    --deal with filters
    handleFilters(midiEvent)
    
    
    if midiEventCb then
        midiEventCb(midiEvent)
    elseif #midiFilters==0 then
        sendMidi(midiEvent)
    end
    
end

function _nativeMidiEventCb(midiEvent)
	midiEvent.inPort = {'native', getNativeMidiName(midiEvent.port_id)}   
    newMidiEvent(midiEvent)
end

function _midiEventCb(midiEvent)    
    midiEvent.inPort = {'host', 0}   
    newMidiEvent(midiEvent)
end

function _setParameterCb(par, value)


    --make sure gui control mappings are updated
    mappings = guiVSTMappings[par]   
    if mappings then
        for i,v in pairs(mappings) do
            --call the gui callback for this control with the new value, just as if the user had moved it             
            if v.control then 
                guiSetValue(v.control, value)               
            end
            v.callback(guiScaleValue(value,v))
        end    
    end
    if setParameterCb then
        setParameterCb(par, value)
    end
end




function _setParameterReservedCb(par, value)


    --check if we need to load a script    
    if par==0 and value>0 then
        script = findFileFromUid(value)
        --load it
        if script then
            reset(script)
        end
    end

    if setParameterReservedCb then
        setParameterReservedCb(par, value)
    end
end





function _messageCb(name, msg)

    --handle specific communication events. Should return 1 if it consumed it, nil otherwise
    if not handleSharedCallbacks(name, msg) then
        if messageCb then
            messageCb(name, msg)
        end
    end
end



function _initCb()

    
    setMaxExecute(_abortLimit)
    
    --makes sure aliases stay synced
    updateSharedMemAliases()
        
                           
    
    loadGlobalPersistence()

    loadPersistentProgramData()
    
    programData.scriptName = ScriptName
    
    if initCb then
        initCb()
    end
    
    --make sure script is aware of current program status

    
end    


function openFileSelector(filetype, func)
    filetype = filetype or "*."
    func = func or nil
    return _openFileSelector(filetype, func)
end



function _saveProgramCb()
     savePersistentProgramData()
    if saveProgramCb then
        saveProgramCb()
   end 
end




function _loadProgramCb()       
    
    loadPersistentProgramData()
    
    if programData.scriptName then       
        reset(programData.scriptName)  
    else       
        reset()
    end
    
    if loadProgramCb then
        loadProgramCb()
    end
end


function _networkReceiveCb(data, ip, port)
    if networkReceiveCb then
        networkReceiveCb(data,ip,port)
    end
end



function _resetCb()

    --make sure program data is recorded
    savePersistentProgramData()
    
    setSharedMemAlias(nil)
       
   
   
   
    
    if resetCb then
        resetCb()
    end
    saveGlobalPersistence()
end