project "parser"
    location "."
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    targetdir ("binaries/bin/%{cfg.buildcfg}")
    objdir ("binaries/bin-int/%{cfg.buildcfg}")

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
