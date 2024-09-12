#include "voxpch.h"
#include "Voxen/Scripting/ScriptBuilder.h"

#include <windows.h>
#include <ShlObj.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <format>

namespace Voxen
{

	void ScriptBuilder::BuildCSProject(const std::filesystem::path& filepath)
	{
        // Get the path to Program Files
        TCHAR programFilesFilePath[MAX_PATH];
        if (!SHGetSpecialFolderPath(0, programFilesFilePath, CSIDL_PROGRAM_FILES, FALSE)) {
            std::cerr << "Failed to retrieve Program Files folder." << std::endl;
            return;
        }

        // Define possible MSBuild paths for different Visual Studio editions
        std::vector<std::filesystem::path> msBuildPaths = {
            std::filesystem::path(programFilesFilePath) / "Microsoft Visual Studio" / "2022" / "Community" / "Msbuild" / "Current" / "Bin" / "MSBuild.exe",
            std::filesystem::path(programFilesFilePath) / "Microsoft Visual Studio" / "2022" / "Professional" / "Msbuild" / "Current" / "Bin" / "MSBuild.exe",
            std::filesystem::path(programFilesFilePath) / "Microsoft Visual Studio" / "2022" / "Enterprise" / "Msbuild" / "Current" / "Bin" / "MSBuild.exe"
        };

        // Try to find a valid MSBuild.exe
        std::filesystem::path msBuildPath;
        bool foundMsBuild = false;
        for (const auto& path : msBuildPaths) {
            if (std::filesystem::exists(path)) {
                msBuildPath = path;
                foundMsBuild = true;
                break;
            }
        }

        // If no MSBuild path was found, print an error and return
        if (!foundMsBuild) {
            std::cerr << "MSBuild.exe not found in any known Visual Studio 2022 edition paths." << std::endl;
            return;
        }

        // Ensure the project file exists
        if (!std::filesystem::exists(filepath)) {
            std::cerr << "Project file not found: " << filepath << std::endl;
            return;
        }

        // Construct the command string
        std::string command = std::format(
            "cd \"{}\" && \"{}\" \"{}\" -property:Configuration={} -t:restore,build",
            filepath.parent_path().string(),
            msBuildPath.string(),
            filepath.filename().string(),
            "Debug"
        );

        // Print the command for debugging purposes
        std::cout << "Executing command: " << command << std::endl;

        // Execute the command
        int result = system(command.c_str());
        if (result != 0) {
            std::cerr << "Build failed with error code: " << result << std::endl;
        }
    }

	void ScriptBuilder::BuildScriptAssembly()
	{
		BuildCSProject(Project::GetScriptProjectPath());
	}

}
