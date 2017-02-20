help.title="Simplified GUI access"
help.description="Uses the simplified GUI access methods."
help.author="tzec"

--callback functionality
g1 = addControl{x=50, y=5, type=GUITypes.kickButton, label="", callback=function (value) vprint("Kick "..value.."\n")  end}


--uses auto-scaling
g2 = addControl{x=100, y=5, type=GUITypes.knob, label="Scaled", min=1, max=100, callback=function (value) vprint("Scaled "..value.."\n") end}

--links to vst parameter 5
g3 = addControl{x=200, y=5, type=GUITypes.knob, label="VST", min=1, max=100, vstparameter=5, callback=function (value) vprint("VSTKNnob "..value.." "..getVSTParameter(5).."\n") end}

--tests meta table use
g3.value = 20

vprint(g3.value)

function openCb()

    --uses instant add (checks duplicate removal works)
    z = addControl{x=300, y=5, type=GUITypes.onOffButton}
    
    

end


function onFrameCb()
   -- vprint(g3.value)
end