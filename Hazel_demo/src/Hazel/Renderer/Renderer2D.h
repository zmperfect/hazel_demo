#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Renderer/Texture.h"

namespace Hazel {

    class Renderer2D
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();
        static void Flush();//ˢ��

        // Primitives
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);//���ƾ���
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);//���ƾ���
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//���ƾ��Σ����������ظ����ӣ�����ɫɫ��
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//���ƾ��Σ����������ظ����ӣ�����ɫɫ��

        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
        static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//�任���������ظ����ӣ���ɫɫ��

        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);//������ת����
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);//������ת����
        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//������ת���Σ����������ظ����ӣ�����ɫɫ��
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//������ת���Σ����������ظ����ӣ�����ɫɫ��

        // Stats
        struct Statistics
        {
            uint32_t DrawCalls = 0;//���Ƶ��ô���
            uint32_t QuadCount = 0;//��������

            uint32_t GetTotalVertexCount() { return QuadCount * 4; }//��������
            uint32_t GetTotalIndexCount() { return QuadCount * 6; }//��������
        };
        static void ResetStats();//����ͳ��
        static Statistics GetStats();//��ȡͳ��
    private:
        static void FlushAndReset();//ˢ�²�����
    };
}