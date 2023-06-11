#pragma once

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

	class HAZEL_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual	~Layer();

		//��ν�Ƿǳ������ˣ�ÿ��Layer���Ա����õ�ʱ���ڣ�
		//������Stack��ʱ��
		//���߳�Stack��ʱ��
		//��Ҫ��Update��ʱ��
		//���յ��¼���ʱ��
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	private:
			std::string m_DebugName;
	};
}