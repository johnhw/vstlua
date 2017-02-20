vprint("Algo-music, using the same algorithim as MusiNum")



function midiEventCb(event)

end



function getNote(v, mode)

    local offset = 0
    mode_notes = modes[mode]
    while v>#mode_notes do
        offset = offset + 12
        v = v - #mode_notes
    end
    
    return mode_notes[v]+offset

end

function playNote(nv, beatTime, beatLen) 
            on = noteOn(nv,127,0)
            on.delta = beatTime
            off = noteOff(nv, 0)
            off.delta = beatTime + beatLen
            scheduleEvent(on)
            scheduleEvent(off)      
end


function countBits(b)
     --count the bits
   local     sh = b
   local        qv = 0
        
        for i=1,31 do
            qv = qv + bit.band(sh,1)
            sh = bit.rshift(sh,1)
        end
   return qv
end

n1=0
note1 = 0
n2 = 8
note2 = 0


function onFrameCb(frameData)
    hosttime = getHostTime()    
   
    beatLen, beat, beatTime = computeBeatTime(hosttime.samplePos, hosttime.tempo, hosttime.sampleRate, 0.25)            
    
    
    
    if beatTime <= VSTFrameLength + 1 then                        
        n1 = n1 + 31
        n2 = n2 + 31
        
        
        
        q = getNote(countBits(n1), "dorian")       
        if note1~=q then
            playNote(q+48, beatTime, beatLen)            
            note1 = q            
        end
        
        q = getNote(countBits(n2), "dorian")       
        if note2~=q then
            playNote(q+67, beatTime, beatLen)            
            note2 = q            
        end
        
        
    
                             
    end       
end