workspace("parser")
    architecture("x64")
    configurations({ "Debug", "Release" })

group "parser"
    include "parser/parser.lua"