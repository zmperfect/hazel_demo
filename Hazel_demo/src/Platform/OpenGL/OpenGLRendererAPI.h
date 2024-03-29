#pragma once

#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel {

    class OpenGLRendererAPI : public RendererAPI
    {
    public:
        virtual void Init() override;//初始化
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;//设置视口

        virtual void SetClearColor(const glm::vec4& color) override;//设置清除颜色
        virtual void Clear() override;//清除

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;//绘制顶点数组
        virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;//绘制线段

        virtual void SetLineWidth(float width) override;//设置线宽
    };
}