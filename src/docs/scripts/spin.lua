vprint("Spin. Repeats a note multiple times, with decay velocity and increasing timing. Like hitting a\n")
vprint("rattle and hearing it spin. Good with drums\n")

function helpCb()
    return "Spin.\n\nRepeats a note multiple times,with decay velocity and increasing timing.\nLike hitting a rattle and hearing it spin.\nGood with drums.\n\nAuthor: tzec\n"
end


function midiEventCb(midiEvent)

    if midiEvent.type==midi.noteOn then
            
        energy = midiEvent.byte3
        delay = 4000 + 100*(128-midiEvent.byte3)
        while energy>10 do
            c = copyMidiEvent(midiEvent)
            c.byte3 = energy
            c.delta = delay
            scheduleEvent(c)
            c = copyMidiEvent(midiEvent)            
            c.delta = delay * 1.19
            c.type = midi.noteOff
            scheduleEvent(c)
            
            
            
            energy = energy * 0.9
            delay = delay * 1.2
        end
        
        
    else
        sendMidi(midiEvent)
    
    end
    
        
end



