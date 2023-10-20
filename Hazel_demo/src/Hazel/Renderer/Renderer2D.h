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
        static void Flush();//刷新

        // Primitives
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);//绘制矩形
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);//绘制矩形
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//绘制矩形，带纹理，带重复因子，带颜色色调
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//绘制矩形，带纹理，带重复因子，带颜色色调

        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
        static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//变换矩阵，纹理，重复因子，颜色色调

        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);//绘制旋转矩形
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);//绘制旋转矩形
        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//绘制旋转矩形，带纹理，带重复因子，带颜色色调
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));//绘制旋转矩形，带纹理，带重复因子，带颜色色调

        // Stats
        struct Statistics
        {
            uint32_t DrawCalls = 0;//绘制调用次数
            uint32_t QuadCount = 0;//矩形数量

            uint32_t GetTotalVertexCount() { return QuadCount * 4; }//顶点数量
            uint32_t GetTotalIndexCount() { return QuadCount * 6; }//索引数量
        };
        static void ResetStats();//重置统计
        static Statistics GetStats();//获取统计
    private:
        static void FlushAndReset();//刷新并重置
    };
}