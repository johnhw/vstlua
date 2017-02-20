

vprint("Tests the shared memory communication\n")


function openCb()
    
   
   listctrl = guiAddControl({x=50, y=90, type=GUITypes.kickButton,  label="List"})   
   
   
   
   sendctrl = guiAddControl({x=120, y=90, type=GUITypes.kickButton,  label="Send"})   
   
   
   
end


function messageCb(name, msg)
    vprint(name.." sent "..msg.."\n")
end

function valueChangedCb(tag, value, text)
    if tag==listctrl and value==1 then
        print_table(getInstances(), vprint, "")
    end
    
    if tag==sendctrl and value==1 then
        instances = getInstances()
        for i,v in ipairs(instances) do            
            sendToInstance(v, "Hello!")
        end
       
        
    end

end

