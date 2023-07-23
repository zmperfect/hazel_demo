#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

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
        virtual void SetClearColor(const glm::vec4& color) = 0;//设置清除颜色
        virtual void Clear() = 0;//清除

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;//绘制顶点数组

        inline static API GetAPI() { return s_API; }//获取API
    private:
        static API s_API;//API
    };
}