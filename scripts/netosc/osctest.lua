--osc test

--messages
msg = toOSC({'/test/message', {1, "hello", false, 2.1, '3'}})
t = fromOSC(msg)
print_table(t, vprint, "")
vprint("\n")


--bundles
msg = toOSC({0, {'/test/message', {1, "hello", false, 2, '3'}}, {'/test/message2', {"jack", '1w'}}})
t = fromOSC(msg)


print_table(t, vprint, "")

vprint("\n")

--explicit types
msg = toOSC({'/test/message', {{'int64',1}, {'string',"hello"}, {'boolean',false}, {'symbol', 'three'}, {'midi', 45}}})
t = fromOSC(msg, 1)
print_table(t, vprint, "")

--nested bundle
msg = toOSC({0, {400, {'/test/message', {1, "hello", false, 2, '3'}}}})
t = fromOSC(msg)
print_table(t, vprint, "")
vprint("\n")

