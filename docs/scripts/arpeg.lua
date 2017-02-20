--Simple arpeggiator


notes_on = {}
speeds = {1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125}
notes_held = 0
speed = 1.0

function openCb()
   newcontrol = {x=20, y=50, type=GUITypes.menu,  label="Speed", menu = {"1/1", "1/2", "1/4", "1/8", "1/16", "1/32"}}
    menu = guiAddControl(newcontrol)
end



function midiEventCb(midiEvent)
    if midiEvent.type==midi.noteOn then
        notes_held = notes_held+1
        notes_on[notes_held] = midiEvent.byte2                
    end
    
    if midiEvent.type==midi.noteOff then
    
        rem_note = nil
        for i,n in ipairs(notes_on) do
            if n==midiEvent.byte2 then
               rem_note = i
                
            end
        end
        
        if rem_note then        
            table.remove(notes_on, rem_note)
            notes_held = notes_held-1   
        end
    
        
        
        
        midiEvent.delta = midiEvent.delta
        sendMidi(midiEvent)
    end
        
end


function valueChangedCb(tag, value) 
    if tag==menu then
        speed = speeds[value+1]
    end
end

--restore values from preset if needed
function initCb()  
end


counter = 0
function onFrameCb(frameData)
    hosttime = getHostTime()    
    beat, beatTime = computeNextQBeat(hosttime.samplePos, hosttime.tempo, hosttime.sampleRate, speed)            
    
    if beatTime <= VSTFrameLength + 1 then    
        
        
        if notes_held>0 then
            if counter>notes_held-1 then
                counter = notes_held-1
            end
            n = notes_on[counter+1]            
            event = noteOn(n,127,0)            
            event.delta = beatTime+1
            sendMidi(event)     
            
             --turn off last note
            last = counter
            if last==0 then
                last = notes_held
            end
            n = notes_on[last]            
            event = noteOff(n,0)
            event.delta = beatTime
            sendMidi(event)
            
           
        end
        
        
            
        counter = counter + 1
        if counter>=notes_held then
            counter = 0
        end
        
                             
    end       
end