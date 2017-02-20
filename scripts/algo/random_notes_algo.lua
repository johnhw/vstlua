--Contributed by Robert Lindeman

function computeNextBeat(samplePos, tempo, sampleRate)
    oneBeatLength = sampleRate * (120/tempo)   
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
    scale_mode = {0,2,4,5,7,9,11,12} --major scale
    if beatTime <= VSTFrameLength + 1 then    
                    
        
        --compute duration
        if counter%2==0 then 
            duration = oneBeatLength/8
        else
            duration = oneBeatLength/4
        end
        
        counter = counter + 1
        
        --random note, scale it
        	generated_note = math.random(60,72)
			for c in pairs(scale_mode) do
    				if generated_note%12 == scale_mode[c] then
					vprint(generated_note.."    ")
					scaled_note = generated_note
		
				
	
	--note on
        event = noteOn(scaled_note,127,0)
        event.delta = beatTime
        scheduleEvent(event)
        
        
        --note off
        event = noteOff(scaled_note,0)
        event.delta = beatTime + duration
        scheduleEvent(event)
        
        
    		end
			end

        
    end       
end

