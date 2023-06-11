#pragma once

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

	class HAZEL_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual	~Layer();

		//可谓是非常抽象了，每个Layer可以被调用的时机在：
		//被塞进Stack的时候
		//被踢出Stack的时候
		//被要求Update的时候
		//被收到事件的时候
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