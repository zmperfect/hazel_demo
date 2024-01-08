#pragma once

#include "Hazel/Core/Base.h"

#include "Hazel/Core/Window.h"
#include "Hazel/Core/LayerStack.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

#include "Hazel/Core/Timestep.h"

#include "Hazel/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);//main函数的声明

namespace Hazel {

	class Application
	{
	public:
		Application(const std::string& name = "Hazel App");
		virtual ~Application();

		void OnEvent(Event& e);//处理事件

		void PushLayer(Layer* layer);//将一个Layer压入LayerStack
		void PushOverlay(Layer* layer);//将一个Layer压入LayerStack的顶部

		Window& GetWindow() { return *m_Window; }

		void Close();//关闭Application

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }//返回ImGuiLayer

		static Application& Get() { return *s_Instance; }
	private:
		void Run();//运行Application
		bool OnWindowClose(WindowCloseEvent& e);//处理窗口关闭事件
		bool OnWindowResize(WindowResizeEvent& e);//处理窗口大小改变事件)
	private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;//LayerStack是一个容器，用来存放Layer

		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);//声明main函数为Application的友元函数
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}