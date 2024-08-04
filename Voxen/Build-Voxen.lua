-- Build Voxen
-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"]  = "Vendor/GLFW/include"
IncludeDir["glad"]  = "Vendor/glad/include"
IncludeDir["imgui"] = "Vendor/imgui"
IncludeDir["glm"]   = "Vendor/glm"
IncludeDir["stb"]   = "Vendor/stb"
IncludeDir["entt"]  = "Vendor/entt"

project "Voxen"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "on"

    files
    {
        "Source/**.h",
        "Source/**.cpp",
        "Vendor/stb/**.h",
        "Vendor/stb/**.cpp",
        "Vendor/glm/glm/**.inl",
        "Vendor/glm/glm/**.hpp",
    }

    pchheader "voxpch.h"
    pchsource "Source/voxpch.cpp"

    includedirs
    {
        "Source",
        "Vendor/spdlog/include",
        "Vendor/GLFW/include",
        "Vendor/glad/include",
        "Vendor/glm",
        "Vendor/imgui",
        "Vendor/stb",
        "Vendor/entt"
    }

    links
    {
        "GLFW",
        "glad",
        "imgui",
        "opengl32.lib"
    }

    targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "VOX_BUILD_DLL",
            "GLFW_INCLUDE_NONE",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
        defines "VOX_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "VOX_RELEASE"
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines "VOX_DIST"
        runtime "Release"
        optimize "On"
        symbols "Off"