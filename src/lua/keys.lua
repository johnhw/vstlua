

dragType = {file=0, string=1}

virtualKeys = {back=1, tab=2, clear=3, kreturn=4, pause=5, escape=6, space=7, knext=8,
kend=9, home=10, left=11, up=12, right=13, down=14, pageup=15, pagedown=16, select=17,
kprint=18, enter=19, snapshot=20, insert=21, delete=22, help=23, num0=24,
num1=25, num2=26, num3=27, num4=28, num5=29, num6=30, num7=31, num8=32,
num9=33, asterisk=34, plus=35, separator=36, minus=37, decimal=38, divide=39, f1=40,
f2=41, f3=42, f4=43, f5=44, f6=45, f7=46, f8=47, f9=48, f10=49, f11=50, f12=51, numlock=52,
scroll=53, shift=54, control=55, alt=56, equals=57}

modifiers = {shift=1, alternate=2, command=4, control=8}

modifierNames = reverseMapping(modifiers)
virtualNames = reverseMapping(virtualKeys)




function keyName(key)
    
    if key.down==1 then
        down = "down"
    else
        down = "up"
    end
    
    
    keymod = modifierNames[key.modifier] or ""
    
    keyvirtual = virtualNames[key.virtual] or ""
    
    if keyvirtual=="" then
        keycharacter = string.char(key.character) or "??"
    else
        keycharacter = keyvirtual
    end
    
    return "["..keycharacter.."]".. " ("..keymod..")".."  "..down
            
end