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
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create();//����һ������
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));//���ô��ڵĻص�����

		Renderer::Init();//��ʼ����Ⱦ��

        m_ImGuiLayer = new ImGuiLayer();//����һ��imgui��
        PushOverlay(m_ImGuiLayer);//��imgui��ѹ��ջ��
	}

	Application::~Application()
    {
        Renderer::Shutdown();//�ر���Ⱦ��
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
		dispatcher.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));//�����ڹر��¼��ַ���OnWindowClose����
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));//�����ڴ�С�ı��¼��ַ���OnWindowResize����

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