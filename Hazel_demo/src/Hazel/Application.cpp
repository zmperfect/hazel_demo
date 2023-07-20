#include "hzpch.h"
#include "Application.h"

#include "Hazel/Log.h"

#include "Hazel/Renderer/Renderer.h"

#include "Input.h"

namespace Hazel {

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
		: m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());//����һ������
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));//���ô��ڵĻص�����

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

		m_VertexArray.reset(VertexArray::Create());//����һ����������

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,//�������꣬��ɫ
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,//�������꣬��ɫ
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f//�������꣬��ɫ
        };
		
		std::shared_ptr<VertexBuffer> vertexBuffer;//���㻺����
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));//����һ�����㻺����
		BufferLayout layout = {//����һ������
            { ShaderDataType::Float3, "a_Position" },//��������
            { ShaderDataType::Float4, "a_Color" }//������ɫ
        };
		vertexBuffer->SetLayout(layout);//���ö��㻺�����Ĳ���
		m_VertexArray->AddVertexBuffer(vertexBuffer);//�����㻺������ӵ�����������

		uint32_t indices[3] = { 0, 1, 2 };//����
		std::shared_ptr<IndexBuffer> indexBuffer;//����������
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));//����һ������������
		m_VertexArray->SetIndexBuffer(indexBuffer);//���ö������������������

		m_SquareVA.reset(VertexArray::Create());//����һ�����ζ�������

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		//�洢��ɫ��Դ����
        std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
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

		//�洢��ɫ��Դ����
		std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
			}
		)";

		//�洢��ɫ��Դ����
		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
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
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

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
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//����������ɫ
			RenderCommand::Clear();//����

			m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });//�������λ��
			m_Camera.SetRotation(45.0f);//���������ת�Ƕ�

			Renderer::BeginScene(m_Camera);//��ʼ��Ⱦ����

			Renderer::Submit(m_BlueShader, m_SquareVA);//�ύ���ζ�������

			Renderer::Submit(m_Shader, m_VertexArray);//�ύ��������

			Renderer::EndScene();//������Ⱦ����

			for (Layer* layer : m_LayerStack)//����layerջ
				layer->OnUpdate();//����ÿһ��layer

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