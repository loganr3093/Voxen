#include "voxpch.h"
#include "LayerStack.h"

namespace Voxen
{
	LayerStack::LayerStack()
	{
		m_LayerInsertIndex = 0;
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		VOX_CORE_TRACE("Pushing layer \'{0}\'", layer->GetName().c_str());
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		VOX_CORE_TRACE("Pushing overlay \'{0}\'", overlay->GetName().c_str());
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		VOX_CORE_TRACE("Popping layer \'{0}\'", layer->GetName().c_str());
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
			layer->OnDetach();
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		VOX_CORE_TRACE("Popping overlay \'{0}\'", overlay->GetName().c_str());
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			overlay->OnDetach();
		}
	}
}