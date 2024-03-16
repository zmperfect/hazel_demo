#pragma once

#include"Hazel/Renderer/RendererAPI.h"

namespace Hazel {

    class RenderCommand
    {
    public:
        static void Init()
        {
            s_RendererAPI->Init();//初始化渲染器API
        }

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            s_RendererAPI->SetViewport(x, y, width, height);//设置视口
        }

        static void SetClearColor(const glm::vec4& color)
        {
            s_RendererAPI->SetClearColor(color);
        }

        static void Clear()
        {
            s_RendererAPI->Clear();
        }

        // 按索引绘制
        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
        {
            s_RendererAPI->DrawIndexed(vertexArray, indexcount);
        }

        // 绘线
        static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
        {
            s_RendererAPI->DrawLines(vertexArray, vertexCount);
        }

        // 设置线宽
        static void SetLineWidth(float width)
        {
            s_RendererAPI->SetLineWidth(width);
        }
    private:
        static Scope<RendererAPI> s_RendererAPI;
    };
}