
function midiEventCb(midiEvent)  
	vprint(midiEventToString(midiEvent)) 
   sendMidi(midiEvent)
end


