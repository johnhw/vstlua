vprint("Plays Tom Johnson's 'Chord Catalogue': all 8178 chords within an octave.")


function playNote(nv, beatTime, beatLen) 
            on = noteOn(nv,100+math.random()*27,0)
            
            on.delta = beatTime 
            off = noteOff(nv, 0)
            off.delta = beatTime + beatLen - 1000
            scheduleEvent(on)
            scheduleEvent(off)      
end



current_chord = {1,2}
next_paused = 0
stopped = 0

function nextChord()

    move_up = -1
    for i=1,#current_chord-1 do
        --can we move one up?
        if current_chord[i]+1<current_chord[i+1] then
            move_up = i        
            break
        end
    end
    
    if move_up>0 then
        current_chord[move_up] = current_chord[move_up]+1
        --move all lower notes to the bottom position
        for i=1,move_up-1 do
            current_chord[i] = i
        end
    
    else
    
        --have to move up top note...
        current_chord[#current_chord] = current_chord[#current_chord]+1
        
        --new note if the top exceeds 12
        if current_chord[#current_chord] > 12 then 
            table.insert(current_chord, #current_chord)
        end
        
        --move to original position
        for i=1,#current_chord-1 do
            current_chord[i] = i
        end
        
        --pause, if we increased by a note for the top
        next_paused = 1
        
        if #current_chord>13 then
            stopped = 1
        end
               
    end    

end
vprint("\n")


function onFrameCb(frameData)
    hosttime = getHostTime()    
   
    beatLen, beat, beatTime = computeBeatTime(hosttime.samplePos, hosttime.tempo, hosttime.sampleRate, 1)            
    
    
    
    if beatTime <= VSTFrameLength + 1 and stopped==0 then                        
        
        
        
        if next_paused==0 then 
            for i,v in ipairs(current_chord) do
                playNote(v+59, beatTime, beatLen)            
            end
            
            for i,v in ipairs(current_chord) do
                vprint(v.." ")
            end
            vprint("\n")
            
            nextChord()
        else
            next_paused = 0
            vprint("\n")
        end
                            
                             
    end       
end