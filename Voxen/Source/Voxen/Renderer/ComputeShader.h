#pragma once

namespace Voxen
{
	class ComputeShader
	{
	public:
		virtual ~ComputeShader() = default;

		virtual void Dispatch(uint32 xGroups, uint32 yGroups, uint32 zGroups) const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32 count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetVector2(const std::string& name, const Vector2& value) = 0;
		virtual void SetVector3(const std::string& name, const Vector3& value) = 0;
		virtual void SetVector4(const std::string& name, const Vector4& value) = 0;
		virtual void SetMat4(const std::string& name, const Matrix4& value) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<ComputeShader> Create(const std::filesystem::path& filepath);
		static Ref<ComputeShader> Create(const std::string& filepath);
		static Ref<ComputeShader> Create(const std::string& name, const std::string& computeSrc);
	};

	class ComputeShaderLibrary
	{
	public:
		void Add(const std::string& name, const Ref<ComputeShader>& ComputeShader);
		void Add(const Ref<ComputeShader>& ComputeShader);
		Ref<ComputeShader> Load(const std::string& filepath);
		Ref<ComputeShader> Load(const std::string& name, const std::string& filepath);

		Ref<ComputeShader> Get(const std::string& name);

		bool Exists(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<ComputeShader>> m_ComputeShaders;
	};

}