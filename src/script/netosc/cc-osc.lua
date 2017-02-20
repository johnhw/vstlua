
-- send midi notes to a remote host running net-recv.lua


local socket = require("socket")
host = "127.0.0.1"
port = 7
host = socket.dns.toip(host)
udp = socket.udp()
udp:setpeername(host, port)
vprint("Using remote host '" ..host.. "' and port " .. port .. "...")

--send notes only
function midiEventCb(event)    
    if event.type==midi.cc then
        osc = toOSC({'/midi/cc', {event.byte2, event.byte3, event.channel}})
        udp:send(osc)
    end
end

