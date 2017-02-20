--Compute beat times

function openCb()
    --Create the gui
    kicks = {}
    x = 20
    for i=1,16 do
        if i%4==1 then 
            label="!"
        else
            label=""
        end
        newcontrol = {x=x, y=60, type=GUITypes.onOffButton,  label=label}
        kicks[i] = guiAddControl(newcontrol)
        x = x + 40         
        guiSetValue(kicks[i], getVSTParameter(i-1))             
    end    
end

--store values in vstparameters to presets work
function valueChangedCb(tag, value)
    setVSTParameter(tag, value)
end

--restore values from preset if needed
function initCb()
   
end



function onFrameCb(frameData)
    hosttime = getHostTime()    
    beat, beatTime = computeNextQBeat(hosttime.samplePos, hosttime.tempo, hosttime.sampleRate, 0.25)            
    
    if beatTime <= VSTFrameLength + 1 then                        
        vbeat = beat % 16            
        --get the button state
        on = getVSTParameter(vbeat)
        
        if on==1.0 then
            --note on
            event = noteOn(noteToNumber('C-3'),127,0)
            event.delta = beatTime
            sendMidi(event)
        
            --note off
            event = noteOff(noteToNumber('C-3'),0)
            event.delta = beatTime + 256
            sendMidi(event)
   
        end
        
        
                             
    end       
end