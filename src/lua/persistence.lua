globalPersistence = {}

function loadGlobalPersistence()
    persistencePath = getVSTPath().."\\persistent_store.lua"    
    --read previous entries
    fileCall = loadfile(persistencePath)    
    if fileCall then
        globalPersistence = {}
        fileCall()
    end

    if not persistence then
        globalPersistence = {}
    end
            
end


function saveGlobalPersistence()
    
    local persistencePath = getVSTPath().."\\persistent_store.lua"    
    local file = io.open(persistencePath, "w")
    if file then
        serialized = serializeTable("globalPersistence", globalPersistence)        
        file:write(serialized)
        io.close(file)
    end
end



programData= {}

function loadPersistentProgramData()

    str = readProgramData(persistString)
    

    
    if str and string.len(str)>1 then
        assert(loadstring(str))()
    else
        programData = {}
    end
   
end


function savePersistentProgramData()

 local persistString = serializeTable("programData", programData)
    
    writeProgramData(persistString)
   
end