#include "hzpch.h"
#include "Hazel/Renderer/Renderer2D.h"

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

    struct Renderer2DStorage
    {
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;
    };

    static Renderer2DStorage* s_Data;

    void Renderer2D::Init()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        s_Data = new Renderer2DStorage();
        s_Data->QuadVertexArray = VertexArray::Create();

        float squareVertices[5 * 4] = {//���ζ�������,��������
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
              0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
              0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };

        Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));//���㻺����
        squareVB->SetLayout({//���ö��㻺�����Ĳ���,λ�ú�����
                   { ShaderDataType::Float3, "a_Position" },
                   { ShaderDataType::Float2, "a_TexCoord" }
            });
        s_Data->QuadVertexArray->AddVertexBuffer(squareVB);//��Ӷ��㻺����

        uint32_t squareIndices[6] = {//���ζ�������
                   0, 1, 2, 2, 3, 0 };
        Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));//����������
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB);//��������������

        s_Data->WhiteTexture = Texture2D::Create(1, 1);//��������
        uint32_t whiteTextureData = 0xffffffff;//��������
        s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));//������������

        s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");//����������ɫ��
        s_Data->TextureShader->Bind();//��������ɫ��
        s_Data->TextureShader->SetInt("u_Texture", 0);//����������ɫ��������
    }

    void Renderer2D::Shutdown()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        s_Data->TextureShader->Bind();//��������ɫ��
        s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());//����������ɫ������ͼͶӰ����
    }

    void Renderer2D::EndScene()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, color);//λ��(��z��Ϊ0)����С����ɫ
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        s_Data->TextureShader->SetFloat4("u_Color", color);//����������ɫ������ɫ
        s_Data->WhiteTexture->Bind();//������

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//ƽ�ƾ���*���ž���

        s_Data->TextureShader->SetMat4("u_Transform", transform);//����������ɫ���ı任����
        s_Data->QuadVertexArray->Bind();//�󶨶�������
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);//���ƶ�������
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, texture);//λ��(��z��Ϊ0)����С������
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        s_Data->TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));//����������ɫ������ɫ
        texture->Bind();//������

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//ƽ�ƾ���*���ž���
        s_Data->TextureShader->SetMat4("u_Transform", transform);//����������ɫ���ı任����

        s_Data->QuadVertexArray->Bind();//�󶨶�������
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);//���ƶ�������
    }
}