#include "hzpch.h"
#include "Application.h"

#include "Hazel/Log.h"

#include <glad/glad.h>

#include "Input.h"

namespace Hazel {

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	//将ShaderDataType转换为OpenGLBaseType
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
        case Hazel::ShaderDataType::Float:    return GL_FLOAT;
        case Hazel::ShaderDataType::Float2:   return GL_FLOAT;
        case Hazel::ShaderDataType::Float3:   return GL_FLOAT;
        case Hazel::ShaderDataType::Float4:   return GL_FLOAT;
        case Hazel::ShaderDataType::Mat3:     return GL_FLOAT;
        case Hazel::ShaderDataType::Mat4:     return GL_FLOAT;
        case Hazel::ShaderDataType::Int:      return GL_INT;
        case Hazel::ShaderDataType::Int2:     return GL_INT;
        case Hazel::ShaderDataType::Int3:     return GL_INT;
        case Hazel::ShaderDataType::Int4:     return GL_INT;
        case Hazel::ShaderDataType::Bool:     return GL_BOOL;
        }

        HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

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

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,//顶点坐标，颜色
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,//顶点坐标，颜色
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f//顶点坐标，颜色
        };

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));//创建一个顶点缓冲区

		{
			BufferLayout layout = {
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float4, "a_Color" }
            };

			m_VertexBuffer->SetLayout(layout);//设置顶点缓冲区的布局
		}

		uint32_t index = 0;
		const auto& layout = m_VertexBuffer->GetLayout();//获取顶点缓冲区的布局
		//遍历布局
		for (const auto& element : layout)
		{
            glEnableVertexAttribArray(index);//启用顶点属性
			//设置顶点属性指针
            glVertexAttribPointer(index,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)element.Offset);
            index++;
		}

		uint32_t indices[3] = { 0, 1, 2 };//索引
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));//创建一个索引缓冲区

        std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

        std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

        m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
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
            glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);//绘制三角形

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