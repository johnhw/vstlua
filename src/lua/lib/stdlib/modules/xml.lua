-- String

module ("xml", package.seeall)

require "base"
require "rex"


-- @func string.writeXML: write a table as XML
-- The input format is assumed to be that output by luaexpat
--   @t: table to print
--   In each element, tag is its name, attr is the table of attributes,
--   and the sub-elements are held in the integer keys
--   @indent: indent between levels ["\t"]
--   @spacing: space before every line
-- @returns
--   @s: XML string
function string.writeXML (t, indent, spacing)
  indent = indent or "\t"
  spacing = spacing or ""
  return render (t,
                 function (x)
                   spacing = spacing .. indent
                   if x.tag then
                     local s = "<" .. x.tag
                     if type (x.attr) == "table" then
                       for i, v in pairs (x.attr) do
                         if type (i) ~= "number" then
                           -- luaexpat gives names of attributes in list elements
                           s = s .. " " .. tostring (i) .. "=" .. string.format ("%q", tostring (v))
                         end
                       end
                     end
                     if #x == 0 then
                       s = s .. " /"
                     end
                     s = s .. ">"
                     return s
                   end
                   return ""
                 end,
                 function (x)
                   spacing = string.gsub (spacing, indent .. "$", "")
                   if x.tag and #x > 0 then
                     return spacing .. "</" .. x.tag .. ">"
                   end
                   return ""
                 end,
                 function (s)
                   s = tostring (s)
                   s = string.gsub (s, "<", "&lt;")
                   s = string.gsub (s, ">", "&gt;")
                   s = rex.gsub (s, "&(?!#?\\w+;)", "&amp;")
                   return s
                 end,
                 function (x, i, v, is, vs)
                   local s = ""
                   if type (i) == "number" then
                     s = spacing .. vs
                   end
                   return s
                 end,
                 function (_, i, _, j)
                   if type (i) == "number" or type (j) == "number" then
                     return "\n"
                   end
                   return ""
                 end)
end
