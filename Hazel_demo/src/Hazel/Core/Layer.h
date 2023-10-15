#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual	~Layer() = default;

		//��ν�Ƿǳ������ˣ�ÿ��Layer���Ա����õ�ʱ���ڣ�
		//������Stack��ʱ��
		//���߳�Stack��ʱ��
		//��Ҫ��Update��ʱ��
		//���յ��¼���ʱ��
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		const std::string& GetName() const { return m_DebugName; }
	private:
			std::string m_DebugName;
	};
}