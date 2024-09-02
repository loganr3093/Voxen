#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Voxen
{
	// Once we have projects, change this
	extern const std::filesystem::path g_AssetPath = "assets";
	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(g_AssetPath)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

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
		ImGui::SliderFloat("##thumbnailsize", &thumbnailSize, 32, 512, "", ImGuiSliderFlags_None);

		// Reverting to the default frame padding and item width
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		// Start a new child region that will contain the rest of the content and is scrollable
		ImGui::BeginChild("##content_region", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);

		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
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