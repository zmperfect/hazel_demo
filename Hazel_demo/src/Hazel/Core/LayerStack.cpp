#include "hzpch.h"
#include "Hazel/Core/LayerStack.h"

namespace Hazel {

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();//��layer�Ӵ����Ϸ���
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
		layer->OnAttach();//��layer���ӵ�������
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);//find() ���������� vector �����в���ָ��Ԫ�أ������ص���һ����������ָ����ǵ�һ������ָ��ֵ��Ԫ�ء�
		if (it != m_Layers.begin() + m_LayerInsertIndex)//�޸�bug�����it������m_Layers.begin() + m_LayerInsertIndex��˵���ҵ��ˣ���ִ������Ĵ��룬����ִ��
		{
			layer->OnDetach();
			m_Layers.erase(it);//erase() ��������ɾ�� vector �����е�ָ��Ԫ�أ������ص���һ����������ָ����Ǳ�ɾ��Ԫ�ص���һ��Ԫ�ء�
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