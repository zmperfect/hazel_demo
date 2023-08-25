#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

    struct Renderer2DStorage
    {
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> FlatColorShader;
        Ref<Shader> TextureShader;
    };

    static Renderer2DStorage* s_Data;

    void Renderer2D::Init()
    {
        s_Data = new Renderer2DStorage();
        s_Data->QuadVertexArray = VertexArray::Create();

        float squareVertices[5 * 4] = {//方形顶点坐标,纹理坐标
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
              0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
              0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };

        Ref<VertexBuffer> squareVB;//顶点缓冲区
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));//创建顶点缓冲区
        squareVB->SetLayout({//设置顶点缓冲区的布局,位置和纹理
                   { ShaderDataType::Float3, "a_Position" },
                   { ShaderDataType::Float2, "a_TexCoord" }
            });
        s_Data->QuadVertexArray->AddVertexBuffer(squareVB);//添加顶点缓冲区

        uint32_t squareIndices[6] = {//方形顶点索引
                   0, 1, 2, 2, 3, 0 };
        Ref<IndexBuffer> squareIB;//索引缓冲区
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));//创建索引缓冲区
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB);//设置索引缓冲区

        s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");//创建着色器
        s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");//创建纹理着色器
        s_Data->TextureShader->Bind();//绑定纹理着色器
        s_Data->TextureShader->SetInt("u_Texture", 0);//设置纹理着色器的纹理
    }

    void Renderer2D::Shutdown()
    {
        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        s_Data->FlatColorShader->Bind();//绑定着色器
        s_Data->FlatColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());//设置着色器的视图投影矩阵

        s_Data->TextureShader->Bind();//绑定纹理着色器
        s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());//设置纹理着色器的视图投影矩阵
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
        s_Data->FlatColorShader->Bind();//绑定着色器
        s_Data->FlatColorShader->SetFloat4("u_Color", color);//设置着色器的颜色

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//平移矩阵*缩放矩阵
        s_Data->FlatColorShader->SetMat4("u_Transform", transform);//设置着色器的变换矩阵

        s_Data->QuadVertexArray->Bind();//绑定顶点数组
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);//绘制顶点数组
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, texture);//位置(将z设为0)，大小，纹理
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
    {
        s_Data->TextureShader->Bind();//绑定纹理着色器

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//平移矩阵*缩放矩阵
        s_Data->TextureShader->SetMat4("u_Transform", transform);//设置纹理着色器的变换矩阵

        texture->Bind();//绑定纹理

        s_Data->QuadVertexArray->Bind();//绑定顶点数组
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);//绘制顶点数组
    }
}