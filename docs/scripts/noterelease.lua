tempo = 120

lastvels = {}

function midiEventCb(midiEvent) 
 
    --store the last velocity
    if midiEvent.type==midi.noteOn then
        lastvels[midiEvent.byte2] = midiEvent.byte3    
    end
    
    if midiEvent.type==midi.noteOff and lastvels[midiEvent.byte2] then
                ev = copyMidiEvent(midiEvent)
                ev.type = midi.noteOn
                ev.byte3 = lastvels[ev.byte2]
                ev.byte2 = ev.byte2 + 7
                ev.delta = ev.delta 
                scheduleEvent(ev)
                ev.type = midi.noteOff
                ev.delta = ev.delta + 20000
                scheduleEvent(ev)
                
    end
 
    
   sendMidi(midiEvent)
   
end


function onFrameCb()
    hostTime = getHostTime()
    tempo = hostTime.tempo

end