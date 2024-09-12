workspace "$PROJECT_NAME$"
	targetdir "build"
	startproject "$PROJECT_NAME$"
	configurations { "Debug", "Release", "Dist" }

group "Voxen"
	include "$SCRIPT_CORE_LUA_RELATIVE_DIRECTORY$"
group ""

project "$PROJECT_NAME$"
	location "Assets/Scripts"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetname "$PROJECT_NAME$"
	targetdir "Assets/Scripts/Binaries"
	objdir "Assets/Scripts/Intermediates"

	files
    {
        "Assets/Scripts/Source/**.cs",
        "Assets/Scripts/Properties/**.cs",
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
