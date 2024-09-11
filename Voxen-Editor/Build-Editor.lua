-- Build Editor
project "Voxen-Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "on"

    files
    {
        "Source/**.h",
        "Source/**.cpp"
    }

    includedirs
    {
        "Source",
        "../Voxen/vendor/spdlog/include",
        "../Voxen/Source",
        "../Voxen/vendor",
        "../Voxen/vendor/glm",
        "../Voxen/vendor/filewatch",
        "../Voxen/vendor/entt",
        "../Voxen/vendor/ImGuizmo"
    }

    links
    {
        "Voxen"
    }

    targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"

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