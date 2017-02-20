

vprint("Tests the shared memory communication\nSet the node name in the gui panel!")



function openCb()       
   listctrl = guiAddControl({x=50, y=90, type=GUITypes.kickButton,  label="List"})            
   sendctrl = guiAddControl({x=120, y=90, type=GUITypes.kickButton,  label="Send"})         
   namectrl = guiAddControl({x=50, y=200, type=GUITypes.text,  label="Alias"})         
   
end


--pipes all incoming messages to the instance called receiver
function midiEventCb(event)
    sendMidiToInstance("receiver", event)

end

--print out incoming messages
function messageCb(name, msg)
    vprint("\n")
    
    --either use the alias, or the plain name if it doesn't have one
    if sharedMemAliases[name] then
        
        vprint(sharedMemAliases[name].." sent "..msg.."\n")
    else
        vprint(name.." sent "..msg.."\n")
    end
end


--gui callbacks
function valueChangedCb(tag, value, text)

    --list all known aliases
    if tag==listctrl and value==1 then
        print(sharedMemAliases)
    end
    
    --send a message to all instances
    if tag==sendctrl and value==1 then
        instances = getInstances()
        for i,v in ipairs(instances) do            
            sendToInstance(v, "Hello!")
        end
       
        
    end
    
    --set the name of this instance
    if tag==namectrl then
        setSharedMemAlias(text)
    
    end

end

