--simple event handler code. Shows how OSC handlers can easily be made.

oscHandler = {}

function addOSCHandler(message, callback)
    oscHandler[message] = callback
end


local socket = require("socket")

--create the udp object
udp = socket.udp()
success, msg = udp:setsockname("127.0.0.1", 7000) 
if not success then
    vprint(msg)
else
    ip, port = udp:getsockname()    
    addOSCHandler("/midi/cc", respondToMessage)
    networkReceiveLoop(udp)
end

function networkReceiveCb(dgram, addr, port)
	if dgram then
        msg = fromOSC(dgram)        
        for name,callback in ipairs(oscHandler) do
            if type(msg[1])=='string' and msg[1]==name then
                callback(msg[1], msg[2])
            end
        end        
    end    
    return 1 -- this is required to keep this callback being called. returning zero or nil will end the receive loop
end


-- the callback we'll register
function respondToMessage(name, parameters)    
    cc = setCC(parameters[1], parameters[2], parameters[3])
    sendEvent(cc)
end


function resetCb()
    --clean up
    udp:close()    
end
