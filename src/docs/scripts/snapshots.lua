vprint("Hold a key down, then set a bunch of CC's.\n")
vprint("Pressing the key again will restore those CC's.\n")
vprint("The restore has inertia, so the cc's will morph.\n")


presets = {}
cur_preset = {}
last_note = -1
cc_table = {}
cc_targets = {}

inertia = 0.9





function midiEventCb(midiEvent) 

    if midiEvent.type==midi.noteOn then
    
        --send the value if it was already stored
        if presets[midiEvent.byte2] then
            cc_targets = {}
            
            for i,v in pairs(presets[midiEvent.byte2]) do
                cc_targets[i] = v
            end
            cur_preset = presets[midiEvent.byte2]
        end
        last_note = midiEvent.byte2                        
    end
 
    
    if midiEvent.type==midi.cc then
        if last_note>=0 then
            cur_preset[midiEvent.byte2] = midiEvent.byte3
        end  

        cc_table[midiEvent.byte2] = midiEvent.byte3
    end
 
    --store the preset
    if midiEvent.type==midi.noteOff then 
    
            if last_note==midiEvent.byte2 then
                presets[last_note] = cur_preset
                cur_preset = {}
            end            
            
            last_note = -1
    end
 
    
   sendMidi(midiEvent)
   
end

--morph to the new setting
function onFrameCb()
    for i,v in pairs(cc_targets) do
    
        if math.abs(cc_table[i]-cc_targets[i])>3.0 then
            v = inertia * cc_table[i] + (1-inertia) * cc_targets[i]
            cc_table[i] = v
            ev = setCC(i,v,0)
            sendMidi(ev)
        else
            cc_targets[i] = nil --clear when reached target
        end
    end
 end
 