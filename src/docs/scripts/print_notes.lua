
function midiEventCb(midiEvent) 
 if midiEvent.type==midi.noteOn then
        vprint("Note on "..numberToNote(midiEvent.byte2).." "..midiEvent.byte3.."\n")
    end
   sendMidi(midiEvent)
end


