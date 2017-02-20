vprint("Tempo-synced lfo. Host must be running. Sends changes on CC 74\n")


skip=0
function onFrameCb(frameData)

    skip=skip+1
    
    --don't send every frame, keep the messages reasonable
    if skip==4 then
        hosttime = getHostTime()
        osc = hosttime.sampleRate * (7/hosttime.tempo)    
        t = hosttime.samplePos
        v = math.cos(t/osc)
        ev = setCC(74, 64+63*v, 0)
        sendMidi(ev)
        skip = 0
    end
    
end