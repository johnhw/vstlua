

vprint("Graphical groove editor \n")

swing_ctrl = {}
vel_ctrl = {}
swing = 0.5
bypass = 0


function openCb()
    
   
   newcontrol = {x=50, y=5, type=GUITypes.onOffButton,  label="Bypass"}
   bypassctrl = guiAddControl(newcontrol)   
   guiSetValue(bypassctrl, bypass)
   
   x = 50
   for i=1,16 do   
    swing_ctrl[i] = guiAddControl({x=x,y=100,type=GUITypes.vSlider, label=""..i})   
    vel_ctrl[i] = guiAddControl({x=x,y=220,type=GUITypes.vSlider, label=""..i})   
    guiSetValue(vel_ctrl[i], 1.0)
    setVSTParameter(i, 0.0)
    setVSTParameter(i+16, 1.0)
    x = x + 18
   end
   
   
end


function valueChangedCb(tag, value, text)
   
    for i=1,16 do
    
        if tag==swing_ctrl[i] then
            setVSTParameter(i, value)
        end
    
        if tag==vel_ctrl[i] then
            setVSTParameter(i+16, value)
        end
        
        
    end
    
    if tag==bypassctrl then
        bypass = value        
    end

end


function midiEventCb(midiEvent) 
    
    if bypass==0 then
    
        --make sure swing time is linked to the beat time
        swingtime = hosttime.sampleRate * (15/hosttime.tempo) * swing
                
        
        --kicks
        if (midiEvent.type==midi.noteOn or midiEvent.type==midi.noteOff) then   

            
                beat, beatTime = computeNextQBeat(hosttime.samplePos+midiEvent.delta, hosttime.tempo, hosttime.sampleRate, 0.25)                    
                vbeat = (math.floor(beat) % 16) + 1
                
                --velocity...
                midiEvent.byte3 = midiEvent.byte3 * getVSTParameter(16+vbeat)
                
                --timing
                midiEvent.delta = midiEvent.delta + getVSTParameter(vbeat)*swingtime
                                        
            
        end    
    end
    
        scheduleEvent(midiEvent)
   
end

function onFrameCb(frameData)
    hosttime = getHostTime()       
end
