

 
eventQueue = {}


function eventCompare(a, b)
    return b.delta>a.delta         
end 
 
function scheduleEvent(event)
    local q
    q = copyMidiEvent(event)
    table.insert(eventQueue, q)        
end

function doSchedule(window)

    --ensure the table is in order
    table.sort(eventQueue, eventCompare)   

    --send all events in this window
    local  continue = 0
    repeat        
        continue = 0
        if eventQueue[1] and eventQueue[1].delta<window then
            sendMidi(eventQueue[1])
            table.remove(eventQueue, 1)  
            continue = 1
        end              
    until continue==0    
    
    --decrement the rest
    for i,n in ipairs(eventQueue) do
        eventQueue[i].delta = eventQueue[i].delta - window                
    end
        
end
