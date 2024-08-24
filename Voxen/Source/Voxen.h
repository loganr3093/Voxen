#pragma once

// For use by Voxen applications

// Engine
// ****************************
#include "Voxen/Core/Application.h"
#include "Voxen/ImGui/ImGuiLayer.h"
#include "Voxen/Core/Input.h"
#include "Voxen/Core/Layer.h"
#include "Voxen/Core/Log.h"
#include "Voxen/Core/Assert.h"

#include "Voxen/Core/Timestep.h"

#include "Voxen/Scene/Scene.h"
#include "Voxen/Scene/Entity.h"
#include "Voxen/Scene/ScriptableEntity.h"
#include "Voxen/Scene/Components.h"

// Macro files
// ****************************
#include "Voxen/Core/KeyCodes.h"
#include "Voxen/Core/MouseCodes.h"

// Renderer
// ****************************
#include "Voxen/Renderer/Renderer2D.h"
#include "Voxen/Renderer/Renderer.h"
#include "Voxen/Renderer/RenderCommand.h"

#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/Framebuffer.h"
#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/Buffer.h"
#include "Voxen/Renderer/VertexArray.h"

#include "Voxen/Renderer/OrthographicCamera.h"
#include "Voxen/Renderer/OrthographicCameraController.h"