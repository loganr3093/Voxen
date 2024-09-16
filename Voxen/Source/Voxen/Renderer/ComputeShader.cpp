#include "voxpch.h"
#include "Voxen/Renderer/ComputeShader.h"

#include "Voxen/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLComputeShader.h"

namespace Voxen
{
	Ref<ComputeShader> ComputeShader::Create(const std::string& filePath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: VOX_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLComputeShader>(filePath);
		}

		VOX_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	Ref<ComputeShader> ComputeShader::Create(const std::string& name, const std::string& computeSrc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: VOX_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLComputeShader>(name, computeSrc);
		}

		VOX_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


	// ComputeShader Library
	void ComputeShaderLibrary::Add(const std::string& name, const Ref<ComputeShader>& ComputeShader)
	{
		VOX_CORE_ASSERT(!Exists(name), "ComputeShader already exists");
		m_ComputeShaders[name] = ComputeShader;
	}
	void ComputeShaderLibrary::Add(const Ref<ComputeShader>& ComputeShader)
	{
		auto& name = ComputeShader->GetName();
		Add(name, ComputeShader);
	}
	Ref<ComputeShader> ComputeShaderLibrary::Load(const std::string& filePath)
	{
		auto ComputeShader = ComputeShader::Create(filePath);
		Add(ComputeShader);
		return ComputeShader;
	}
	Ref<ComputeShader> ComputeShaderLibrary::Load(const std::string& name, const std::string& filePath)
	{
		auto ComputeShader = ComputeShader::Create(filePath);
		Add(name, ComputeShader);
		return ComputeShader;
	}
	Ref<ComputeShader> ComputeShaderLibrary::Get(const std::string& name)
	{
		VOX_CORE_ASSERT(Exists(name), "ComputeShader not found");
		return m_ComputeShaders[name];
	}

	bool ComputeShaderLibrary::Exists(const std::string& name)
	{
		return m_ComputeShaders.find(name) != m_ComputeShaders.end();
	}
}