
function _expandConsole(str)
    
    local split = "[%,%( %=%+%-%\%/%<%>%&%|\"%'%%%[%]]"
    local s,e
    s,e = string.find(str, split)
    local first = ""
        
    if e then
        first = string.sub(str,1,e)
        str = string.sub(str, e+1) 
        
    end
        
       
    str = "^"..str
    
    
    
        for i,v in pairs(_G) do       
            if string.find(i,str) then
                match = first..i                                
                return match
            end    
        end       

end

function _parseConsole(str)
    s = assert(loadstring(str))
    print(s())
end