debugMode = false

--trace all activity if debug is on
if debugMode then
    function printLine (event,line)
          local s = debug.getinfo(2).short_src
          vprint(s .. ":" ..line.."\n")
    end
    debug.sethook(printLine, "l")
end

require "partial"
require "executetrap"
require "utils"
require "keys"
require "midi"
require "scheduler"
require "adsr"
require "gui"
require "intervals"
require "chords"
require "scales"
require "sysex"
require "persistence"
require "callback_wrappers"
require "timing"
require "console"
require "shared"
require "nativemidi"

 