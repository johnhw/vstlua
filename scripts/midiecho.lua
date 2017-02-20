tempo = 120


function midiEventCb(midiEvent) 
 if midiEvent.type==midi.noteOn then
        ev = copyMidiEvent(midiEvent)
        ev.byte3 = ev.byte3/4
        ev.byte2 = ev.byte2
        ev.delta = ev.delta + VSTSampleRate/((tempo*2)/60)
        
        sendMidi(ev)
    end
 
    if midiEvent.type==midi.noteOff then
                ev = copyMidiEvent(midiEvent)
                ev.byte3 = 0
                ev.byte2 = ev.byte2
                ev.delta = ev.delta + VSTSampleRate/((tempo*2)/60)
                sendMidi(ev)
    end
 
    
   sendMidi(midiEvent)
   
end


function onFrameCb()
    hostTime = getHostTime()
    tempo = hostTime.tempo

end