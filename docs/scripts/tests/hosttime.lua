

vprint("Tests setHostTime() functionality")


curhosttime = { tempo = 120 }


function openCb()
    
      
   tempoctrl = guiAddControl({x=120, y=110, type=GUITypes.knob,  label=""})   
   guiSetValue(tempoctrl, curhosttime.tempo/300.0)
   guiSetLabel(tempoctrl, "Tempo = "..curhosttime.tempo)
   
   time44ctrl = guiAddControl({x=180, y=110, type=GUITypes.kickButton,  label="4/4"})   
   time34ctrl = guiAddControl({x=220, y=110, type=GUITypes.kickButton,  label="3/4"})   
   
   startctrl = guiAddControl({x=260, y=110, type=GUITypes.kickButton,  label="Jump!"})   
   
   shortloop = guiAddControl({x=320, y=110, type=GUITypes.kickButton,  label="Short loop"})   
   longLoop = guiAddControl({x=380, y=110, type=GUITypes.kickButton,  label="Long loop"})   
   
   
end


function valueChangedCb(tag, value, text)
    
    if tag==tempoctrl then
        setHostTime({tempo=value*300})
        guiSetLabel(tempoctrl, "Tempo = "..math.floor(value*300))
    end
    
    if tag==time44ctrl then
        setHostTime({timeSigNumerator=4, timeSigDenominator=4})
    end
    
    if tag==time34ctrl then
        setHostTime({timeSigNumerator=3, timeSigDenominator=4})
    end
    
    if tag==startctrl then
        setHostTime({ppqPos=0})
    end

    if tag==shortloop then
        setHostTime({cycleStartPos=0, cycleEndPos=4})
    end
    
    if tag==longloop then
        setHostTime({cycleStartPos=0, cycleEndPos=64})
    end



end

function onFrameCb(hosttime)
    curhosttime = hosttime
end