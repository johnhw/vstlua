

--gui types
GUITypes = {onOffButton=0, kickButton=1, transientLed=2, stickLed=3, knob=4, hSlider=5, vSlider=6, label=7, menu=8, xypad=9, text=10, vuMeter=11}



--list of all controls
guiControlTable = {}

--controls to be added when window opened
controlsToAdd = {}


--mapping VST par no. -> list of controls that are linked to it
guiVSTMappings = {}


--add a control, specifying its center point

function guiAddControlCentered(control)
    control.x = control.x + GUISizes[control.type].width/2
    control.y = control.y + GUISizes[control.type].height/2
    return guiAddControl(control)
end



function registerGUIAdd(constructor)
	table.insert(controlsToAdd, constructor)
    
    --register vst linkage
    vpar = constructor.vstparameter
    if  vpar then
    
        --create the list if it doesn't exist
        if not guiVSTMappings[vpar] then
            guiVSTMappings[vpar] = {}
        end
        
        table.insert(guiVSTMappings[vpar], constructor)
        
    end
    
end


--do the scaling
function guiScaleValue(value, constructor)

    if constructor.min and constructor.max then
        return value*(constructor.max-constructor.min)+constructor.min
    else
        return value
    end

end

function addControl(constructor)

    --check this isn't a duplicate
    for i,v in pairs(guiControlTable) do
        if v.x == constructor.x and v.y == constructor.y and v.type==constructor.type then            
            return v.guiTableProxy
        end    
    end

            
    local guiTableProxy = {}
    constructor.guiTableProxy = guiTableProxy
    
    --register for creation on openCb()
	registerGUIAdd(constructor)	
    
	--do it now if it's already open
	if windowOpen==true then	
		doAddControl(constructor)
	end	
               	
	--add assignment capabilities to the "value" field of the control
	local mt = {__index = function(t,k) 
		if k=='value' then
			return guiGetScaledValue(t.control)
        else
            return rawget(t,k)
		end
        
	end,
	__newindex = function(t,k,v)
		if k=='value' then
			guiSetScaledValue(t.control)
        else
            rawset(t,k,v)
		end
	end}
	
	setmetatable(guiTableProxy, mt)
	
	return guiTableProxy

    
end


--return the value of a control, rescaling if necessary
function guiGetScaledValue(control)

    --this will be nil if openCb() has not been called yet.
    if not control then
        return 0
    end

	local constructor = guiControlTable[control]
	local value = guiGetValue(control)
	
	--scale the value	
    local realValue = guiScaleValue(value, constructor)
		
	return realValue
end


--set the value of control, rescaling if necessaryu
function guiSetScaledValue(control, value)

    --this will be nil if openCb() has not been called yet.
    if not control then
        return
    end

	local constructor = guiControlTable(control)

	
	--scale the value
	if constructor.min and constructor.max then
		local realValue = (value-constructor.min)/(constructor.max-constructor.min)
	else
		local realValue = value
	end
		
	--update vst paramters
	if constructor.vstparameter then
		setVSTParameter(constructor.vstparameter, realValue)
	end
	
	--update the control itself
	guiSetValue(control, realValue)

end
	

--argument table:
--x, y, type, label, [menu], as for guiAddControl
--[callback] -> callback to be called when value changes
--[min, max] -> minimum and maximum values. Get/set will be automatically mapped to this range
--[vstparameter] -> this control value will be linked to the given vst parameter
--[value], initial value. Will set the vst parameter as well, if vstparameter is given (be careful!)
	
function doAddControl(constructor)

	local guiConstructor = {x=constructor.x, y=constructor.y, type=constructor.type, menu=constructor.menu, label=constructor.label}
	local  control = guiAddControl(guiConstructor)
	
	guiControlTable[control] = constructor
	
	constructor.control = control
    
	if constructor.value then			
		guiSetScaledValue(control, value)
		--make sure vst parameter matches
		if constructor.vstparameter then
			setVSTParameter(constructor.vstparameter, value)
		end
		
	end
	
	--read value from vst parameter if necessary
	if constructor.vstparameter then
		guiSetValue(control, getVSTParameter(constructor.vstparameter))
	end
	
    
    --set the control in the proxy table
    constructor.guiTableProxy.control = control
	
	
end


