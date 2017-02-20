--osc test

local socket = require("socket")
host = "127.0.0.1"
port = 7000
host = socket.dns.toip(host)
udp = socket.udp()
udp:setpeername(host, port)
vprint("Using remote host '" ..host.. "' and port " .. port .. "...")

udp:send(toOSC({'/test1', {20, 20}}))
udp:close

