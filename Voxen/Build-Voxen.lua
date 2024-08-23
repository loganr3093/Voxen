-- Build Voxen

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
        "vendor/stb/**.h",
        "vendor/stb/**.cpp",
        "vendor/glm/glm/**.inl",
        "vendor/glm/glm/**.hpp",
    }

    pchheader "voxpch.h"
    pchsource "Source/voxpch.cpp"

    includedirs
    {
        "Source",
        "vendor/spdlog/include",
        "vendor/GLFW/include",
        "vendor/glad/include",
        "vendor/glm",
        "vendor/imgui",
        "vendor/stb",
        "vendor/entt",
        "vendor/yaml-cpp/include"
    }

    links
    {
        "GLFW",
        "glad",
        "imgui",
        "yaml-cpp",
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
            "_CRT_SECURE_NO_WARNINGS",
            "YAML_CPP_STATIC_DEFINE"
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