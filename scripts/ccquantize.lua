
vprint("Quantizes CC's\n")


help.description="Quantizes all CC's to intervals of 32.\n"
help.title="CC Quantizer"
help.author="tzec"



function midiEventCb(midiEvent) 
    if midiEvent.type==midi.cc then            
            midiEvent.byte3 = (midiEvent.byte3\32) * 32            
    end        
   sendMidi(midiEvent)   
end
