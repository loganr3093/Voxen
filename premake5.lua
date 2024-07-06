workspace "Voxen"
    architecture "x64"
    startproject "Sandbox"

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
IncludeDir["glad"] = "Voxen/vendor/glad/include"
IncludeDir["imgui"] = "Voxen/vendor/imgui"
IncludeDir["glm"] = "Voxen/vendor/glm"
IncludeDir["stb"] = "Voxen/vendor/stb"

group "Dependencies"
    include "Voxen/vendor/GLFW"
    include "Voxen/vendor/glad"
    include "Voxen/vendor/imgui"

group ""

project "Voxen"
    location "Voxen"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "voxpch.h"
    pchsource "Voxen/src/voxpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb/**.h",
        "%{prj.name}/vendor/stb/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/glm/glm/**.hpp",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.stb}"
    }

    links
    {
        "GLFW",
        "glad",
        "imgui",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "VOX_PLATFORM_WINDOWS",
            "VOX_BUILD_DLL",
            "GLFW_INCLUDE_NONE",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
        defines "VOX_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "VOX_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "VOX_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

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
        "Voxen/src",
        "Voxen/vendor",
        "%{IncludeDir.glm}"
    }

    links
    {
        "Voxen"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "VOX_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "VOX_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "VOX_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "VOX_DIST"
        runtime "Release"
        optimize "on"