#include "hzpch.h"
#include "Hazel/Renderer/Renderer2D.h"

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/UniformBuffer.h"
#include "Hazel/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel {

	//方形顶点
	struct QuadVertex
	{
		glm::vec3 Position;//位置
		glm::vec4 Color;//颜色
		glm::vec2 TexCoord;//纹理坐标
		float TexIndex;//纹理索引
		float TilingFactor;//平铺因子

		// Editor-only
		int EntityID;//实体ID
	};

	//圆形顶点
	struct CircleVertex 
	{
        glm::vec3 WorldPosition;//世界位置
        glm::vec3 LocalPosition;//本地位置
        glm::vec4 Color;//颜色
        float Thickness;//厚度
        float Fade;//淡化

        // Editor-only
        int EntityID;//实体ID
	};

	// 线段顶点
    struct LineVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;

        // Editor-only
        int EntityID;
    };

	//渲染数据
	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;//最大方形数量
		static const uint32_t MaxVertices = MaxQuads * 4;//最大顶点数量
		static const uint32_t MaxIndices = MaxQuads * 6;//最大索引数量
		static const uint32_t MaxTextureSlots = 32; // 最大纹理槽数 TODO: RenderCaps

		// Quad
		Ref<VertexArray> QuadVertexArray;//方形顶点数组
		Ref<VertexBuffer> QuadVertexBuffer;//方形顶点缓冲区
		Ref<Shader> QuadShader;//方形着色器
		Ref<Texture2D> WhiteTexture;//白色纹理

        uint32_t QuadIndexCount = 0;//方形索引数量
        QuadVertex* QuadVertexBufferBase = nullptr;//方形顶点缓冲区基地址
        QuadVertex* QuadVertexBufferPtr = nullptr;//方形顶点缓冲区指针

		// Circle
        Ref<VertexArray> CircleVertexArray;
        Ref<VertexBuffer> CircleVertexBuffer;
        Ref<Shader> CircleShader;

        uint32_t CircleIndexCount = 0;
        CircleVertex* CircleVertexBufferBase = nullptr;
        CircleVertex* CircleVertexBufferPtr = nullptr;

		// Line
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;// 线宽

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;//纹理槽
		uint32_t TextureSlotIndex = 1; // 纹理槽索引，0 = white texture

		glm::vec4 QuadVertexPositions[4];//方形顶点位置

		Renderer2D::Statistics Stats;//统计

		struct CameraData
		{
			glm::mat4 ViewProjection;//视图投影矩阵
		};
		CameraData CameraBuffer;//相机缓冲区
		Ref<UniformBuffer> CameraUniformBuffer;//相机统一缓冲区
	};

	static Renderer2DData s_Data;//渲染器2D数据

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();//性能分析

		s_Data.QuadVertexArray = VertexArray::Create();//创建方形顶点数组

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));//创建方形顶点缓冲区
		s_Data.QuadVertexBuffer->SetLayout({//设置方形顶点缓冲区布局
            { ShaderDataType::Float3, "a_Position"		},
            { ShaderDataType::Float4, "a_Color"			},
            { ShaderDataType::Float2, "a_TexCoord"    },
            { ShaderDataType::Float,  "a_TexIndex"      },
            { ShaderDataType::Float,  "a_TilingFactor"  },
            { ShaderDataType::Int,    "a_EntityID"			}
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

        // Circles
        s_Data.CircleVertexArray = VertexArray::Create();

        s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
        s_Data.CircleVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_WorldPosition" },
            { ShaderDataType::Float3, "a_LocalPosition" },
            { ShaderDataType::Float4, "a_Color"         },
            { ShaderDataType::Float,  "a_Thickness"     },
            { ShaderDataType::Float,  "a_Fade"          },
            { ShaderDataType::Int,    "a_EntityID"      }
            });
        s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
        s_Data.CircleVertexArray->SetIndexBuffer(quadIB); // Use quad IB
        s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

        // Lines
        s_Data.LineVertexArray = VertexArray::Create();

        s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
        s_Data.LineVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color"    },
            { ShaderDataType::Int,    "a_EntityID" }
            });
        s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
        s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

		s_Data.WhiteTexture = Texture2D::Create(1, 1);//创建纹理
		uint32_t whiteTextureData = 0xffffffff;//白色纹理数据
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));//设置纹理数据

		int32_t samplers[s_Data.MaxTextureSlots];//纹理槽
		// 设置纹理槽
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		// 创建着色器
		s_Data.QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");//创建方形着色器
		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");//创建圆形着色器
		s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");//创建线着色器

		// Set first texture slot to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };//左下角
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };//右下角
		s_Data.QuadVertexPositions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };//右上角
		s_Data.QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };//左上角

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);//创建相机统一缓冲区
	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();//设置相机缓冲区视图投影矩阵
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));//设置相机统一缓冲区数据

		StartBatch();//开始批处理
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);//设置相机缓冲区视图投影矩阵
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));//设置相机统一缓冲区数据

		StartBatch();//开始批处理
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		HZ_PROFILE_FUNCTION();//性能分析
		
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();//设置相机缓冲区视图投影矩阵
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));//设置相机统一缓冲区数据

		StartBatch();//开始批处理
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		Flush();//刷新
	}

	// 开始批处理
	void Renderer2D::StartBatch()
	{
		// 方形
        s_Data.QuadIndexCount = 0;//方形索引数量
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;//方形顶点缓冲区指针

		// 圆形
		s_Data.CircleIndexCount = 0;//圆形索引数量
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;//圆形顶点缓冲区指针

		// 线
        s_Data.LineVertexCount = 0;//线顶点数量
        s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;//线顶点缓冲区指针

        s_Data.TextureSlotIndex = 1;//纹理槽索引
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount)// 如果方形索引数量大于0
		{
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);//数据大小
            s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);//设置方形顶点缓冲区数据

            // 绑定纹理
            for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
                s_Data.TextureSlots[i]->Bind(i);

            s_Data.QuadShader->Bind();//绑定纹理着色器
            RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);//按索引绘制
            s_Data.Stats.DrawCalls++;//绘制调用次数++
		}
        
        if (s_Data.CircleIndexCount)// 如果圆形索引数量大于0
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);//数据大小
            s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);//设置圆形顶点缓冲区数据

            s_Data.CircleShader->Bind();//绑定圆形着色器
            RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);//按索引绘制
            s_Data.Stats.DrawCalls++;//绘制调用次数++
        }

		if (s_Data.LineVertexCount)// 如果线顶点数量大于0
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
            s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

            s_Data.LineShader->Bind();
            RenderCommand::SetLineWidth(s_Data.LineWidth);
            RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
            s_Data.Stats.DrawCalls++;
        }

	}

	void Renderer2D::NextBatch()
	{
		Flush();//刷新
		StartBatch();//开始批处理
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

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		HZ_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;//方形顶点数量
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };//纹理坐标
		const float tilingFactor = 1.0f;//平铺因子

		//如果方形索引数量大于最大方形索引数量,则刷新并重置
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            NextBatch();

		//设置方形顶点缓冲区
		for (size_t i = 0; i < quadVertexCount; i++)
		{
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];//位置
            s_Data.QuadVertexBufferPtr->Color = color;//颜色
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];//纹理坐标
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;//纹理索引
            s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;//平铺因子
			s_Data.QuadVertexBufferPtr->EntityID = entityID;//实体ID
            s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;//方形数量++
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		HZ_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;//方形顶点数量
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } }; //纹理坐标

		//如果方形索引数量大于最大方形索引数量,则刷新并重置
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            NextBatch();

		float textureIndex = 0.0f;//纹理索引
		
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)//遍历纹理槽
		{
			if (*s_Data.TextureSlots[i]== *texture)//如果纹理槽中的纹理与传入的纹理相同
			{
				textureIndex = (float)i;//纹理索引为i
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
            if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
                NextBatch();

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
			s_Data.QuadVertexBufferPtr->EntityID = entityID;//实体ID
            s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;//方形数量++
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);//位置(将z设为0)，大小，旋转角度，颜色
	}

	//旋转的方形
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		//变换矩阵
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//转换矩阵
		
		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		//变换矩阵
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/, int entityID /*= -1*/)
    {
        HZ_PROFILE_FUNCTION();

        // TODO: 为圆实现批处理
        // if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
        // 	NextBatch();

        for (size_t i = 0; i < 4; i++)
        {
            s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
            s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
            s_Data.CircleVertexBufferPtr->Color = color;
            s_Data.CircleVertexBufferPtr->Thickness = thickness;
            s_Data.CircleVertexBufferPtr->Fade = fade;
            s_Data.CircleVertexBufferPtr->EntityID = entityID;
            s_Data.CircleVertexBufferPtr++;
        }

        s_Data.CircleIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
    {
		// 线段起始位置
        s_Data.LineVertexBufferPtr->Position = p0;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

		// 线段结束位置
        s_Data.LineVertexBufferPtr->Position = p1;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVertexCount += 2;
    }

	// 绘制矩形
    void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
    {
		// 根据矩形中心计算矩形的四个顶点
        glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
        glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
        glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
        glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		// 绘制矩形的四条边
        DrawLine(p0, p1, color);
        DrawLine(p1, p2, color);
        DrawLine(p2, p3, color);
        DrawLine(p3, p0, color);
    }

	// 绘制矩形
    void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
		// 根据变换矩阵计算矩形的四个顶点
        glm::vec3 lineVertices[4];// 矩形的四个顶点vertex
        for (size_t i = 0; i < 4; i++)
            lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

		// 绘制矩形的四条边
        DrawLine(lineVertices[0], lineVertices[1], color);
        DrawLine(lineVertices[1], lineVertices[2], color);
        DrawLine(lineVertices[2], lineVertices[3], color);
        DrawLine(lineVertices[3], lineVertices[0], color);
    }

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}

	// 获取线宽
    float Renderer2D::GetLineWidth()
    {
        return s_Data.LineWidth;
    }

	// 设置线宽
    void Renderer2D::SetLineWidth(float width)
    {
        s_Data.LineWidth = width;
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