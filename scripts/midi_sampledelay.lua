

vprint("Delays midi in on a sample-wise basis\n")
vprint("Set the delay from the gui panel\n")

delay = 200

function openCb()

    newcontrol = {x=76, y=65, type=GUITypes.label,  label="Delay"}
    guiAddControl(newcontrol)
    
   newcontrol = {x=50, y=90, type=GUITypes.knob,  label=""..delay}
   delayctrl = guiAddControl(newcontrol)   
   guiSetValue(delayctrl, delay/10000)
end


function valueChangedCb(tag, value, text)
    if tag==delayctrl then
        delay = math.floor(value * 10000)
        guiSetLabel(delayctrl, ""..delay)
    end

end

function midiEventCb(midiEvent) 
   midiEvent.delta = midiEvent.delta  + delay
   scheduleEvent(midiEvent)   
end
