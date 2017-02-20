vprint("Pitch drifting. Host must be running. Receiver should have polyphonic pitchbend.\n")


skip=0
v1 = 8192
v2 = 8192

--duplicate notes across channels 1 + 2
function midiEventCb(midiEvent)
    if midiEvent.type==midi.noteOn or midiEvent.type==midi.noteOff then                        
            c = copyMidiEvent(midiEvent)
            c.channel = 1
            v1 = 8192
            v2 = 8192
            scheduleEvent(c)
        end
        midiEvent.channel=0
        sendMidi(midiEvent)
      
            
end


function onFrameCb(frameData)

    skip=skip+1
    
    --don't send every frame, keep the messages reasonable
    if skip==4 then
        
        
    
        v1 = v1 + (math.random() - 0.5) * 100

        ev = pitchBend(v1, 0)
        sendMidi(ev)
        v2 = v2 + (math.random() - 0.5) * 100
        ev = pitchBend(v2, 1)
        sendMidi(ev)
        skip = 0
    end
    
end