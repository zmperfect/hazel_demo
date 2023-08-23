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

		m_Window = std::unique_ptr<Window>(Window::Create());//����һ������
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));//���ô��ڵĻص�����

		Renderer::Init();//��ʼ����Ⱦ��

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);//��layerѹ��ջ��
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);//��overlayѹ��ջ��
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));//�����ڹر��¼��ַ���OnWindowClose����
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));//�����ڴ�С�ı��¼��ַ���OnWindowResize����

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )//�Ӻ���ǰ����
		{
			(*--it)->OnEvent(e);//���¼����ݸ�ÿһ��layer
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
            float time = (float)glfwGetTime();//��ȡ��ǰʱ��
            Timestep timestep = time - m_LastFrameTime;//����ʱ���
            m_LastFrameTime = time;//������һ֡ʱ��

			//�������û����С�������layer
			if (!m_Minimized)
			{
                for (Layer* layer : m_LayerStack)//����layerջ
                    layer->OnUpdate(timestep);//����ÿһ��layer
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
		//ǿ�ƴ��ڴ�СΪ0*0ʱʹ������С��
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());//��Ⱦ�����ڴ�С�ı�

		return false;
	}
}