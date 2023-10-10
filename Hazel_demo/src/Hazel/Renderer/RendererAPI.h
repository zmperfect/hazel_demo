#pragma once

#include <glm/glm.hpp>

#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {

    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0, OpenGL = 1
        };
    public:
        virtual void Init() = 0;//初始化
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;//设置视口
        virtual void SetClearColor(const glm::vec4& color) = 0;//设置清除颜色
        virtual void Clear() = 0;//清除

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0) = 0;//绘制顶点数组

        static API GetAPI() { return s_API; }//获取API
        static Scope<RendererAPI> Create();//创建

    private:
        static API s_API;//API
    };
}