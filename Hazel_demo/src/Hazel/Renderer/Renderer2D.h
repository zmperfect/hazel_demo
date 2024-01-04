#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Renderer/Texture.h"

#include "Hazel/Renderer/Camera.h"
#include "Hazel/Renderer/EditorCamera.h"

namespace Hazel {

    class Renderer2D
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void BeginScene(const EditorCamera& camera);//根据编辑相机开始场景
        static void BeginScene(const OrthographicCamera& camera);// TODO: 移除
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

            uint32_t GetTotalVertexCount() const { return QuadCount * 4; }//顶点数量.这个函数是一个常量成员函数，这意味着它不能修改它所属的对象的任何数据成员。这是一种很好的实践，因为它明确地表明了这个函数不会改变对象的状态。
            uint32_t GetTotalIndexCount() const { return QuadCount * 6; }//索引数量
        };
        static void ResetStats();//重置统计
        static Statistics GetStats();//获取统计
    private:
        static void StartBatch();//开始批处理
        static void NextBatch();//下一批处理
    };
}