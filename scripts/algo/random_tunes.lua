--Compute beat times

function openCb()
  
end

--store values in vstparameters to presets work
function valueChangedCb(tag, value)
    
end

--restore values from preset if needed
function initCb()
   
end


n=0
function onFrameCb(frameData)
    hosttime = getHostTime()    
    beat, beatTime = computeNextQBeat(hosttime.samplePos, hosttime.tempo, hosttime.sampleRate, 1.0)            
    
    if beatTime <= VSTFrameLength + 1 then                        
        vbeat = beat % 16            
        
        --move along the circle of fifths
        n = n + math.random(-1,1)*7
        n = n % 12
        v= 50+math.random(127)
        
        ev = noteOn(60+n, v, 0)
        ev.delta = beatTime
        sendMidi(ev)
        
        ev = noteOff(60+n,  0)
        ev.delta = beatTime+5000
        sendMidi(ev)
        
        ev = noteOn(64+n,  v)
        ev.delta = beatTime
        sendMidi(ev)
        
        ev = noteOff(64+n,  0)
        ev.delta = beatTime+5000
        sendMidi(ev)

        ev = noteOn(67+n,  v)
        ev.delta = beatTime
        sendMidi(ev)
        
        ev = noteOff(67+n,  0)
        ev.delta = beatTime+5000
        sendMidi(ev)

                             
    end       
end