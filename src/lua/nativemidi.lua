openMidiDevices = {}
deviceIdMappings = {}

function getNativeMidiName(port)
	return deviceIdMappings(port)
end

--Get a list of midi names (either "in" or "out")
function getMidiDevices(midiType)
	dev_table = _getMidiDeviceNames()
	i = dev_table[midiType]
	names = {}
	for i,v in ipairs(i) do
		
		name = v[1]
		
		table.insert(names, name)
	end
	return names

end

--return all available midi in devices
function getMidiInDevices()
	return getMidiDevices("in")
	
end 

--return all available midi out devices
function getMidiOutDevices()
	return getMidiDevices("out")
end 


-- return name to id mapping
function getMidiDeviceID()
	dev_table = _getMidiDeviceNames()
	
	ids = {}
	for i,v in ipairs(dev_table["in"]) do		
		ids[v[1]] = v[2]				
	end
	for i,v in ipairs(dev_table["out"]) do		
		ids[v[1]] = v[2]				
	end
	
	return ids
end 



-- open a midi device by name. Name must be one of those given by getMidiDevices.
function openMidiDevice(name)	
	if not openMidiDevices[name] then 		
		ids = getMidiDeviceID()		
		if ids[name]==nil then
			verror("Midi device "..name.." does not exist!\n")
			return false
		end
		
		result = _openMidiDevice(ids[name])
		if not result then
			verror("Midi device "..result.." could not be opened.\n")
			return false
		end
		
		openMidiDevices[name] = ids[name]	
		deviceIdMappings[ids[name]] = name
	else
		verror("Midi device "..name.." is already open!\n")
		return false
	end
	return true 
end

--  close an open midi device.
function closeMidiDevice(name)
	if openMidiDevices[name] then
		_closeMidiDevice(openMidiDevices[name])
		deviceIdMappings[openMidiDevice[name]] = nil
		openMidiDevices[name] = nil		
	else
		verror("Midi device "..name.." not open!\n")
	end
end

