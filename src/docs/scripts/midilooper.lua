
vprint("Midi looper. Controlled by foot pedal (CC 64)\n")


help.description="Midi looper. Depress sustain footpedal (CC64) to record. Release at end, and sequence will loop.\nLoop lengths can be quantized (host must be playing)\n"
help.title="Midi Looper"
help.author="tzec"

--initiialise variables
recording = false --we're not recording
loop = {} --this will store the MIDI data
baseTime = 0
curTime = 0

pstates = {starting=1, playing=2, nothing=4}

--don't quantize the loop to bar starts
quantize = false

--start in the nothing event
play_state = pstates.nothing


function startRecording(event)
    vprint("Recording\n")
    loop = {} --clear the loop
    recording = true
    baseTime = event.delta -- store the exact offset
    curTime = 0
end


function stopRecording(event)
    vprint("Stopped\n")
    recording = false  
    curTime = 0
    --start playback
    play_state = pstates.starting
end

function loopAdvance(samples, hosttime)
    
    --starting, wait for the next bar...
    if play_state == pstates.starting then
    
        --jump immediately to playing if not quantizing
        if quantize==false then
            play_state =pstates.playing
            
        else
            --otherwise compute note start time
            len,b,timetobeat = computeBeatTime(hosttime.samplePos,  hosttime.tempo, hosttime.sampleRate, 4)
            if timetobeat<samples then
                play_state = pstates.playing
                curTime = -timetobeat --adjust so that time is dead on
            end
        end
    end
    
    
    if play_state == pstates.playing then    
        --playback time!    
        if recording==false then                 
            maxtime = 0       
            mintime = 0
   
            --if we're quantizing, shift the first note to start exactly on the bar
            if quantize==true then
                if #loop>0 then
                    mintime = loop[1][1] --very first note offset
                end    
            end
            
            for i,v in ipairs(loop) do
            
                t = v[1]-mintime
                m = v[2]
                
                --play correct notes
                if t>=curTime and t<curTime+samples then
                    c = copyMidiEvent(m)                
                    c.delta = t-curTime       
                    
                    sendMidi(c)
                end
                
                if t>maxtime then
                    maxtime = t
                end
                
            end
            
            --loop back to zero
            if curTime>maxtime then
                play_state = pstates.starting
                curTime = maxtime-curTime
            end
            
        end
    end
                
    
        curTime = curTime + samples    

end

--insert a new event into the buffer
function addToLoop(event)
    table.insert(loop, {curTime+event.delta, event})
end


function midiEventCb(event)

    
    if event.type==midi.cc and event.byte2==midicc.sustain then
        --toggle recording when sustain goes high
        if event.byte3>64 then
          startRecording(event)
        else
          stopRecording(event)
        end
    else
        --record, if we need to
        if recording==true then
           addToLoop(event)
           sendMidi(event) -- and echo through...
        end
    end
end
   

function resetCb()
panic()
end
    
    
function onFrameCb()       
  loopAdvance(VSTFrameLength, curHostTime)            
end