

--simple keytracking for synths that respond to CC74
function midiEventCb(event)

    --send the chorded note
    if event.type==midi.noteOn then
        mevent = setCC(74, event.byte2/2, event.channel)
        mevent.delta = event.delta
        sendMidi(mevent)
    end
    
    sendMidi(event)
        
end
