
help.title="SysEx Test"
help.description="Tests sysex functionality, including hex string <-> binary conversion\nand host <-> plugin sysex communication"
help.author="tzec"

vprint(sysexToHex("hello").."\n")


vprint(sysexToHex(hexToSysex("f006f7")).."\n")
vprint(sysexToHex(hexToSysex("f0 06 f7")).."\n")
vprint(sysexToHex(hexToSysex("f0, 06, f7")).."\n")
vprint(sysexToHex(hexToSysex({"f0, 06", "f7"})).."\n")
vprint(sysexToHex(hexToSysex({"f0, 06", 0xf7})).."\n")
vprint(sysexToHex(hexToSysex({0xf0, "06", 0xf7})).."\n")
vprint(sysexToHex(hexToSysex({0xf0, {"06"}, 0xf7})).."\n")

print_table(sysexToTable(hexToSysex("f006f7")))




function midiEventCb(event) 

    if event.type==midi.sysex then
        vprint(sysexToHex(event.sysex))
    end
end


ctr = 0
function onFrameCb()
    ctr = ctr + 1
    
    if ctr==10 then 
        sendMidi(sysexMsg(hexToSysex("f00600f7")))
        ctr = 0
    end
end