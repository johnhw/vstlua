--receive midi from an instance of vstlua running net-send.lua

local socket = require("socket")
host =  "127.0.0.1"
port =  7000

--create the udp object
vprint("Binding to host '" ..host.. "' and port " ..port.. "...")
udp = socket.udp()
success, msg = udp:setsockname(host, port) 
if not success then
    vprint(msg)
end

ip, port = udp:getsockname()
networkReceiveLoop(udp)

function networkReceiveCb(dgram, addr, port)

    --parse the packet
	if dgram then
        msg = fromOSC(dgram,1)
        print_table(msg, vprint, "")
    end    
    return 1 -- this is required to keep this callback being called. returning zero or nil will end the receive loop
end

function resetCb()
    --clean up
    udp:close()    
end
