#pragma once

#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel {

    class OpenGLRendererAPI : public RendererAPI
    {
    public:
        virtual void Init() override;//初始化

        virtual void SetClearColor(const glm::vec4& color) override;//设置清除颜色
        virtual void Clear() override;//清除

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;//绘制顶点数组
    };
}