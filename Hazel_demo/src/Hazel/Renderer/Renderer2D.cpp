#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

    struct Renderer2DStorage
    {
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> FlatColorShader;
    };

    static Renderer2DStorage* s_Data;

    void Renderer2D::Init()
    {
        s_Data = new Renderer2DStorage();
        s_Data->QuadVertexArray = VertexArray::Create();

        float squareVertices[5 * 4] = {//方形顶点坐标
            -0.5f, -0.5f, 0.0f,
              0.5f, -0.5f, 0.0f,
              0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };

        Ref<VertexBuffer> squareVB;//顶点缓冲区
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));//创建顶点缓冲区
        squareVB->SetLayout({//设置顶点缓冲区的布局
                   { ShaderDataType::Float3, "a_Position" },
            });
        s_Data->QuadVertexArray->AddVertexBuffer(squareVB);//添加顶点缓冲区

        uint32_t squareIndices[6] = {//方形顶点索引
                   0, 1, 2, 2, 3, 0 };
        Ref<IndexBuffer> squareIB;//索引缓冲区
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));//创建索引缓冲区
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB);//设置索引缓冲区

        s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");//创建着色器
    }

    void Renderer2D::Shutdown()
    {
        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->Bind();//绑定着色器
        std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());//上传着色器的视图投影矩阵
        std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->UploadUniformMat4("u_Transform", glm::mat4(1.0f));//上传着色器的变换矩阵
    }

    void Renderer2D::EndScene()
    {
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, color);//位置(将z设为0)，大小，颜色
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->Bind();//绑定着色器
        std::dynamic_pointer_cast<Hazel::OpenGLShader>(s_Data->FlatColorShader)->UploadUniformFloat4("u_Color", color);//上传着色器的颜色

        s_Data->QuadVertexArray->Bind();//绑定顶点数组
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);//绘制顶点数组
    }
}