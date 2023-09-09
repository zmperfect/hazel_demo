#include "hzpch.h"
#include "Hazel/Core/LayerStack.h"

namespace Hazel {

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();//将layer从窗口上分离
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
		layer->OnAttach();//将layer附加到窗口上
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);//find() 函数用于在 vector 容器中查找指定元素，它返回的是一个迭代器，指向的是第一个等于指定值的元素。
		if (it != m_Layers.begin() + m_LayerInsertIndex)//修复bug，如果it不等于m_Layers.begin() + m_LayerInsertIndex，说明找到了，就执行下面的代码，否则不执行
		{
			layer->OnDetach();
			m_Layers.erase(it);//erase() 函数用于删除 vector 容器中的指定元素，它返回的是一个迭代器，指向的是被删除元素的下一个元素。
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}
}