#include "ContentBrowserPanel.h"

#include "Voxen/Editor/EditorResources.h"

#include "Voxen/Project/Project.h"

#include <imgui/imgui.h>

namespace Voxen
{
	ContentBrowserPanel::ContentBrowserPanel()
		: m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
	}

	const static std::vector<std::string> excludedFileTypes = { ".bin", ".vs", ".auto", ".sln", ".csproj" };
	const static std::vector<std::string> excludedFiles = { };
	const static std::vector<std::string> excludedDirectories = { "Binaries", "Intermediates" };
	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		// Thinner Slider: Adjust frame padding for a thinner appearance
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));

		// Slider Width and Centering
		float sliderWidth = 480.0f; // Set a fixed width for the slider
		float panelWidth = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX((panelWidth - sliderWidth) * 0.5f); // Center the slider

		// Push the width of the slider
		ImGui::PushItemWidth(sliderWidth);

		// Display the slider, thinner, centered, and without displaying the number
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 32, 512, "", ImGuiSliderFlags_None);

		// Reverting to the default frame padding and item width
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		// Start a new child region that will contain the rest of the content and is scrollable
		ImGui::BeginChild("##content_region", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);

		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();
			std::string fileExtension = path.extension().string();

			// Files to skip
			if (
				filenameString[0] == '.' ||
				std::find(excludedDirectories.begin(), excludedDirectories.end(), filenameString) != excludedDirectories.end() ||
				std::find(excludedFiles.begin(), excludedFiles.end(), filenameString) != excludedFiles.end() ||
				std::find(excludedFileTypes.begin(), excludedFileTypes.end(), fileExtension) != excludedFileTypes.end()
				)
			{
				continue;
			}

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = EditorResources::FolderIcon;

			if (!directoryEntry.is_directory())
			{
				icon = EditorResources::BlankFileIcon;

				// You can now check the file type based on the extension
				if (fileExtension == ".txt")
				{
					icon = EditorResources::TextFileIcon;
				}
				else if (fileExtension == ".cs")
				{
					icon = EditorResources::CSFileIcon;
				}
				else if (fileExtension == ".vscene")
				{
					icon = EditorResources::VSCFileIcon;
				}
				else if (fileExtension == ".lua")
				{
					icon = EditorResources::LuaFileIcon;
				}
				else if (fileExtension == ".bat")
				{
					icon = EditorResources::BatFileIcon;
				}
			}

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::EndChild(); // End of the scrollable content region

		ImGui::End(); // End of Content Browser window
	}
}