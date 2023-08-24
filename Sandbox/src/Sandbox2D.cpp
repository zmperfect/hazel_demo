#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
		: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
    m_SquareVA = Hazel::VertexArray::Create();//��������

    float squareVertices[5 * 4] = {//���ζ�������
            - 0.5f, -0.5f, 0.0f,
			   0.5f, -0.5f, 0.0f,
			   0.5f,  0.5f, 0.0f,
            - 0.5f,  0.5f, 0.0f
    };

    Hazel::Ref<Hazel::VertexBuffer> squareVB;//���㻺����
    squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));//�������㻺����
    squareVB->SetLayout({//���ö��㻺�����Ĳ���
               { Hazel::ShaderDataType::Float3, "a_Position" },
                  });
    m_SquareVA->AddVertexBuffer(squareVB);//��Ӷ��㻺����

    uint32_t squareIndices[6] = {//���ζ�������
               0, 1, 2, 2, 3, 0 };
    Hazel::Ref<Hazel::IndexBuffer> squareIB;//����������
    squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));//��������������
    m_SquareVA->SetIndexBuffer(squareIB);//��������������

    m_FlatColorShader = Hazel::Shader::Create("assets/shaders/FlatColor.glsl");//������ɫ��
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }ָ����RGBA����������͸���ȣ���Χ��0~1
    Hazel::RenderCommand::Clear();//�����ɫ����������Ȼ�����

    Hazel::Renderer::BeginScene(m_CameraController.GetCamera());//��ʼ��Ⱦ����

    std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();//����ɫ��
    std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);//�ϴ���ɫ������ɫ

    Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));//�ύ��Ⱦ

    Hazel::Renderer::EndScene();//������Ⱦ����
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");//��ʼ����
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));//�༭��ɫ
    ImGui::End();//��������
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
    m_CameraController.OnEvent(e);
}
