



_sharedProxy = {aliases={}}


sharedMemory = {
read = function(t,k) 
    str = _sharedRead()
    assert(loadstring(str))()
    return _sharedProxy
end,

write = function()
    _sharedWrite(serializeTable("_sharedProxy", _sharedProxy, {}))
    end,
    
lock= function()
    _sharedLock()    
    end,

unlock= function()
    _sharedUnlock()    
    end      
}



sharedMemReverseAliases = {}


--handle a shared message callback. Returns nil if it didn't process it, 1 otherwise
function handleSharedCallbacks(name, msg)

    if msg=='__ALIAS_SET__' then
        updateSharedMemAliases()
        return 1
    end
    
    
    --check for midi messages 
    local match, packed
    match = "^__MIDI_MSG__(.*)"
    _,_,packed = string.find(msg, match)    
    if packed then
        local e = {}
        e.type, e.channel, e.byte2, e.byte3, e.byte4, e.delta, e.noteOffset, e.noteLength, e.detune, e.noteOffVelocity, e.sysex = struct.unpack('BBBBBLLLLBBc0', packed)
        e = completeMidiFields(e)
		e.inPort={'instance', name}
        newMidiEvent(e)
        return 1
    end
    
    

    return nil
end

--sends midi to another instance. 
function sendMidiToInstance(instance, event)
    local event = completeMidiFields(event)
    
  
    
    local packed = "__MIDI_MSG__"..struct.pack('BBBBBLLLLBBc0', event.type, event.channel, event.byte2, event.byte3, event.byte4, event.delta, event.noteOffset, event.noteLength, event.detune, event.noteOffVelocity, string.len(event.sysex), event.sysex)
    
    
    if sharedMemReverseAliases[instance] then
        instance = sharedMemReverseAliases[instance]
    end
    
    sendToInstance(instance, packed)
end

--broadcast a message to all listening instances
function broadcastShared(name)
    local instances = getInstances()
    
    for i,v in ipairs(instances) do
        sendToInstance(v, name)
    end
    
end


sharedMemAliases = {}
--called when a set alias event is received. copies alias data into local cached copy
function updateSharedMemAliases()
    sharedMemory.lock()
    sharedMemory.read()
        
    sharedMemory.unlock()
    sharedMemAliases = _sharedProxy.aliases
    
    --add the reverse mapping
    sharedMemReverseAliases = {}  

    
    for i,v in pairs(sharedMemAliases) do    
        sharedMemReverseAliases[v] = i
    end
   

end

--sets the alias for a this name
function setSharedMemAlias(name)    
    instances = getInstances()
    thisName = instances.this
    sharedMemory.lock()
    sharedMemory.read()
    
    if not _sharedProxy.aliases then
        _sharedProxy.aliases = {}
        
    end
    
    _sharedProxy.aliases[thisName] = name
    
    
    sharedMemory.write()
    sharedMemory.unlock()
    
    --message all instances...
    instances = getInstances()
    if instances then 
        for i,v in ipairs(instances) do                
            sendToInstance(v, "__ALIAS_SET__")
        end
    end
end