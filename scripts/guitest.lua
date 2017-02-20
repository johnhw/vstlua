
function keyCb(key)
    vprint(keyName(key).." pressed!\n")
    return true
end

function dropCb(dropped)
    print_table(dropped, vprint, "")
end



function openCb()
    
    newcontrol = {x=50, y=125, type=GUITypes.onOffButton,  label="Switch me!"}
    onoff = guiAddControl(newcontrol)

    newcontrol = {x=150, y=125, type=GUITypes.kickButton,  label="Kick me!"}
    kick = guiAddControl(newcontrol)
    
    newcontrol = {x=550, y=125, type=GUITypes.vuMeter,  label="Level"}
    vu = guiAddControl(newcontrol)


    
    newcontrol = {x=150, y=175, type=GUITypes.kickButton,  label="Open file selector"}
    fileopen = guiAddControl(newcontrol)
    
    newcontrol = {x=290, y=175, type=GUITypes.onOffButton,  label="Resize!"}
    resize = guiAddControl(newcontrol)


    newcontrol = {x=250, y=125, type=GUITypes.transientLed,  label="Flicker!"}
    led1 = guiAddControl(newcontrol)

    newcontrol = {x=350, y=125, type=GUITypes.stickLed,  label="Stick!"}
    led2 = guiAddControl(newcontrol)

    newcontrol = {x=450, y=75, type=GUITypes.knob,  label="Twist!"}
    knob = guiAddControl(newcontrol)

    newcontrol = {x=5, y=200, type=GUITypes.hSlider,  label="Slide!"}
    hslider = guiAddControl(newcontrol)

    newcontrol = {x=200, y=200, type=GUITypes.vSlider,  label="Slide up!"}
    vslider = guiAddControl(newcontrol)

    newcontrol = {x=450, y=200, type=GUITypes.label,  label="Here"}
    label = guiAddControl(newcontrol)
    print(label)

    newcontrol = {x=5, y=300, type=GUITypes.menu,  label="MenuTest", menu = {"One", {"Two", {"Two.1", "Two.2"}}, "Three", "Four"}}
    menu = guiAddControl(newcontrol)
    
    print(menu)
    
    newcontrol = {x=505, y=300, type=GUITypes.text,  label="Text box"}
    textbox = guiAddControl(newcontrol)
    
    newcontrol = {x=305, y=300, type=GUITypes.xypad,  label="XY Pad"}
    xypad = guiAddControl(newcontrol)
    
   



end

function printName(name)
    vprint("You chose (multithreaded!) "..name)
end

function valueChangedCb(tag, value,str)

    vprint(tag.."  =  "..value.."\n")
    
    if tag==menu then
        vprint("Menu option:"..str)
    end
    
    if tag==textbox then
        vprint("Entered:"..str)
    end
    
    if tag==resize then
        if  value==1 then
            guiSetSize({width=400, height=400})            
        else
            guiSetSize({width=0, height=0})            
        end
    end

    if tag==fileopen and value==1 then
        file = openFileSelector("*.*")
        if file then
            vprint("You chose "..file.."\n")
        else
            vprint("You cancelled!\n")
        end
    end
    
    if tag==kick then
        guiSetValue(led1, value)        
    end
    
    
    if tag==xypad and str then
        
        vprint("mouse at "..str)
    
    end
    
    if tag==onoff then
        guiSetValue(led2, value)        
    end
        
    if tag==knob then
        guiSetLabel(label, ""..guiGetValue(knob), 0)        
        guiSetValue(vu, value)
    end

end
