--receive midi from an instance of vstlua running net-send.lua

local socket = require("socket")
host =  "127.0.0.1"
port =  7

--create the udp object
vprint("Binding to host '" ..host.. "' and port " ..port.. "...")
udp = socket.udp()
success, msg = udp:setsockname(host, port) 
if not success then
    vprint(msg)
end



ip, port = udp:getsockname()
vprint("Waiting packets on " .. ip .. ":" .. port .. "...")
vprint("Using networkReceiveLoop");
networkReceiveLoop(udp)

function networkReceiveCb(dgram, addr, port)

    --parse the packet
	if dgram then
        _,_,note,velocity = string.find(dgram, "(%d+) (%d+)")        
        vprint(note..":"..velocity.."\n")        
        event = noteOn(note, velocity,0)
        sendMidi(event)		
    end    
    return 1 -- this is required to keep this callback being called. returning zero or nil will end the receive loop
end

function resetCb()
    --clean up
    udp:close()    
end
