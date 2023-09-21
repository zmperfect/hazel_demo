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
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

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
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        Renderer::Shutdown();//�ر���Ⱦ��
    }

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		m_LayerStack.PushLayer(layer);//��layerѹ��ջ��
		layer->OnAttach();//��layer���ӵ�������
	}

	void Application::PushOverlay(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		m_LayerStack.PushOverlay(layer);//��overlayѹ��ջ��
		layer->OnAttach();//��overlay���ӵ�������
	}

	void Application::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));//�����ڹر��¼��ַ���OnWindowClose����
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));//�����ڴ�С�ı��¼��ַ���OnWindowResize����

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it )//�Ӻ���ǰ����
		{
			(*it)->OnEvent(e);//���¼����ݸ�ÿһ��layer
			if (e.Handled)//����¼�������������ѭ��
				break;
		}
	}

	void Application::Run()
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		while (m_Running)
		{
			HZ_PROFILE_SCOPE("RunLoop");//��ȡ������

            float time = (float)glfwGetTime();//��ȡ��ǰʱ��
            Timestep timestep = time - m_LastFrameTime;//����ʱ���
            m_LastFrameTime = time;//������һ֡ʱ��

			//�������û����С�������layer
			if (!m_Minimized)
			{
				HZ_PROFILE_SCOPE("LayerStack OnUpdate");//��ȡ������

                for (Layer* layer : m_LayerStack)//����layerջ
                    layer->OnUpdate(timestep);//����ÿһ��layer
			}

            m_ImGuiLayer->Begin();//��ʼimgui��
			{
				HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");//��ȡ������

				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();//����ÿһ��layer��imgui��
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
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

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