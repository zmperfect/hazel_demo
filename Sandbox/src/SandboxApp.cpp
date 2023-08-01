#include <Hazel.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
        m_VertexArray.reset(Hazel::VertexArray::Create());//����һ����������

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,//�������꣬��ɫ
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,//�������꣬��ɫ
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f//�������꣬��ɫ
        };

        Hazel::Ref<Hazel::VertexBuffer> vertexBuffer;//���㻺����
        vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));//����һ�����㻺����
        Hazel::BufferLayout layout = {//����һ������
            { Hazel::ShaderDataType::Float3, "a_Position" },//��������
            { Hazel::ShaderDataType::Float4, "a_Color" }//������ɫ
        };
        vertexBuffer->SetLayout(layout);//���ö��㻺�����Ĳ���
        m_VertexArray->AddVertexBuffer(vertexBuffer);//�����㻺������ӵ�����������

        uint32_t indices[3] = { 0, 1, 2 };//����
        Hazel::Ref<Hazel::IndexBuffer> indexBuffer;//����������
        indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));//����һ������������
        m_VertexArray->SetIndexBuffer(indexBuffer);//���ö������������������

        m_SquareVA.reset(Hazel::VertexArray::Create());//����һ�����ζ�������

        float squareVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,//�������꣬��������
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,//�������꣬��������
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,//�������꣬��������
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f//�������꣬��������
        };

        Hazel::Ref<Hazel::VertexBuffer> squareVB;
        squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({
            { Hazel::ShaderDataType::Float3, "a_Position" },//��������
            { Hazel::ShaderDataType::Float2, "a_TexCoord" }//��������
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        Hazel::Ref<Hazel::IndexBuffer> squareIB;
        squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);

        //�洢������ɫ������
        std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		//Ƭ����ɫ������
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

        m_Shader = Hazel::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);//����һ������λ����ɫ����ɫ��

        //shader
        std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		//Ƭ����ɫ������
        std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

        m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);//����һ����ɫ��ɫ��

        auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

        m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
        m_LJRLogoTexture = Hazel::Texture2D::Create("assets/textures/LJRLogo.png");

        std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
        std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

	}

	void OnUpdate(Hazel::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		//��Ⱦ��ͼ
        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//����������ɫ
        Hazel::RenderCommand::Clear();//����

        Hazel::Renderer::BeginScene(m_CameraController.GetCamera());//��ʼ��Ⱦ����

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));//����

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();//��shader
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);//�ϴ���ɫ

		//��Ⱦ��20*20������
		for(int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		//���������ύ
        m_Texture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_LJRLogoTexture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//�������ύ
		//Hazel::Renderer::Submit(m_Shader, m_VertexArray);//�ύ��������

		Hazel::Renderer::EndScene();//������Ⱦ����

	}

	//��ȾGUI
	virtual void OnImGuiRender() override
	{
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));//���÷�����ɫ
        ImGui::End();
	}

	void OnEvent(Hazel::Event& e) override
	{
        m_CameraController.OnEvent(e);//����������¼�
	}
private:
	Hazel::ShaderLibrary m_ShaderLibrary;//��ɫ����
    Hazel::Ref<Hazel::Shader> m_Shader;//shader
    Hazel::Ref<Hazel::VertexArray> m_VertexArray;//��������

    Hazel::Ref<Hazel::Shader> m_FlatColorShader;//��ɫshader
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;//���ζ�������

	Hazel::Ref<Hazel::Texture2D> m_Texture, m_LJRLogoTexture;//����,logo����

	Hazel::OrthographicCameraController m_CameraController;//���������
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };//������ɫ
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}