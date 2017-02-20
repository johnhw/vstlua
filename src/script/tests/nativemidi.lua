print("Midi input ports\n")
for i,v in ipairs(getMidiInDevices()) do
	print(v)
	print("\n")
end

print("\n\n")
print("Midi output ports\n")
for i,v in ipairs(getMidiOutDevices()) do
	print(v)
	print("\n")
end

openMidiDevice("padKONTROL 1 PORT A")


function resetCb()
	closeMidiDevice("padKONTROL 1 PORT A")
end

function midiEventCb(msg)
	print(msg)

end