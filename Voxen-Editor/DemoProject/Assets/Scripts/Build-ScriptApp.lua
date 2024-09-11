-- Build Voxen-ScriptCore

-- Workspace
workspace "Demo"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   --***
   startproject "Demo"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

-- Project
project "Demo"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"

    targetdir ("Binaries")
    objdir ("Intermediates")

    files
    {
        "Source/**.cs",
        "Properties/**.cs",
    }

    links
    {
        "Voxen-ScriptCore"
    }

    filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"

group "Voxen"
    include "../../../../Voxen-ScriptCore/Build-ScriptCore.lua"
group ""