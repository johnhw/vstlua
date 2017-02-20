

vprint("Adds groove to input, but only on C-3 (kick drum) and F#3 (hihat) \n")

swing = 0.5
bypass = 0


function openCb()
    
   newcontrol = {x=50, y=90, type=GUITypes.knob,  label="Swing"}
   delayctrl = guiAddControl(newcontrol)   
   guiSetValue(delayctrl, swing)
   
   newcontrol = {x=120, y=110, type=GUITypes.onOffButton,  label="Bypass"}
   bypassctrl = guiAddControl(newcontrol)   
   guiSetValue(bypassctrl, bypass)
   
   
end


function valueChangedCb(tag, value, text)
    if tag==delayctrl then
        swing = value       
    end
    
    if tag==bypassctrl then
        bypass = value        
    end

end


function midiEventCb(midiEvent) 
    
    if bypass==0 then
    
        --make sure swing time is linked to the beat time
        swingtime = hosttime.sampleRate * (7/hosttime.tempo) * swing
                
        
        --kicks
        if (midiEvent.type==midi.noteOn or midiEvent.type==midi.noteOff) then   

            -- humanize everything
            midiEvent.byte3 = midiEvent.byte3*(0.9+math.random()*0.1)
            
            if midiEvent.byte2==36  then
                beat, beatTime = computeNextQBeat(hosttime.samplePos+midiEvent.delta, hosttime.tempo, hosttime.sampleRate, 0.25)                    
                
                --swing every second 16th note
                if beat%2==0 then        
                    midiEvent.delta = midiEvent.delta + swingtime
                    midiEvent.byte3 = midiEvent.byte3 * 0.7
                               
                end
            end
            
            --hihat
            if midiEvent.byte2==42  then
                beat, beatTime = computeNextQBeat(hosttime.samplePos+midiEvent.delta, hosttime.tempo, hosttime.sampleRate, 0.25)                            
                --swing every second 16th note hard
                if beat%2==0 then        
                    midiEvent.delta = midiEvent.delta+ swingtime 
                    midiEvent.byte3 = midiEvent.byte3 * 0.5                           
                end
                
                --damp velocity again on every third 16th
                if beat%3==0 then        
                    midiEvent.byte3 = midiEvent.byte3 * 0.5                          
                end
                
            end
        end    
    end
    
        scheduleEvent(midiEvent)
   
end

function onFrameCb(frameData)
    hosttime = getHostTime()       
end
