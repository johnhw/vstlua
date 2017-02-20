vprint("Adds inertia to all CC's\n")


help.description="Adds inertia to all CC's.\nTry adjusting some cc's; the motion should be smoother and laggier."
help.title="CC Smooth"
help.author="tzec"



cctarget = {}
ccvals = {}

function midiEventCb(midiEvent) 

    if midiEvent.type==midi.cc then    
            --set the target
            cctarget[midiEvent.byte2+128*midiEvent.channel] = midiEvent.byte3
            
    else
    sendMidi(midiEvent)   
    end
end


function onFrameCb(hosttime)
    
	--compute filter based on frame length
    alpha = 0.9999 ^ VSTFrameLength
    
    
    for i,v in pairs(cctarget) do

        
        if not ccvals[i] then
			--initialise if never done so yet
            ccvals[i] = v
        else
			--otherwise move towards target
            ccvals[i] = ccvals[i] * alpha + (1-alpha) * cctarget[i]            
        end
        
        --send event (NB \ = integer divide)
        sendMidi(setCC(i % 128, ccvals[i], i \ 128))
        
        
        --remove acheived targets
        if math.abs(ccvals[i]-cctarget[i])<1 then 
            cctarget[i] = nil            
        end
        
    end

end