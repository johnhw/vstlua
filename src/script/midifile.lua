


function readVarLen(chunk)
    local i = 0
    local c = struct.unpack("B", chunk[i])
    local val = c
    if (val & 0x80)==1 then
        val = val & 0x7f
        repeat
            c = struct.unpack("B", chunk[i])
            val = (val << 7) + (c & 0x7f)        
        until not (c&80)==0
    end
    return val
end

function parseHeader(chunk)
    local format, numTracks, division = struct.unpack('HHH', chunk)

end

function parseTrack(chunk)
    

end

function readMidiChunk(bindata, fileptr)    
    --parse the header (char[4] name; int len)
    local remainder = string.sub(bindata, fileptr)
    
    
    local name, len = struct.unpack(">c4L", remainder)
    
    print(name)
    local block = struct.unpack("c"..len, string.sub(remainder,8))
    
    
    fileptr = fileptr + len + 9       
    
    if name=='MThd' then
        header = parseHeader(block)
        return fileptr, {name, header}
    end
        
    if name=='MTrk' then
        track = parseTrack(block)
        return fileptr, {name, track}            
    end
    
    return fileptr, nil
end

function parseMidiBinary(bindata)
    local fileptr = 0
    local len = string.len(bindata)
    while fileptr<len do
        
        fileptr, chunk = readMidiChunk(bindata, fileptr)            
    end    
end

function parseMidi(fname)
    local f = assert(io.open(fname, "rb"))
    local bindata = f:read("*all")
    
    parseMidiBinary(bindata)
    
    f:close()
    
end


parseMidi("scripts/canyon.mid")