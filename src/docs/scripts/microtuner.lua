
vprint("Microtuner. Load a scale from the gui panel (in Scala format).\n")
vprint("Notes will then be remapped with pitchbend to be tuned according to that scale.\n")
vprint("Notes will be spread across all 16 channels to avoid shifting notes already playing.\n")
vprint("This will only work on instruments which support separate channel pitch bend.\n")


scale = {}

channels_used = {}
key_map = {}


target_detune = 300 -- cents. Should match the detune level on the target synth


--compute midi note frequencies    
midibase = 8.1758 --hz
midiNotes = {}
for i=0,127 do
    midiNotes[i] = midibase*2^(i/12.0)        
    key_map[i] = {note=i, bend=8192}
end

--convert a ratio (as a string in the form x/y) to a cent value
function ratioToCents(str)
    slash = string.find(str, "/")
    first = string.sub(str, 1, slash-1)
    second = string.sub(str, slash+1)
    x = (math.log(first/second)/math.log(2)) * 1200 
    return x    
end

--find the detune that takes the given note to target
function findPitchBend(noteHz, targetHz, detune_level)
    ratio = targetHz/noteHz
    cents = (math.log(ratio)/math.log(2)) * 1200
    detune = (cents/detune_level) * 8192 + 8192

    --clip
    if detune>16383 then
        detune = 16383
    end
    if detune<0 then
        detune = 0
    end

    return detune
end

--compute the map for each key, given the current detune level and the current scale
function computeKeyboardMap()
    local n = scale.notes
    local ctr = 1    
    local base = 8.1758 --hz    
    local hzNotes = {}
    
    --start at lowest C as a reference (midi note 0)    
    for i=0,127 do    
        if ctr>n then
            ctr = 1
            base = hz
        end        
        hz = base * 2^(scale[ctr]/1200.0)        
        hzNotes[i] = hz
        ctr = ctr + 1                        
    end
    
    
    --search for the nearest matching note
    for i=0,127 do    
        target = hzNotes[i]
                
        minDiff = 1e12
        minIndex = 0
        
        --find nearest note (inefficiently!)
        for j=0,127 do
            midiHz = midiNotes[j]
            if math.abs(target-midiHz)<minDiff then
                minDiff =math.abs(target-midiHz)
                minIndex = j
            end
        end

        --set the note and bend in the map
        note = minIndex
        bend = findPitchBend(midiNotes[minIndex], target, target_detune)
        
        --store the map
        key_map[i] = {note=note, bend=bend}    
    end        
end



--parse a scala file, converting all ratios into cent values
function parseScala(filename)
    io.input(filename)    
    
    local line
    local state = 0
    scale = {}
    
    repeat
        line = io.read() -- first line is always description/comment               
        
        if line then
            if string.find(line, '!')  then
                --ignore comments
                
            else
                --first non comment is the description
                if state==0 then
                    scale.description =line
                end
                
                --second is the number of notes
                if state==1 then
                    scale.notes = line+0
                end
                                
                --the remainder are scale lines
                if state>1 then
                    --is it a cent value (MUST have a period, as per Scala specs)
                    if string.find(line, "%.") then                        
                        scale[state-1] = line+0
                    else
                        scale[state-1] = ratioToCents(line)
                    end                   
                end                
                state = state + 1
            end        
        end    
    until not line
    
    if scale.description then
      vprint(scale.description)
    end
    
    computeKeyboardMap()
end



function openCb()
    newcontrol = {x=45, y=5, type=GUITypes.kickButton,  label="Load"}
    loadButton = guiAddControl(newcontrol)    
    
end



function valueChangedCb(tag, value)

    --remember lua tables are 1-indexed not 0-indexed!
    if tag==loadButton and value==1 then
        fileToLoad = openFileSelector("scl")
        if fileToLoad then        
            parseScala(fileToLoad)
        end
    end    
end


function removeNote(note)    
    
    for i=1,16 do
        if channels_used[i]==note then
            channels_used[i] = nil
            return noteOff(note, i-1)
        end
    end
    
    return nil
end



function midiEventCb(event)    
    if event.type==midi.noteOn or event.type==midi.noteOff then            
        if event.type==midi.noteOn then
            

            --find a free note
            for v=1,16 do                
                if not channels_used[v] then
                    note = key_map[event.byte2].note
                    
                    channels_used[v] = note
                    event.channel = v-1             
                    
                    --send the bend
                    ev = pitchBend(key_map[event.byte2].bend, v-1)
                    ev.delta = event.delta
                    sendMidi(ev)
                    
                    --send the note
                    event.byte2 = note                    
                    sendMidi(event)                                        
                    
                    break
                end
            end
            
            
        else
            --remove the note
            r = removeNote(key_map[event.byte2].note)           
            --pass on the message
            if r then 
                sendMidi(r)                
            end            
        end        
     
    --otherwise passthrough
    else
        sendMidi(event)    
    end

end
