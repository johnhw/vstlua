
--create an ADSR envelope, which rises to emax after a samples, decays to s after d samples, and goes back to zero after r samples

ADSR = {}

function ADSR.trigger(env,value)
    --precompute the adjustments...
    env.recip_a = value/env.a
    env.recip_d = (value-env.s)/env.d
    env.max = value
    env.recip_r = (env.s/env.r)
    env.state = 1
    env.time = 0
    env.value = 0
    
end

function ADSR.release(env)
    env.state = 4
    env.time = 0
end

function ADSR.update(env, time)

        if env.state == 1 then
            if env.time<env.a then 
                env.value =  env.recip_a*env.time
                
            else
                env.time = 0
                env.state = 2
            end    
        end
        
        --d
        if env.state == 2 then
            if env.time<env.d then 
                env.value =  env.max - env.recip_d*(env.time)                
            else
                env.state = 0
            end    
        end
        
        --s
        
        --r
        if env.state == 4 then
            if env.time<env.r then 
                env.value = env.s - env.recip_r*(env.time)
            else
                env.state = 0
            end    
        end
                
                
        env.time = env.time + time
        
        return env.value
end


function ADSR:new(o)
    
    self.__index = self        
    local env = {
        a = o.a,
        d = o.d,
        s = o.s,
        r = o.r,    
        state = 0,
        time = 0,
        value = 0,
            
    }
    setmetatable(env, self)
    return env
end

