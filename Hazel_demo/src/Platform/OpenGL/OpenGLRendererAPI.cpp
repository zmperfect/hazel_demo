#include "hzpch.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Hazel {

    //简单的异常处理，用于glDebugMessageCallback()
    void OpenGLMessageCallback(
        unsigned source,
        unsigned type,
        unsigned id,
        unsigned severity,
        int length,
        const char* message,
        const void* userParam)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         HZ_CORE_CRITICAL(message); return;//严重错误
            case GL_DEBUG_SEVERITY_MEDIUM:       HZ_CORE_ERROR(message); return;//错误
            case GL_DEBUG_SEVERITY_LOW:          HZ_CORE_WARN(message); return;//警告
            case GL_DEBUG_SEVERITY_NOTIFICATION: HZ_CORE_TRACE(message); return;//通知
        }

        HZ_CORE_ASSERT(false, "Unknown severity level!");//未知错误
    }

    void OpenGLRendererAPI::Init()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        #ifdef HZ_DEBUG
            glEnable(GL_DEBUG_OUTPUT);//启用调试输出
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);//启用调试输出同步
            glDebugMessageCallback(OpenGLMessageCallback, nullptr);//调试消息回调函数

            //设置调试消息的输出方式
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        #endif

        glEnable(GL_BLEND);//允许混合
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//混合函数

        glEnable(GL_DEPTH_TEST);//允许深度测试
        glEnable(GL_LINE_SMOOTH);//允许线条平滑
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);//设置视口，x,y 以像素为单位，指定了视口的左下角位置。width，height 表示这个视口矩形的宽度和高度，根据窗口的实时变化重绘窗口。
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    //按照索引绘制
    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        vertexArray->Bind();//绑定顶点数组
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();//获取索引数量
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);//绘制三角形，顶点索引，索引类型，偏移量
    }

    // 画线
    void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
    {
        vertexArray->Bind();//绑定顶点数组
        glDrawArrays(GL_LINES, 0, vertexCount);//绘制线段，图元类型，起始索引，顶点数量
    }

    // 设置线宽
    void OpenGLRendererAPI::SetLineWidth(float width)
    {
        glLineWidth(width);
    }
}