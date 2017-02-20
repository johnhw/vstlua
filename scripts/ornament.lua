--"ornamentation"

ornaments = 
{
    octAttack = {{12,1000}, {0,0}},    
    bigOctAttack = {{24,1000},{12,1000}, {0,0}},    
    hugeOctAttack = {{36,1000},{24,1000},{12,1000}, {0,0}},    
    fifthAttack = {{7,1000}, {0,0}},    
    majorAttack = {{0,1000}, {4,1000}, {7,1000}, {0,2000}, {4,2000}, {7,2000},{0,0}},    
    minorAttack = {{0,1000}, {3,1000}, {7,1000}, {12,2000}, {15,2000}, {19,2000},{0,0}},  
}


ornament_list = {}

for i,v in pairs(ornaments) do
    table.insert(ornament_list, i)    
end

ornament = ornaments[ornament_list[1]]

function openCb()
   newcontrol = {x=20, y=50, type=GUITypes.menu,  label="Ornament", menu = ornament_list}
    menu = guiAddControl(newcontrol)
end



function midiEventCb(midiEvent)
    if midiEvent.type==midi.noteOn then
        t = 0
        
        
        for i, orn in pairs(ornament) do        
            v = orn[1]
            td = orn[2]
            c = copyMidiEvent(midiEvent)
            c.byte2 = c.byte2+v
            c.delta = c.delta+t
            scheduleEvent(c)
            t = t + td
            
            if td~=0 then 
                c = copyMidiEvent(c)
                c.delta = midiEvent.delta+t
                c.type = midi.noteOff
                scheduleEvent(c)
            end
        end
        
        
        
    else
        sendMidi(midiEvent)
    
    end
    
        
end


function valueChangedCb(tag, value) 
    if tag==menu then
        
        ornament = ornaments[ornament_list[value+1]]
    end
end

--restore values from preset if needed
function initCb()  
end


