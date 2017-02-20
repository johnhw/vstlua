--simple example of how CC's can be remapped
vprint("Simple example of how CC's can be remapped\n")
vprint("Maps cc 1 to 74, and cc 2 to to 75\n")

--map cc 1 to 74, and cc 2 to to 75
ccmap_table = {[1]=74, [2]=75}

function midiEventCb(midiEvent) 
    if midiEvent.type==midi.cc then
            if ccmap_table[midiEvent.byte2] then                
                midiEvent.byte2 = ccmap_table[midiEvent.byte2]
            end
    end        
   sendMidi(midiEvent)   
end
