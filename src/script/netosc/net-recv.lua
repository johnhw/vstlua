--receive midi from an instance of vstlua running net-send.lua

local socket = require("socket")
host =  "127.0.0.1"
port =  7

vprint("Binding to host '" ..host.. "' and port " ..port.. "...")
udp = socket.udp()
udp:setsockname(host, port)
udp:settimeout(0)  -- don't block
ip, port = udp:getsockname()

vprint("Waiting packets on " .. ip .. ":" .. port .. "...")

function onFrameCb()
	dgram, ip, port = udp:receivefrom()
	if dgram then
        _,_,note,velocity = string.find(dgram, "(%d+) (%d+)")        
        vprint(note..":"..velocity.."\n")        
        event = noteOn(note, velocity,0)
        sendMidi(event)		
    end
end
