#include "hzpch.h"
#include "Hazel/Renderer/Renderer2D.h"

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

    struct QuadVertex
    {
        glm::vec3 Position;//位置
        glm::vec4 Color;//颜色
        glm::vec2 TexCoord;//纹理坐标
        // TODO: TexID
    };

    struct Renderer2DData
    {
        const uint32_t MaxQuads = 10000;//最大方形数量
        const uint32_t MaxVertices = MaxQuads * 4;//最大顶点数量
        const uint32_t MaxIndices = MaxQuads * 6;//最大索引数量

        Ref<VertexArray> QuadVertexArray;//顶点数组
        Ref<VertexBuffer> QuadVertexBuffer;//顶点缓冲区
        Ref<Shader> TextureShader;//纹理着色器
        Ref<Texture2D> WhiteTexture;//白色纹理

        uint32_t QuadIndexCount = 0;//方形索引数量
        QuadVertex* QuadVertexBufferBase = nullptr;//方形顶点缓冲区基地址
        QuadVertex* QuadVertexBufferPtr = nullptr;//方形顶点缓冲区指针
    };

    static Renderer2DData s_Data;//渲染器2D数据

    void Renderer2D::Init()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        s_Data.QuadVertexArray = VertexArray::Create();//创建顶点数组

        s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));//创建顶点缓冲区
        s_Data.QuadVertexBuffer->SetLayout({//设置顶点缓冲区的布局,位置和纹理
                   { ShaderDataType::Float3, "a_Position" },
                   { ShaderDataType::Float4, "a_Color" },
                   { ShaderDataType::Float2, "a_TexCoord" }
            });

        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);//添加顶点缓冲区

        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];//创建方形顶点缓冲区基地址

        uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];//创建方形索引缓冲区

        uint32_t offset = 0;//偏移量
        //设置方形索引缓冲区
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;//索引
            quadIndices[i + 1] = offset + 1;//索引
            quadIndices[i + 2] = offset + 2;//索引

            quadIndices[i + 3] = offset + 2;//索引
            quadIndices[i + 4] = offset + 3;//索引
            quadIndices[i + 5] = offset + 0;//索引

            offset += 4;//偏移量
        }

        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);//创建索引缓冲区
        s_Data.QuadVertexArray->SetIndexBuffer(quadIB);//设置顶点数组的索引缓冲区
        delete[] quadIndices;//删除方形索引缓冲区

        s_Data.WhiteTexture = Texture2D::Create(1, 1);//创建纹理
        uint32_t whiteTextureData = 0xffffffff;//纹理数据
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));//设置纹理数据

        s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");//创建纹理着色器
        s_Data.TextureShader->Bind();//绑定纹理着色器
        s_Data.TextureShader->SetInt("u_Texture", 0);//设置纹理着色器的纹理
    }

    void Renderer2D::Shutdown()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        s_Data.TextureShader->Bind();//绑定纹理着色器
        s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());//设置纹理着色器的视图投影矩阵

        s_Data.QuadIndexCount = 0;//方形索引数量
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;//方形顶点缓冲区指针
    }

    void Renderer2D::EndScene()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;//数据大小
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);//设置顶点缓冲区的数据

        Flush();//刷新
    }

    void Renderer2D::Flush()
    {
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);//绘制方形顶点数组
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, color);//位置(将z设为0)，大小，颜色
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        //设置方形顶点缓冲区
        s_Data.QuadVertexBufferPtr->Position = position;//设置方形顶点缓冲区的位置
        s_Data.QuadVertexBufferPtr->Color = color;//设置方形顶点缓冲区的颜色
        s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };//设置方形顶点缓冲区的纹理坐标
        s_Data.QuadVertexBufferPtr++;//方形顶点缓冲区指针自增

        s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
        s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = { position.x, position.y + size.y, 0.0f };
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadIndexCount += 6;

        /*s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
        s_Data.WhiteTexture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//平移矩阵*缩放矩阵

        s_Data.TextureShader->SetMat4("u_Transform", transform);//设置纹理着色器的变换矩阵
        s_Data.QuadVertexArray->Bind();//绑定顶点数组
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);//绘制顶点数组*/
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);//位置(将z设为0)，大小，纹理，平铺因子，颜色
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        s_Data.TextureShader->SetFloat4("u_Color", tintColor);//设置纹理着色器的颜色
        s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);//设置纹理着色器的平铺因子
        texture->Bind();//绑定纹理

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//平移矩阵*缩放矩阵
        s_Data.TextureShader->SetMat4("u_Transform", transform);//设置纹理着色器的变换矩阵

        s_Data.QuadVertexArray->Bind();//绑定顶点数组
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);//绘制顶点数组
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);//位置(将z设为0)，大小，旋转角度，颜色
    }

    //旋转的方形
    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.TextureShader->SetFloat4("u_Color", color);
        s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
        s_Data.WhiteTexture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//转换矩阵
        s_Data.TextureShader->SetMat4("u_Transform", transform);

        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.TextureShader->SetFloat4("u_Color", tintColor);
        s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);
        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        s_Data.TextureShader->SetMat4("u_Transform", transform);

        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
    }
}