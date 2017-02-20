


--instructions that will be executed before aborting
_abortLimit = 10000000

--instructions that will executed before aborting inside onFrameCb()
_frameAbortLimit = 100000

executeTrap = {}

function infiniteError()
    error("Too many consecutive instructions (infinite loop?)\n")
end

function setMaxExecute(exmax)
    if not debugMode then
        debug.sethook(infiniteError, "", exmax)
    end
end
    

local mt = {
__index = function(t,k)
        if k=='limit' then return _abortLimit end
        if k=='frameLimit' then return _abortFrameLimit end
    end,
    
    __newindex = function(t,k,v)
        if k=='limit' then _abortLimit=v
        setMaxExecute(v)
        end
        if k=='frameLimit' then 
        _abortFrameLimit=v
        end       
    end
    
    }



setmetatable(executeTrap, mt)
