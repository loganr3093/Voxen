workspace "Voxen"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}=%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Voxen/vendor/GLFW/include"

include "Voxen/vendor/GLFW"

project "Voxen"
    location "Voxen"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "voxpch.h"
    pchsource "Voxen/src/voxpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}"
    }

    links
    {
        "GLFW",
        "opengl32.lib"
    }

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "VOX_PLATFORM_WINDOWS",
            "VOX_BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPYDIR} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }

    filter "configurations:Debug"
        defines "VOX_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "VOX_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "VOX_DIST"
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Voxen/vendor/spdlog/include",
        "Voxen/src"
    }

    links
    {
        "Voxen"
    }

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "VOX_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "VOX_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "VOX_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "VOX_DIST"
        optimize "On"