--Qauntize events


vprint("Quantizes midi note/off in realtime (i.e. delays until next beat).\n")
vprint("Choose quantize level in GUI panel\n")
vprint("Only quantizes if host is playing...\n")
speeds = {1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125}
speed = 0.25

function openCb()
   newcontrol = {x=20, y=50, type=GUITypes.menu,  label="Speed", menu = {"1/1", "1/2", "1/4", "1/8", "1/16", "1/32"}}
    menu = guiAddControl(newcontrol)
    guiSetValue(menu, 2)
end



function midiEventCb(midiEvent)
    if midiEvent.type==midi.noteOn or midiEvent.type==midi.noteOff then        
        beat, beatTime = computeNextQBeat(hosttime.samplePos, hosttime.tempo, hosttime.sampleRate, speed)                    
        midiEvent.delta = beatTime
        scheduleEvent(midiEvent)
    else
        sendMidi(midiEvent)
    end
        
end


function valueChangedCb(tag, value) 
    if tag==menu then
        speed = speeds[value+1]
    end
end


counter = 0

function onFrameCb()
    hosttime = getHostTime()    
    
end