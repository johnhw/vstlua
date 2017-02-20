vprint("Slight pitch variations in notes. Originally by austinfx @ kvr\n")


--START
--bendWidth is the max amount of possible bend - change as needed, or
--learn how to put in a GUI control
bendWidth = 300
function midiEventCb(midiEvent)
    if midiEvent.type==midi.noteOn then
        --math.random() returns a number between 0 and 1
        bendValue = (math.random()-0.5)*bendWidth
        --use the same channel as the event that was input
        eventToSend = pitchBend( bendValue+8192 , midiEvent.channel )
        sendMidi( eventToSend )
        
        --add a little velocity variation
        midiEvent.byte3 = midiEvent.byte3 * (0.6 + math.random()*0.4)
        --and a little timing variation
        midiEvent.delta = midiEvent.delta + 1000 * math.random()
    end
    --send the original event you played, whatever it may be
    --if it was a noteOn, then at least you've already set the pitch bend
    --amount!
    scheduleEvent(midiEvent)
end
--END