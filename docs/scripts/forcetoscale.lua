vprint("Force to scale. Choose scales in the GUI panel\n")
scale_list = {}
note_list = {}

--get the lists of modes and notes
for k,v in pairs(modes) do
    table.insert(scale_list, k)
end

for k,v in pairs(plainnotenames) do
    table.insert(note_list, k)
end


--initialise
curscale = modes[scale_list[1]]
offset = plainnotenames[note_list[1]]

--create the gui
function openCb()  
   newcontrol = {x=20, y=50, type=GUITypes.menu,  label="Key", menu = note_list}
   menu = guiAddControl(newcontrol)        
   newcontrol = {x=20, y=90, type=GUITypes.menu,  label="Scale Types", menu = scale_list}
   scalemenu = guiAddControl(newcontrol)
end


function midiEventCb(midiEvent)   
    --repitch incoming notes
    if midiEvent.type==midi.noteOn or midiEvent.type==midi.noteOff then
        note = midiEvent.byte2
        
        --de-octave
        note = note % 12
        octave = midiEvent.byte2 - note
        
        minNote = 100
        bestNote = note
        
        --find best matching note
        for i,v in ipairs(curscale) do
            n = (v + offset) % 12            
            if math.abs(n-note)<minNote then
                minNote = math.abs(n-note)
               bestNote = n
            end
        end        
        
        --send back new note with right octave
        midiEvent.byte2 = bestNote + octave
        sendMidi(midiEvent)          
    end                               
end

--respond to gui events
function valueChangedCb(tag, value, text) 
    if tag==menu then        
        offset = plainnotenames[note_list[value+1]]
    end
    
    if tag==scalemenu then
        curscale = modes[scale_list[value+1]]                
    end
end
