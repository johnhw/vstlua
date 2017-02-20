
--Construct the list of avaliable chords
chord_list = {}
for c,k in pairs(common_chords) do
    table.insert(chord_list, c)    
end

current_chord = chord_list[1]


function openCb()
    newcontrol = {x=15, y=55, type=GUITypes.menu,  label="Chord", menu = chord_list}
    menu = guiAddControl(newcontrol)
end



function valueChangedCb(tag, value)

    --remember lua tables are 1-indexed not 0-indexed!
    if tag==menu then
        current_chord = chord_list[value+1]         
    end    
end


function midiEventCb(event)

    
    --send the chorded note
    if event.type==midi.noteOn or event.type==midi.noteOff then
        clist = chords[current_chord]                        
        basenote = event.byte2        
        
        for i,c in pairs(clist) do            
             event.byte2 = basenote + c                          
             sendMidi(event)
        end        
        
    
    --otherwise passthrough
    else
        sendMidi(event)
    
    end

end
