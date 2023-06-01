#pragma once

#include "Core.h"

#include "Window.h"
#include "Hazel/LayerStack.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

namespace Hazel {

	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);//��һ��Layerѹ��LayerStack
		void PushOverlay(Layer* overlay);//��һ��Layerѹ��LayerStack�Ķ���
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;//LayerStack��һ���������������Layer
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}