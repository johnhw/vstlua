--redefine print
function print(...)

    for i,v in ipairs(arg) do
        if v==nil then
            vprint("nil ")
        elseif type(v)=='table' then
            print_table(v)
        elseif type(v)=='number' or type(v)=='string' then
            vprint(v.." ")
        else 
            vprint(type(v)..' ')
        end
    end
    --vprint("\n")
    
end




function print_table(t, printer, indent)
    printer = printer or vprint
    indent = indent or ""
    for i, n in pairs(t) do
    
        if type(i)!='string' and type(i)!='number' then
            i = type(i)
        end
    
        if type(n)=='table' then
            printer(indent..i..": table; ")
            print_table(n, printer, indent.."  ")        
        else        
        
            if type(n)!='boolean' and type(n)!='number' and type(n)!='string' then
                n = type(n) -- just print the tupe
            end
        
            if type(n)=='boolean' then
                if n then
                    n = 'true'
                else
                    n = 'false'
                end
            end
            
            
            
            printer(indent..i..":"..n.."  ")
        end              
    end
end


--return reverse a mapping
function reverseMapping(t)
    nt = {}
    for key, value in pairs(t) do
        nt[value] = key
    end
    return nt
end


--from the lua manual
function basicSerialize (o)
      if type(o) == "number" then
        return tostring(o)
      else   -- assume it is a string
        return string.format("%q", o)
      end
end

 function serializeTable (name, value, saved)
      saved = saved or {}       -- initial value
      local string_table = {}
      
      table.insert(string_table, name.." = ")
      if type(value) == "number" or type(value) == "string" then
        table.insert(string_table,basicSerialize(value).."\n")
        
      elseif type(value) == "table" then
        if saved[value] then    -- value already saved?
          table.insert(string_table,saved[value].."\n")          
        else
          saved[value] = name   -- save name for next time
          table.insert(string_table, "{}\n")          
          for k,v in pairs(value) do      -- save its fields
            
            local fieldname = string.format("%s[%s]", name,
                                            basicSerialize(k))
            table.insert(string_table, serializeTable(fieldname, v, saved))
          end
        end
      else
        error("cannot save a " .. type(value))
      end
      
      return table.concat(string_table)
    end