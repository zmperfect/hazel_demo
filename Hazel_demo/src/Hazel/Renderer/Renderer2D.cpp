#include "hzpch.h"
#include "Hazel/Renderer/Renderer2D.h"

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

	//方形顶点
	struct QuadVertex
	{
		glm::vec3 Position;//位置
		glm::vec4 Color;//颜色
		glm::vec2 TexCoord;//纹理坐标
		float TexIndex;//纹理索引
		float TilingFactor;//平铺因子
	};

	//渲染数据
	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;//最大方形数量
		static const uint32_t MaxVertices = MaxQuads * 4;//最大顶点数量
		static const uint32_t MaxIndices = MaxQuads * 6;//最大索引数量
		static const uint32_t MaxTextureSlots = 32; // 最大纹理槽数 TODO: RenderCaps

		Ref<VertexArray> QuadVertexArray;//方形顶点数组
		Ref<VertexBuffer> QuadVertexBuffer;//方形顶点缓冲区
		Ref<Shader> TextureShader;//纹理着色器
		Ref<Texture2D> WhiteTexture;//白色纹理

		uint32_t QuadIndexCount = 0;//方形索引数量
		QuadVertex* QuadVertexBufferBase = nullptr;//方形顶点缓冲区基地址
		QuadVertex* QuadVertexBufferPtr = nullptr;//方形顶点缓冲区指针

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;//纹理槽
		uint32_t TextureSlotIndex = 1; // 纹理槽索引，0 = white texture

		glm::vec4 QuadVertexPositions[4];//方形顶点位置

		Renderer2D::Statistics Stats;//统计
	};

	static Renderer2DData s_Data;//渲染器2D数据

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();//性能分析

		s_Data.QuadVertexArray = VertexArray::Create();//创建方形顶点数组

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));//创建方形顶点缓冲区
		s_Data.QuadVertexBuffer->SetLayout({//设置方形顶点缓冲区布局
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" }
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);//添加方形顶点缓冲区

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];//创建方形顶点缓冲区基地址

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];//创建方形索引

		uint32_t offset = 0;//偏移量
		//设置方形索引
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);//创建方形索引缓冲区
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);//设置方形顶点数组索引缓冲区
		delete[] quadIndices;//删除方形索引

		s_Data.WhiteTexture = Texture2D::Create(1, 1);//创建纹理
		uint32_t whiteTextureData = 0xffffffff;//白色纹理数据
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));//设置纹理数据

		int32_t samplers[s_Data.MaxTextureSlots];//纹理槽
		//设置纹理槽
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");//创建纹理着色器
		s_Data.TextureShader->Bind();//绑定纹理着色器
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);//设置纹理槽

		// Set all texture slots to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };//左下角
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };//右下角
		s_Data.QuadVertexPositions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };//右上角
		s_Data.QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };//左上角
	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();//绑定纹理着色器
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());//设置视图投影矩阵

		s_Data.QuadIndexCount = 0;//方形索引数量
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;//方形顶点缓冲区指针

		s_Data.TextureSlotIndex = 1;//纹理槽索引
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);//数据大小
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);//设置方形顶点缓冲区数据

		Flush();//刷新
	}

	void Renderer2D::Flush()
	{
		if(s_Data.QuadIndexCount == 0)//方形索引数量为0
            return;//没啥要画的

		// 绑定纹理
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);//按索引绘制
		s_Data.Stats.DrawCalls++;//绘制调用次数++
	}

	void Renderer2D::FlushAndReset()
	{
        EndScene();//结束场景

        s_Data.QuadIndexCount = 0;//方形索引数量
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;//方形顶点缓冲区指针

        s_Data.TextureSlotIndex = 1;//纹理槽索引
    }

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);//位置(将z设为0)，大小，颜色
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		//变换矩阵
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)//平移
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//缩放

		DrawQuad(transform, color);//变换矩阵，颜色
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		//变换矩阵
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;//方形顶点数量
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };//纹理坐标
		const float tilingFactor = 1.0f;//平铺因子

		//如果方形索引数量大于最大方形索引数量,则刷新并重置
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

		//设置方形顶点缓冲区
		for (size_t i = 0; i < quadVertexCount; i++)
		{
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];//位置
            s_Data.QuadVertexBufferPtr->Color = color;//颜色
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];//纹理坐标
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;//纹理索引
            s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;//平铺因子
            s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;//方形数量++
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);//位置(将z设为0)，大小，旋转角度，颜色
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;//方形顶点数量
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } }; //纹理坐标

		//如果方形索引数量大于最大方形索引数量,则刷新并重置
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

		float textureIndex = 0.0f;//纹理索引
		
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)//遍历纹理槽
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())//如果纹理槽中的纹理与传入的纹理相同
			{
				textureIndex = (float)i;//纹理索引为i
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
            if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
                FlushAndReset();

			textureIndex = (float)s_Data.TextureSlotIndex;//设置纹理索引
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;//将纹理放入纹理槽
			s_Data.TextureSlotIndex++;
		}

		//设置方形顶点缓冲区
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];//位置
            s_Data.QuadVertexBufferPtr->Color = tintColor;//颜色
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];//纹理坐标
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;//纹理索引
            s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;//平铺因子
            s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;//方形数量++
	}

	//旋转的方形
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;//方形顶点数量
		const float textureIndex = 0.0f;//纹理索引
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };//纹理坐标
		const float tilingFactor = 1.0f;//平铺因子

		//如果方形索引数量大于最大方形索引数量,则刷新并重置
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();


		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//转换矩阵
		
		//设置方形顶点缓冲区
		for (size_t i = 0; i < quadVertexCount; i++)
		{
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color = color;
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
            s_Data.QuadVertexBufferPtr++;
		}

        s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;//方形数量++
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;//方形顶点数量
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };//纹理坐标

		//如果方形索引数量大于最大方形索引数量,则刷新并重置
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

		float textureIndex = 0.0f;//纹理索引
		for(uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)//遍历纹理槽
        {
            if(*s_Data.TextureSlots[i] == *texture)//如果纹理槽中的纹理与传入的纹理相同
            {
                textureIndex = (float)i;//纹理索引为纹理槽的索引
                break;
            }
        }

		if (textureIndex == 0.0f)
		{
            if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
                FlushAndReset();

			textureIndex = (float)s_Data.TextureSlotIndex;//纹理索引为纹理槽的索引
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;//纹理槽中的纹理为传入的纹理
            s_Data.TextureSlotIndex++;//纹理槽索引加一
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		//设置方形顶点缓冲区
		for (size_t i = 0; i < quadVertexCount; i++)
		{
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color = tintColor;
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
            s_Data.QuadVertexBufferPtr++;
		}

        s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;//方形数量++
	}

	void Renderer2D::ResetStats()
	{
        memset(&s_Data.Stats, 0, sizeof(Statistics));//将统计数据清零
    }

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

}