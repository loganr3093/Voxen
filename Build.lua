-- Build
workspace "Voxen"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   --***
   startproject "Voxen-Editor"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

-- Binary files location
OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
    include "Voxen/vendor/GLFW"
    include "Voxen/vendor/glad"
    include "Voxen/vendor/imgui"
    include "Voxen/vendor/yaml-cpp"
group ""

-- Build for Voxen
group "Core"
    include "Voxen/Build-Voxen.lua"
    include "Voxen-ScriptCore/Build-ScriptCore"
group ""

-- Build for the Voxen Editor
group "Tools"
    include "Voxen-Editor/Build-Editor"
group ""

-- Build for new project
group "Misc"
    include "New Project/Build-App.lua"
group ""