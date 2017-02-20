

--Compute beat times


curDrumType = 0

function openCb()
    --Create the gui
    kicks = {}
    leds = {}
    x = 20
    for i=1,16 do
        if i%4==1 then 
            label="!"
        else
            label=""
        end
        newcontrol = {x=x, y=90, type=GUITypes.onOffButton,  label=label}
        kicks[i] = guiAddControl(newcontrol)
        
        newcontrol = {x=x, y=40, type=GUITypes.stickLed,  label=""}
        leds[i] = guiAddControl(newcontrol)
        
        
        
        x = x + 40         
        guiSetValue(kicks[i], getVSTParameter(i-1))             
              
    end
    drumType = {x=20, y=120, type=GUITypes.menu,  label="Drum", menu={"Kick", "Snare", "Hihat", "Tom"}}
    dt = guiAddControl(drumType)   

    
    
end



--store values in vstparameters to presets work
function valueChangedCb(tag, value)       
    if tag==dt then
         vprint(""..curDrumType)
        curDrumType = value
        vprint(""..value)
        for i=1,16 do
            guiSetValue(kicks[i], getVSTParameter(i-1+curDrumType*16))
        end
    end

    for i=1,16 do
    
        if tag==kicks[i] then
            setVSTParameter(i-1+16*curDrumType, value)
        end
    end
    
    if tag==sw then
        setVSTParameter(65, value)
        theSwing = value
    end
    
end

--restore values from preset if needed
function initCb()
   
end




function onFrameCb(frameData)
    hosttime = getHostTime()    
    beat, beatTime = computeNextQBeat(hosttime.samplePos, hosttime.tempo, hosttime.sampleRate, 0.25)            
               
    
    if beatTime <= VSTFrameLength + 1 then                        
        vbeat = beat % 16                
        
            if leds and leds[vbeat+1] then
                guiSetValue(leds[vbeat+1], 1)
                last = vbeat
                if last==0 then
                    last = 16
                end
                guiSetValue(leds[last], 0)
                guiRedraw(leds[last])
                guiRedraw(leds[vbeat+1])
            end
        
            for drum = 1,4 do
                --get the button state
                on = getVSTParameter(vbeat+16*(drum-1))
                
                if on==1.0 then
                    --note on
                    event = noteOn(noteToNumber('C-3')+drum,127,0)
                    event.delta = beatTime
                    scheduleEvent(event)
                
                    --note off
                    event = noteOff(noteToNumber('C-3')+drum,0)
                    event.delta = beatTime + 256
                    scheduleEvent(event)
                end        
        end                             
    end       
end