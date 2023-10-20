#include "hzpch.h"
#include "Hazel/Renderer/Renderer2D.h"

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

	//���ζ���
	struct QuadVertex
	{
		glm::vec3 Position;//λ��
		glm::vec4 Color;//��ɫ
		glm::vec2 TexCoord;//��������
		float TexIndex;//��������
		float TilingFactor;//ƽ������
	};

	//��Ⱦ����
	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;//���������
		static const uint32_t MaxVertices = MaxQuads * 4;//��󶥵�����
		static const uint32_t MaxIndices = MaxQuads * 6;//�����������
		static const uint32_t MaxTextureSlots = 32; // ���������� TODO: RenderCaps

		Ref<VertexArray> QuadVertexArray;//���ζ�������
		Ref<VertexBuffer> QuadVertexBuffer;//���ζ��㻺����
		Ref<Shader> TextureShader;//������ɫ��
		Ref<Texture2D> WhiteTexture;//��ɫ����

		uint32_t QuadIndexCount = 0;//������������
		QuadVertex* QuadVertexBufferBase = nullptr;//���ζ��㻺��������ַ
		QuadVertex* QuadVertexBufferPtr = nullptr;//���ζ��㻺����ָ��

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;//�����
		uint32_t TextureSlotIndex = 1; // �����������0 = white texture

		glm::vec4 QuadVertexPositions[4];//���ζ���λ��

		Renderer2D::Statistics Stats;//ͳ��
	};

	static Renderer2DData s_Data;//��Ⱦ��2D����

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();//���ܷ���

		s_Data.QuadVertexArray = VertexArray::Create();//�������ζ�������

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));//�������ζ��㻺����
		s_Data.QuadVertexBuffer->SetLayout({//���÷��ζ��㻺��������
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" }
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);//��ӷ��ζ��㻺����

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];//�������ζ��㻺��������ַ

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];//������������

		uint32_t offset = 0;//ƫ����
		//���÷�������
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

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);//������������������
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);//���÷��ζ�����������������
		delete[] quadIndices;//ɾ����������

		s_Data.WhiteTexture = Texture2D::Create(1, 1);//��������
		uint32_t whiteTextureData = 0xffffffff;//��ɫ��������
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));//������������

		int32_t samplers[s_Data.MaxTextureSlots];//�����
		//���������
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");//����������ɫ��
		s_Data.TextureShader->Bind();//��������ɫ��
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);//���������

		// Set all texture slots to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };//���½�
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };//���½�
		s_Data.QuadVertexPositions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };//���Ͻ�
		s_Data.QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };//���Ͻ�
	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();//��������ɫ��
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());//������ͼͶӰ����

		s_Data.QuadIndexCount = 0;//������������
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;//���ζ��㻺����ָ��

		s_Data.TextureSlotIndex = 1;//���������
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);//���ݴ�С
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);//���÷��ζ��㻺��������

		Flush();//ˢ��
	}

	void Renderer2D::Flush()
	{
		if(s_Data.QuadIndexCount == 0)//������������Ϊ0
            return;//ûɶҪ����

		// ������
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);//����������
		s_Data.Stats.DrawCalls++;//���Ƶ��ô���++
	}

	void Renderer2D::FlushAndReset()
	{
        EndScene();//��������

        s_Data.QuadIndexCount = 0;//������������
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;//���ζ��㻺����ָ��

        s_Data.TextureSlotIndex = 1;//���������
    }

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);//λ��(��z��Ϊ0)����С����ɫ
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		//�任����
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)//ƽ��
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//����

		DrawQuad(transform, color);//�任������ɫ
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		//�任����
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;//���ζ�������
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };//��������
		const float tilingFactor = 1.0f;//ƽ������

		//��������������������������������,��ˢ�²�����
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

		//���÷��ζ��㻺����
		for (size_t i = 0; i < quadVertexCount; i++)
		{
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];//λ��
            s_Data.QuadVertexBufferPtr->Color = color;//��ɫ
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];//��������
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;//��������
            s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;//ƽ������
            s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;//��������++
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);//λ��(��z��Ϊ0)����С����ת�Ƕȣ���ɫ
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;//���ζ�������
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } }; //��������

		//��������������������������������,��ˢ�²�����
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

		float textureIndex = 0.0f;//��������
		
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)//���������
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())//���������е������봫���������ͬ
			{
				textureIndex = (float)i;//��������Ϊi
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
            if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
                FlushAndReset();

			textureIndex = (float)s_Data.TextureSlotIndex;//������������
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;//��������������
			s_Data.TextureSlotIndex++;
		}

		//���÷��ζ��㻺����
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];//λ��
            s_Data.QuadVertexBufferPtr->Color = tintColor;//��ɫ
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];//��������
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;//��������
            s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;//ƽ������
            s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;//��������++
	}

	//��ת�ķ���
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;//���ζ�������
		const float textureIndex = 0.0f;//��������
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };//��������
		const float tilingFactor = 1.0f;//ƽ������

		//��������������������������������,��ˢ�²�����
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();


		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//ת������
		
		//���÷��ζ��㻺����
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

		s_Data.Stats.QuadCount++;//��������++
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;//���ζ�������
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };//��������

		//��������������������������������,��ˢ�²�����
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

		float textureIndex = 0.0f;//��������
		for(uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)//���������
        {
            if(*s_Data.TextureSlots[i] == *texture)//���������е������봫���������ͬ
            {
                textureIndex = (float)i;//��������Ϊ����۵�����
                break;
            }
        }

		if (textureIndex == 0.0f)
		{
            if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
                FlushAndReset();

			textureIndex = (float)s_Data.TextureSlotIndex;//��������Ϊ����۵�����
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;//������е�����Ϊ���������
            s_Data.TextureSlotIndex++;//�����������һ
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		//���÷��ζ��㻺����
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

		s_Data.Stats.QuadCount++;//��������++
	}

	void Renderer2D::ResetStats()
	{
        memset(&s_Data.Stats, 0, sizeof(Statistics));//��ͳ����������
    }

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

}