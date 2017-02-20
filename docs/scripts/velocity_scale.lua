--simple example of how CC's can be remapped
vprint("Set the response curve of velocities\n")
vprint("Use knob on gui panel to adjust\n")

function openCb()
    newcontrol = {x=50, y=5, type=GUITypes.knob,  label="Response"}
    responsectrl = guiAddControl(newcontrol)   
    guiSetValue(responsectrl, getVSTParameter(0))
    
    vuctrl= guiAddControl{x=120, y=5, type=GUITypes.vuMeter, label="Velocity"}
    
    
    guiSetSize({width=200,height=100})
end



function valueChangedCb(tag, value, text)
    if tag==responsectrl then
        setVSTParameter(0, value)    
    end    
end

function midiEventCb(midiEvent) 
    if midiEvent.type==midi.noteOn then
            coeff = (2*(1-getVSTParameter(0)))
            midiEvent.byte3 = ((midiEvent.byte3 / 128.0) ^ coeff) * 127            
            guiSetValue(vuctrl, midiEvent.byte3/128.0)
    end        
    if midiEvent.type==midi.noteOff then
        guiSetValue(vuctrl, 0)
    end
    
   sendMidi(midiEvent)   
end
