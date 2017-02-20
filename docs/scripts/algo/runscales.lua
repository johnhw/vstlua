--Simple arpeggiator

vprint("Play scales. Choose scales in the GUI panel\n")
vprint("Press a note to start. Use MW to control speed.\n")

speeds = {1.0, 0.5, 0.25, 0.125}
notes_held = 0
speed = 1.0
counter_dir = 1
scale_list = {}
counter = 1
note_down = 0
note_vel = 0

for k,v in pairs(scales) do
    table.insert(scale_list, k)
end
curscale = scales[scale_list[1]]


function openCb()
   newcontrol = {x=20, y=50, type=GUITypes.menu,  label="Speed", menu = {"1/1", "1/2", "1/4", "1/8"}}
   menu = guiAddControl(newcontrol)
        
   newcontrol = {x=20, y=90, type=GUITypes.menu,  label="Scale Types", menu = scale_list}
   scalemenu = guiAddControl(newcontrol)
end


function midiEventCb(midiEvent)   
    if midiEvent.type==midi.noteOn then
        
       
        --reset if not already down
        if  note_down == 0 then               
            counter = 1
            counter_dir = 1
        end
        
        note_down = midiEvent.byte2
        note_vel = midiEvent.byte3
        
    end
    
    --turn off the scale if we released the first note pushed down
    if midiEvent.type==midi.noteOff and note_down == midiEvent.byte2 then
        note_down = 0
        
    end
    
    --set speed with mw
    if midiEvent.type==midi.cc and midiEvent.byte2==1 then
        speedIndex = math.floor((midiEvent.byte3 * 4) / 128)        
        speed = speeds[speedIndex+1]
    end
        
end





function valueChangedCb(tag, value) 
    if tag==menu then
        speed = speeds[value+1]
    end
    
    if tag==scalemenu then
        curscale = scales[scale_list[value+1]]
        vprint(#curscale)
    end
end

--restore values from preset if needed
function initCb()  
end



function onFrameCb(frameData)

    if note_down>0 then 
        hosttime = getHostTime()    
        beat, beatTime = computeNextQBeat(hosttime.samplePos, hosttime.tempo, hosttime.sampleRate, speed)            
        
        if beatTime <= VSTFrameLength + 1 then    
            
                   
            n = note_down+curscale[counter]
            event = noteOn(n,note_vel,0)           
            event.delta = beatTime
            scheduleEvent(event)
            
            event = noteOff(n,0)
            event.delta = beatTime+2000
            scheduleEvent(event)                       
        
                    
                
            counter = counter + counter_dir
            if counter==#curscale  then
                counter_dir = -1            
            end
            
            if counter==1 then
                counter_dir = 1
            end
            
                                 
        end       
    end
end