

vprint("Sends the midi input from this instance to all other instances, using the shared memory protocol.\n")


function messageCb(name, msg)
    --parse the incoming table    
    
    loadstring(msg)()
    sendMidi(remoteMidiEvent)
end

function midiEventCb(midiEvent)

    instances = getInstances()
    event_string = serializeTable('remoteMidiEvent', midiEvent)
    --look for another instance that isn't this one
    for i,v in ipairs(instances) do
        if v~=instances.this then    
            
            sendToInstance(v, event_string)            
        end
    end

end