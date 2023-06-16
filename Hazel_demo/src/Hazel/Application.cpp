#include "hzpch.h"
#include "Application.h"

#include "Hazel/Log.h"

#include <glad/glad.h>

#include "Input.h"

namespace Hazel {

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());//创建一个窗口
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));//设置窗口的回调函数

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

		glGenVertexArrays(1, &m_VertexArray);//创建一个顶点数组
		glBindVertexArray(m_VertexArray);//绑定顶点数组

		glGenBuffers(1, &m_VertexBuffer);//创建一个顶点缓冲区
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);//绑定顶点缓冲区

		float vertices[3 * 3] = {//顶点坐标
            -0.5f, -0.5f, 0.0f,//左下角
             0.5f, -0.5f, 0.0f,//右下角
             0.0f,  0.5f, 0.0f//顶点
        };

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//将顶点数据传入缓冲区

		glEnableVertexAttribArray(0);//启用顶点属性
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);//设置顶点属性

		glGenBuffers(1, &m_IndexBuffer);//创建一个索引缓冲区
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);//绑定索引缓冲区

		unsigned int indices[3] = { 0, 1, 2 };//索引
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);//将索引数据传入缓冲区

        std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

        std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}
		)";

        m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);//将layer压入栈中
		layer->OnAttach();//将layer附加到窗口上
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);//将overlay压入栈中
		layer->OnAttach();
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

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
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();//绑定着色器
            glBindVertexArray(m_VertexArray);//绑定顶点数组
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);//绘制三角形

			for (Layer* layer : m_LayerStack)//遍历layer栈
				layer->OnUpdate();//更新每一个layer

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

}