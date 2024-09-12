#include "voxpch.h"
#include "Voxen/Utilities/PlatformUtils.h"

#include "Voxen/Core/Application.h"

#include <commdlg.h>
#include <shlobj.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <shobjidl.h>

namespace Voxen
{
	float Time::GetTime()
	{
		return glfwGetTime();
	}

    std::filesystem::path FileDialogs::OpenFile(const char* filter)
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        CHAR currentDir[256] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryA(256, currentDir))
            ofn.lpstrInitialDir = currentDir;
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE)
            return std::filesystem::path(ofn.lpstrFile);

        return std::filesystem::path();
    }

    std::filesystem::path FileDialogs::SaveFile(const char* filter)
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        CHAR currentDir[256] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryA(256, currentDir))
            ofn.lpstrInitialDir = currentDir;
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        ofn.lpstrDefExt = strchr(filter, '\0') + 1;

        if (GetSaveFileNameA(&ofn) == TRUE)
            return std::filesystem::path(ofn.lpstrFile);

        return std::filesystem::path();
    }

    std::filesystem::path FileDialogs::OpenFolder()
    {
        // Initialize COM library
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr)) {
            return std::filesystem::path();  // COM initialization failed
        }

        IFileDialog* pfd = nullptr;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

        if (SUCCEEDED(hr)) {
            // Set options to pick folders
            DWORD dwOptions;
            hr = pfd->GetOptions(&dwOptions);
            if (SUCCEEDED(hr)) {
                pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);  // Add FOS_PICKFOLDERS to allow folder selection
            }

            // Show the dialog
            hr = pfd->Show(glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow()));
            if (SUCCEEDED(hr)) {
                // Get the result (selected folder)
                IShellItem* psiResult;
                hr = pfd->GetResult(&psiResult);
                if (SUCCEEDED(hr)) {
                    // Get the folder path
                    PWSTR pszFilePath = nullptr;
                    hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr)) {
                        // Convert wide string (PWSTR) to std::filesystem::path
                        std::filesystem::path folderPath = pszFilePath;
                        CoTaskMemFree(pszFilePath);  // Free memory allocated by the dialog
                        psiResult->Release();
                        pfd->Release();
                        CoUninitialize();  // Uninitialize COM

                        return folderPath;
                    }

                    psiResult->Release();
                }
            }

            pfd->Release();
        }

        CoUninitialize();  // Uninitialize COM on failure
        return std::filesystem::path();  // Return an empty path if no folder is selected or an error occurred
    }
}