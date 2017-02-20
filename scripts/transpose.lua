    function midiEventCb(midiEvent)
            if midiEvent.type==midi.noteOn or midiEvent.type==midi.noteOff then                
                local note = midiEvent.byte2
                                
                -- decide whether to transpose it up or down
                if note>=noteToNumber('C-5') then
                    note = midiEvent.byte2 + interval.fifth
                else
                   note = note - interval.fifth
                end
                
                midiEvent.byte2=note               
            end                
            --pass on original note unchanged
            sendMidi(midiEvent)        
            
    end