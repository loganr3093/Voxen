#include "voxpch.h"
#include "Voxen/Scene/SceneSerializer.h"

#include "Voxen/Core/UUID.h"

#include "Voxen/Scene/Entity.h"
#include "Voxen/Scene/Components.h"

#include "Voxen/Scripting/ScriptEngine.h"

#include "Voxen/Utilities/YAMLHelper.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace Voxen
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		VOX_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent
			out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;

			// Fields
			Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
			const auto& fields = entityClass->GetFields();
			if (fields.size() > 0)
			{
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
				out << YAML::BeginSeq;
				for (const auto& [name, field] : fields)
				{
					if (entityFields.find(name) == entityFields.end())
						continue;

					out << YAML::BeginMap; // ScriptField
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);

					out << YAML::Key << "Data" << YAML::Value;
					ScriptFieldInstance& scriptField = entityFields.at(name);

					switch (field.Type)
					{
						WRITE_SCRIPT_FIELD(Float, float);
						WRITE_SCRIPT_FIELD(Double, double);
						WRITE_SCRIPT_FIELD(Bool, bool);
						WRITE_SCRIPT_FIELD(Char, char);
						WRITE_SCRIPT_FIELD(Byte, int8_t);
						WRITE_SCRIPT_FIELD(Short, int16_t);
						WRITE_SCRIPT_FIELD(Int, int32_t);
						WRITE_SCRIPT_FIELD(Long, int64_t);
						WRITE_SCRIPT_FIELD(UByte, uint8_t);
						WRITE_SCRIPT_FIELD(UShort, uint16_t);
						WRITE_SCRIPT_FIELD(UInt, uint32_t);
						WRITE_SCRIPT_FIELD(ULong, uint64_t);
						WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
						WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
						WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
						WRITE_SCRIPT_FIELD(Entity, UUID);
					}
					out << YAML::EndMap; // ScriptFields
				}
				out << YAML::EndSeq;
			}

			out << YAML::EndMap; // ScriptComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << filepath.stem().string();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		for (const auto [entityID, ref] : m_Scene->m_Registry.storage<TagComponent>().each())
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			VOX_CORE_ERROR("Failed to load .vscene file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		bool shouldSerialize = false;
		std::string sceneName = data["Scene"].as<std::string>();

		VOX_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		m_Scene->SetName(filepath.stem().string());

		auto entities = data["Entities"];
		if (!entities) return true;
		for (auto entity : entities)
		{
			uint64 uuid = entity["Entity"].as<uint64>();

			std::string name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent)
				name = tagComponent["Tag"].as<std::string>();

			VOX_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

			Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				// Entities always have transforms
				auto& tc = deserializedEntity.GetComponent<TransformComponent>();
				tc.Translation = transformComponent["Translation"].as<Vector3>(Vector3(0.0f));
				tc.Rotation = transformComponent["Rotation"].as<Vector3>(Vector3(0.0f));
				tc.Scale = transformComponent["Scale"].as<Vector3>(Vector3(0.0f));
			}

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				auto& component = deserializedEntity.AddComponent<CameraComponent>();
				const auto& cameraNode = cameraComponent["Camera"];

				component.Camera = SceneCamera();
				auto& camera = component.Camera;

				if (cameraNode.IsMap())
				{
					if (cameraNode["ProjectionType"])
						camera.SetProjectionType((SceneCamera::ProjectionType)cameraNode["ProjectionType"].as<int>());
					if (cameraNode["PerspectiveFOV"])
						camera.SetPerspectiveVerticalFOV(cameraNode["PerspectiveFOV"].as<float>());
					if (cameraNode["PerspectiveNear"])
						camera.SetPerspectiveNearClip(cameraNode["PerspectiveNear"].as<float>());
					if (cameraNode["PerspectiveFar"])
						camera.SetPerspectiveFarClip(cameraNode["PerspectiveFar"].as<float>());
					if (cameraNode["OrthographicSize"])
						camera.SetOrthographicSize(cameraNode["OrthographicSize"].as<float>());
					if (cameraNode["OrthographicNear"])
						camera.SetOrthographicNearClip(cameraNode["OrthographicNear"].as<float>());
					if (cameraNode["OrthographicFar"])
						camera.SetOrthographicFarClip(cameraNode["OrthographicFar"].as<float>());
				}

				component.Primary = cameraComponent["Primary"].as<bool>();
			}

			auto scriptComponent = entity["ScriptComponent"];
			if (scriptComponent)
			{
				auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
				sc.ClassName = scriptComponent["ClassName"].as<std::string>();

				auto scriptFields = scriptComponent["ScriptFields"];
				if (scriptFields)
				{
					Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
					if (entityClass)
					{
						const auto& fields = entityClass->GetFields();
						auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

						for (auto scriptField : scriptFields)
						{
							std::string name = scriptField["Name"].as<std::string>();
							std::string typeString = scriptField["Type"].as<std::string>();
							ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

							ScriptFieldInstance& fieldInstance = entityFields[name];

							// TODO(Yan): turn this assert into Hazelnut log warning
							VOX_CORE_ASSERT(fields.find(name) != fields.end());

							if (fields.find(name) == fields.end())
								continue;

							fieldInstance.Field = fields.at(name);

							switch (type)
							{
								READ_SCRIPT_FIELD(Float, float);
								READ_SCRIPT_FIELD(Double, double);
								READ_SCRIPT_FIELD(Bool, bool);
								READ_SCRIPT_FIELD(Char, char);
								READ_SCRIPT_FIELD(Byte, int8_t);
								READ_SCRIPT_FIELD(Short, int16_t);
								READ_SCRIPT_FIELD(Int, int32_t);
								READ_SCRIPT_FIELD(Long, int64_t);
								READ_SCRIPT_FIELD(UByte, uint8_t);
								READ_SCRIPT_FIELD(UShort, uint16_t);
								READ_SCRIPT_FIELD(UInt, uint32_t);
								READ_SCRIPT_FIELD(ULong, uint64_t);
								READ_SCRIPT_FIELD(Vector2, glm::vec2);
								READ_SCRIPT_FIELD(Vector3, glm::vec3);
								READ_SCRIPT_FIELD(Vector4, glm::vec4);
								READ_SCRIPT_FIELD(Entity, UUID);
							}
						}
					}
				}
			}

			auto spriteRendererComponent = entity["SpriteRendererComponent"];
			if (spriteRendererComponent)
			{
				auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
				src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
			}
		}

		return true;
	}
}