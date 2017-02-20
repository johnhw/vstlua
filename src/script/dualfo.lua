vprint("LFO modulating LFO. Modulates CC 74\n")


skip = 0
function onFrameCb(frameData)

    skip=skip+1
    
    
    
    --don't send every frame, keep the messages reasonable
    if skip==1 then
        hosttime = getHostTime()

        osc1 = hosttime.sampleRate * (240/hosttime.tempo)    
        
        t = hosttime.samplePos
        
        
        v1 = math.cos(t/osc1)
        osc2 = hosttime.sampleRate * (60/hosttime.tempo)  * (v1+2) / 5
        v2 = math.cos(t/osc2)
        
        ev = setCC(74, 64+63*v2, 0)

        
        sendMidi(ev)
        
        skip = 0
    end
    
end