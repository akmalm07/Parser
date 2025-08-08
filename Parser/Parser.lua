project "Parser"
    location "."
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir ("Parser/bin/%{cfg.buildcfg}")
    objdir ("Parser/bin-int/%{cfg.buildcfg}")

    files { "headers.h", "include/**.h", "include/**.inl", "**.cpp" }

    includedirs "."

    pchheader "headers.h"
    pchsource "src/headers.cpp"


    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
