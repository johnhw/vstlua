
--envelope trigger example
--press c2 to trigger the cc 74 envelope

vprint("Envelope trigger example\n")
vprint("Press C-2 to trigger the cc 74 envelope\n")

env = ADSR:new{a=900, d=3000, s=50, r=5000}

function makeKnob(x,y,label,param)
    newcontrol = {x=x, y=y, type=GUITypes.knob, label=label}
    knob = guiAddControl(newcontrol)
    guiSetValue(knob, getVSTParameter(param))
    valueChangedCb(knob, getVSTParameter(param), "")
    
    return knob

end

function openCb()
    
    attack_knob = makeKnob(50,50,'Attack',1)
    decay_knob = makeKnob(120,50,'Decay',2)
    sustain_knob = makeKnob(190,50,'Sustain',3)    
    release_knob = makeKnob(260,50,'Release',4)
    
    newcontrol = {x=340, y=70, type=GUITypes.transientLed, label="Trigger"}
    led = guiAddControl(newcontrol)
    
end

function valueChangedCb(control, value, text)
    if control==attack_knob then
        env.a = value * 50000 + 1
        setVSTParameter(1, value)
    end
    
    if control==decay_knob then
        env.d = value * 50000 + 1
        setVSTParameter(2, value)
    end

    if control==sustain_knob then
        env.s = value * 127 + 1
        setVSTParameter(3, value)
    end

    
    if control==release_knob then
        env.r = value * 50000 + 1
        setVSTParameter(4, value)
    end
end




function midiEventCb(midiEvent) 
 
    --store the last velocity
    if midiEvent.type==midi.noteOn and midiEvent.byte2==24 then
        env:trigger(midiEvent.byte3)
        if led then
            guiSetValue(led, 1)
        end
    
    elseif midiEvent.type==midi.noteOff and midiEvent.byte2==24 then
        env:release()
    else

        sendMidi(midiEvent)   
    end
end


function onFrameCb()
    
        if env.state>0 then 
            val = env:update(VSTFrameLength)
            
            sendMidi(setCC(74,val,0))        
        end
    
    
end