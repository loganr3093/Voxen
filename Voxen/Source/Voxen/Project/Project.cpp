#include "voxpch.h"
#include "Voxen/Project/Project.h"

#include "Voxen/Project/ProjectSerializer.h"

namespace Voxen
{
	Ref<Project> Project::New()
	{
		s_ActiveProject = CreateRef<Project>();
		return Ref<Project>();
	}

	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();

		ProjectSerializer serializer(project);
		if (!serializer.Deserialize(path))
			return nullptr;

		project->m_ProjectDirectory = path.parent_path();

		s_ActiveProject = project;
		return s_ActiveProject;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(path))
		{
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			return true;
		}

		return false;
	}
}