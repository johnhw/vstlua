
--create a coroutine that runs until max instructions, then yields until the next time
function createPartialCoroutine(func, instructions)

    local wrapFunc = function()
        debug.sethook(coroutine.yield, "", instructions)
        func()    
    end
    
    cor = coroutine.create(wrapFunc)    
    
    
    local exec = function()             
        coroutine.resume(cor)        
    end
    
    return exec
    
end


--create a coroutine that runs until a maximum of timing ms, then yields
function createTimedPartialCoroutine(func, timing, resolution)
    resolution=resolution or 500

    
    local lastTime 
    
    local timer = function()
        s = (getTimeUs()-lastTime)/1000
        if s>timing then
            coroutine.yield()
        end
    end
    
    
    local wrapFunc = function()
        debug.sethook(timer, "", resolution)
        func()    
    end
    
    local cor = coroutine.create(wrapFunc)    
    
    
    
    local exec = function()     
        f, n, c = debug.gethook()    
        lastTime = getTimeUs()
        debug.sethook(timer, "", resolution)
        coroutine.resume(cor)
        debug.sethook(f,n,c)
    end
    
    return exec
end

