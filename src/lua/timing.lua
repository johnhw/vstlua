

--Compute time to next beat, and the index of that beat. BeatLength 
function computeNextQBeat(samplePos, tempo, sampleRate, beatLength)
    local oneBeatLength = sampleRate * (60/(tempo)) * beatLength
    local beatTime = samplePos/oneBeatLength
    local offBeat = beatTime - math.floor(beatTime)
    local timetoNextBeat = 1.0 - offBeat                
    local timeinFrames = (timetoNextBeat*oneBeatLength)   
    
    return math.floor(beatTime), timeinFrames
end



--Compute time to next beat, and the index of that beat. 
function computeBeatTime(samplePos, tempo, sampleRate, beatLength)
    local oneBeatLength = sampleRate * (60/(tempo)) * beatLength
    local beatTime = samplePos/oneBeatLength
    local offBeat = beatTime - math.floor(beatTime)
    local timetoNextBeat = 1.0 - offBeat                
    local timeinFrames = (timetoNextBeat*oneBeatLength)       
    return oneBeatLength, math.floor(beatTime), timeinFrames
end

