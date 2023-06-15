#include "hzpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace Hazel {

    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        HZ_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::Init()
    {
        //这里本质上是绑定了一个用户自定义的指针到window，签名里是个void*，根据文档，这就是
        //一个用户自己爱干嘛干嘛的入口，glfw本身不会对这个指针做任何操作，我们可以把对应的
        //信息传进去
        glfwMakeContextCurrent(m_WindowHandle);
        //这里是用来初始化glad的，glad是一个OpenGL的函数指针管理库，用来管理OpenGL的函数指针
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        HZ_CORE_ASSERT(status, "Failed to initialize Glad!");
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}