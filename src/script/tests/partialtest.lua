

vprint("Tests partial execution functionality")


   function do_something()
        for i=1,1000000 do
            vprint(i.." ")
        end
    end
    
    z = createPartialCoroutine(do_something, 1000)
    
    z() --executes 1000 instructions worth of func
    print("And the next lot...")
    z() --executes another 1000 instructions worth of func