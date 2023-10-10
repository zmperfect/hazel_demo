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
		const uint32_t MaxQuads = 10000;//���������
		const uint32_t MaxVertices = MaxQuads * 4;//��󶥵�����
		const uint32_t MaxIndices = MaxQuads * 6;//�����������
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

		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;//���ݴ�С
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);//���÷��ζ��㻺��������

		Flush();//ˢ��
	}

	void Renderer2D::Flush()
	{
		// ������
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);//����������
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);//λ��(��z��Ϊ0)����С����ɫ
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		const float texIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;//ƽ������

		//���÷��ζ��㻺����
		s_Data.QuadVertexBufferPtr->Position = position;//λ��
		s_Data.QuadVertexBufferPtr->Color = color;//��ɫ
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };//��������
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;//��������
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;//ƽ������
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x, position.y + size.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		/*s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_Data.WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//ƽ�ƾ���*���ž���
		s_Data.TextureShader->SetMat4("u_Transform", transform);//����������ɫ���ı任����
		s_Data.QuadVertexArray->Bind();//�󶨶�������
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);//���ƶ�������*/
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);//λ��(��z��Ϊ0)����С������ƽ�����ӣ���ɫ
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };//����һ����̬����ɫ

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
			textureIndex = (float)s_Data.TextureSlotIndex;//������������
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;//��������������
			s_Data.TextureSlotIndex++;
		}

		//���÷��ζ��㻺����
		s_Data.QuadVertexBufferPtr->Position = position;
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x, position.y + size.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		#if OLD_PATH
		s_Data.TextureShader->SetFloat4("u_Color", tintColor);//����������ɫ������ɫ
		s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);//����������ɫ����ƽ������
		texture->Bind();//������

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//ƽ�ƾ���*���ž���
		s_Data.TextureShader->SetMat4("u_Transform", transform);//����������ɫ���ı任����

		s_Data.QuadVertexArray->Bind();//�󶨶�������
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);//���ƶ�������
		#endif
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);//λ��(��z��Ϊ0)����С����ת�Ƕȣ���ɫ
	}

	//��ת�ķ���
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_Data.WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });//ת������
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