#include "hzpch.h"
#include "Hazel/Core/Application.h"

#include "Hazel/Core/Log.h"

#include "Hazel/Renderer/Renderer.h"

#include "Hazel/Core/Input.h"

#include <glfw/glfw3.h>

namespace Hazel {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_PROFILE_FUNCTION();//获取函数签名

		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create();//创建一个窗口
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));//设置窗口的回调函数

		Renderer::Init();//初始化渲染器

        m_ImGuiLayer = new ImGuiLayer();//创建一个imgui层
        PushOverlay(m_ImGuiLayer);//将imgui层压入栈中
	}

	Application::~Application()
    {
		HZ_PROFILE_FUNCTION();//获取函数签名

        Renderer::Shutdown();//关闭渲染器
    }

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();//获取函数签名

		m_LayerStack.PushLayer(layer);//将layer压入栈中
		layer->OnAttach();//将layer附加到窗口上
	}

	void Application::PushOverlay(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();//获取函数签名

		m_LayerStack.PushOverlay(layer);//将overlay压入栈中
		layer->OnAttach();//将overlay附加到窗口上
	}

	void Application::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();//获取函数签名

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));//将窗口关闭事件分发给OnWindowClose函数
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));//将窗口大小改变事件分发给OnWindowResize函数

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it )//从后往前遍历
		{
			(*it)->OnEvent(e);//将事件传递给每一个layer
			if (e.Handled)//如果事件被处理则跳出循环
				break;
		}
	}

	void Application::Run()
	{
		HZ_PROFILE_FUNCTION();//获取函数签名

		while (m_Running)
		{
			HZ_PROFILE_SCOPE("RunLoop");//获取作用域

            float time = (float)glfwGetTime();//获取当前时间
            Timestep timestep = time - m_LastFrameTime;//计算时间差
            m_LastFrameTime = time;//更新上一帧时间

			//如果窗口没有最小化则更新layer
			if (!m_Minimized)
			{
				HZ_PROFILE_SCOPE("LayerStack OnUpdate");//获取作用域

                for (Layer* layer : m_LayerStack)//遍历layer栈
                    layer->OnUpdate(timestep);//更新每一个layer
			}

            m_ImGuiLayer->Begin();//开始imgui层
			{
				HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");//获取作用域

				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();//更新每一个layer的imgui层
			}
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
		HZ_PROFILE_FUNCTION();//获取函数签名

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