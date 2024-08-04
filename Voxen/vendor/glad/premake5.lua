project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("Binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c",
    }

    includedirs
    {
        "include"
    }

    filter "system:linux"
        pic "On"
        systemversion "latest"

    filter "system:macosx"
        pic "On"
        systemversion "latest"

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
