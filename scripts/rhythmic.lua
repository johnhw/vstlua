


function computeNextBeat(samplePos, tempo, sampleRate)
    oneBeatLength = sampleRate * (60/tempo)   
    beatTime = samplePos/oneBeatLength
    offBeat = beatTime - math.floor(beatTime)
    timetoNextBeat = 1.0 - offBeat                
    timeinFrames = (timetoNextBeat*oneBeatLength)    
    return timeinFrames
end


counter = 0
function onFrameCb(frameData)
    hosttime = getHostTime()
    
    beatTime = computeNextBeat(hosttime.samplePos, hosttime.tempo, hosttime.sampleRate)
    
    if beatTime <= VSTFrameLength + 1 then    
                    
        --compute duration
        if counter%2==0 then 
            duration = oneBeatLength/4
        else
            duration = oneBeatLength/2
        end
        
        counter = counter + 1
        
        --note on
        event = noteOn(noteToNumber('C-3'),127,0)
        event.delta = beatTime
        sendMidi(event)
        
        --note off
        event = noteOff(noteToNumber('C-3'),0)
        event.delta = timeinFrames + duration
        sendMidi(event)
                             
    end       
end