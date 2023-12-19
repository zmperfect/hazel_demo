#pragma once

#include "Hazel/Core/Layer.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel {
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

        void Begin();
        void End();

		void BlockEvents(bool block) { m_BlockEvents = block; } //设置是否阻塞事件

		void SetDarkThemeColors(); //设置暗色主题颜色
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}
