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
    m_SquareVA = Hazel::VertexArray::Create();//顶点数组

    float squareVertices[5 * 4] = {//方形顶点坐标
            - 0.5f, -0.5f, 0.0f,
			   0.5f, -0.5f, 0.0f,
			   0.5f,  0.5f, 0.0f,
            - 0.5f,  0.5f, 0.0f
    };

    Hazel::Ref<Hazel::VertexBuffer> squareVB;//顶点缓冲区
    squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));//创建顶点缓冲区
    squareVB->SetLayout({//设置顶点缓冲区的布局
               { Hazel::ShaderDataType::Float3, "a_Position" },
                  });
    m_SquareVA->AddVertexBuffer(squareVB);//添加顶点缓冲区

    uint32_t squareIndices[6] = {//方形顶点索引
               0, 1, 2, 2, 3, 0 };
    Hazel::Ref<Hazel::IndexBuffer> squareIB;//索引缓冲区
    squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));//创建索引缓冲区
    m_SquareVA->SetIndexBuffer(squareIB);//设置索引缓冲区

    m_FlatColorShader = Hazel::Shader::Create("assets/shaders/FlatColor.glsl");//创建着色器
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }指的是RGBA，即红绿蓝透明度，范围是0~1
    Hazel::RenderCommand::Clear();//清除颜色缓冲区和深度缓冲区

    Hazel::Renderer::BeginScene(m_CameraController.GetCamera());//开始渲染场景

    std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();//绑定着色器
    std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);//上传着色器的颜色

    Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));//提交渲染

    Hazel::Renderer::EndScene();//结束渲染场景
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");//开始设置
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));//编辑颜色
    ImGui::End();//结束设置
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
    m_CameraController.OnEvent(e);
}
