-- Build
workspace "Voxen"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   --***
   startproject "New Project"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

-- Binary files location
OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
    include "Voxen/vendor/GLFW"
    include "Voxen/vendor/glad"
    include "Voxen/vendor/imgui"
group ""

-- Build for Voxen
include "Voxen/Build-Voxen.lua"

-- Build for new project
include "New Project/Build-App.lua"

-- Build for the Voxen Editor
include "Voxen-Editor/Build-Editor"