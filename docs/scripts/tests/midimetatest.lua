help.title="Metatable test"
help.description="Uses the metatables for midi access so things like event.note work"
help.author="tzec"

function midiEventCb(event)

    --note and velocity aliases
    if event.type==midi.noteOn then
        vprint(event.note.." "..event.velocity.."\n")
        event.note = 60
        event.velocity = 120
        vprint(event.byte2.." "..event.byte3.."\n")
        
    end
    
    --controller/value aliases
    if event.type==midi.cc then
        print(event.controller.." "..event.value.."\n")
    end
    
    --bend aliases
    if event.type==midi.pb then
        print(event.bend.."\n")
        event.bend = 8192        
        vprint(event.byte2.." "..event.byte3.."\n")
    end
    
    --aftertouch aliases
    if event.type==midi.at then
        print(event.aftertouch.."\n")
    end
    

end