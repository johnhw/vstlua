vprint("16 channels pitch drifting. Sort of simulates the effect in Ligeti's 'Atmospheres' if you use a choir patch. Receiver should have polyphonic pitchbend.\n")


skip=0

bend = {}
phase = {}
freq = {}
for i=1,16 do    
    phase[i] = math.random() * math.pi * 2
    freq[i] = math.random() * 20 + 20
end

--duplicate notes across all channels!
function midiEventCb(midiEvent)
    if midiEvent.type==midi.noteOn or midiEvent.type==midi.noteOff then                        
                
            for i=1,16 do
                c = copyMidiEvent(midiEvent)
                c.channel = i-1
                scheduleEvent(c)
            end
                  
    else
        
        sendMidi(midiEvent)
    end
            
end


ctr = 0
function onFrameCb(frameData)

    skip=skip+1
    
    --don't send every frame, keep the messages reasonable
    if skip==4 then                
        for i=1,16 do
        
            --maximum pitch bending!
            bend[i] = 8192 + 8192 * math.sin(phase[i]+freq[i]*ctr)
            ev = pitchBend(bend[i], i-1)
            sendMidi(ev)        
        end        
        ctr = ctr + 0.001
        
        skip = 0
    end
    
end