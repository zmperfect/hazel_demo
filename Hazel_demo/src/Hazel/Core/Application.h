#pragma once

#include "Hazel/Core/Core.h"

#include "Hazel/Core/Window.h"
#include "Hazel/Core/LayerStack.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

#include "Hazel/Core/Timestep.h"

#include "Hazel/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);//main����������

namespace Hazel {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);//��һ��Layerѹ��LayerStack
		void PushOverlay(Layer* layer);//��һ��Layerѹ��LayerStack�Ķ���

		Window& GetWindow() { return *m_Window; }

		static Application& Get() { return *s_Instance; }
	private:
		void Run();//����Application
		bool OnWindowClose(WindowCloseEvent& e);//�����ڹر��¼�
		bool OnWindowResize(WindowResizeEvent& e);//�����ڴ�С�ı��¼�)
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;//LayerStack��һ���������������Layer

		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);//����main����ΪApplication����Ԫ����
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}