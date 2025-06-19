workspace "Parser"
    architecture "x64"
    configurations { "Debug", "Release" }

project "Parser"
    location "Parser"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir ("Parser/bin/%{cfg.buildcfg}")
    objdir ("Parser/bin-int/%{cfg.buildcfg}")

    files { "%{prj.name}/include/**.h", "%{prj.name}/include/**.inl", "%{prj.name}/**.cpp" }

    pchheader "headers.h"
    pchsource "Parser/src/headers.cpp"


    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"