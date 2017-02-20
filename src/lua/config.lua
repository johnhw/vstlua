
--The canDo's for this VST
canDo={"receiveVstMidiEvent", "receiveVstEvents", "receiveVstTimeInfo", "sendVstEvents", "sendVstMidiEvent", "sendVstTimeInfo", "receiveVstMidiEvents", "sendVstMidiEvents", "sendVstEvent", "receiveVstEvent"}


-------If you're using Sonar, the below must be set to
--numInputs=0
--numOutputs=0
--isSynth=1


--audio ins and outs
numInputs = 0
numOutputs = 0
isSynth = 1


-- does it support processReplacing (if this is zero, onFrameCb will never be called!)
processReplacing = 1


--midi channels
midiInChannels = 16 
midiOutChannels = 16


--console configuration
console_width = 600
console_height = 700
console_font_path = "consolefont\\ConsoleFont.bmp"

