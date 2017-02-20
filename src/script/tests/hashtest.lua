

function testFunc()
    
end

function otherTestFunc()
    
end

testThread = coroutine.create(testFunc)

vprint("hash of {1,2} = " .. hash({1,2}).."\n")
vprint("hash of 'hello' = " .. hash("hello").."\n")
vprint("hash of 24 = " .. hash(24).."\n")
vprint("hash of true = " .. hash(true).."\n")
vprint("hash of nil = " .. hash(nil).."\n")
vprint("hash of testThread = " .. hash(testThread).."\n")
vprint("hash of testFunc() = " .. hash(testFunc).."\n")
vprint("hash of otherTestFunc() = " .. hash(otherTestFunc).."\n")
vprint("hash of {1,'hello',false,{2,3},testFunc()} = " .. hash({1,"hello",false,{2,3},testFunc()}).."\n")
vprint("hash of {} = "..hash({}).."\n")
vprint("hash of {{}} = "..hash({{}}).."\n")
vprint("hash of {{},{}} = "..hash( {{},{}} ).."\n")
vprint("hash of {{{}},{}} = "..hash({{{}},{}}).."\n")
vprint("hash of {{{},{}}} = "..hash({{{},{}}}).."\n")

a = {1,2}
b = {1,2,a}
a = {1,2,b}
vprint(hash(a))