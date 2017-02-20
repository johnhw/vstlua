help.title="Midi filter"
help.description="Shows how to use midi filters to simplify handling midi events"
help.author="tzec"

addMidiFilter{type={midi.noteOn,midi.noteOff}, byte2="40-60", callback = function(event) 
event.byte2= event.byte2-7
sendMidi(event) 
end}

addMidiFilter{type={midi.noteOn,midi.noteOff}, note="60-80", callback = function(event) 
event.note= event.note+7
sendMidi(event) 
end}

--wacky!
addMidiFilter{type={midi.noteOn,midi.noteOff},note={81,83,85,87,89,91,93}, callback = function(event)
event.note = event.note-4
sendMidi(event)
end}


--convert cc1 to pitchbend
addMidiFilter{type=midi.cc, controller=1, callback = function(event)
event.type = midi.pb
event.bend = event.velocity * 127
sendMidi(event)
end}
