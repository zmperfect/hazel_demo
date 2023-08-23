#include "hzpch.h"
#include "Application.h"

#include "Hazel/Log.h"

#include "Hazel/Renderer/Renderer.h"

#include "Input.h"

#include <glfw/glfw3.h>

namespace Hazel {

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());//创建一个窗口
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));//设置窗口的回调函数

		Renderer::Init();//初始化渲染器

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);//将layer压入栈中
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);//将overlay压入栈中
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));//将窗口关闭事件分发给OnWindowClose函数
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));//将窗口大小改变事件分发给OnWindowResize函数

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )//从后往前遍历
		{
			(*--it)->OnEvent(e);//将事件传递给每一个layer
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
            float time = (float)glfwGetTime();//获取当前时间
            Timestep timestep = time - m_LastFrameTime;//计算时间差
            m_LastFrameTime = time;//更新上一帧时间

			//如果窗口没有最小化则更新layer
			if (!m_Minimized)
			{
                for (Layer* layer : m_LayerStack)//遍历layer栈
                    layer->OnUpdate(timestep);//更新每一个layer
			}

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiRender();
            m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		//强制窗口大小为0*0时使窗口最小化
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());//渲染器窗口大小改变

		return false;
	}
}