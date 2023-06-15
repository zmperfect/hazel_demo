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
        //���ﱾ�����ǰ���һ���û��Զ����ָ�뵽window��ǩ�����Ǹ�void*�������ĵ��������
        //һ���û��Լ�������������ڣ�glfw����������ָ�����κβ��������ǿ��԰Ѷ�Ӧ��
        //��Ϣ����ȥ
        glfwMakeContextCurrent(m_WindowHandle);
        //������������ʼ��glad�ģ�glad��һ��OpenGL�ĺ���ָ�����⣬��������OpenGL�ĺ���ָ��
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        HZ_CORE_ASSERT(status, "Failed to initialize Glad!");
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}