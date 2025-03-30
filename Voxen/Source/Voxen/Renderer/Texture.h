#pragma once

#include "Voxen/Core/Core.h"

#include <string>

namespace Voxen
{
	enum class TextureFormat
	{
		None = 0,
		RGBA8,			// 8-bit per channel RGBA
		RED_INTEGER,	// 32-bit integer red channel
		RGBA16F,		// 16-bit per channel RGBA
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
	};

	class TextureRW : public Texture
	{
	public:
		virtual void BindImage(uint32_t unit) const = 0;
		static Ref<TextureRW> Create(uint32_t width, uint32_t height);
		static Ref<TextureRW> Create(uint32_t width, uint32_t height, TextureFormat format);
	};
}

