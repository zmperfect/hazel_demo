#include "hzpch.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Hazel {

    //�򵥵��쳣��������glDebugMessageCallback()
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
            case GL_DEBUG_SEVERITY_HIGH:         HZ_CORE_CRITICAL(message); return;//���ش���
            case GL_DEBUG_SEVERITY_MEDIUM:       HZ_CORE_ERROR(message); return;//����
            case GL_DEBUG_SEVERITY_LOW:          HZ_CORE_WARN(message); return;//����
            case GL_DEBUG_SEVERITY_NOTIFICATION: HZ_CORE_TRACE(message); return;//֪ͨ
        }

        HZ_CORE_ASSERT(false, "Unknown severity level!");//δ֪����
    }

    void OpenGLRendererAPI::Init()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        #ifdef HZ_DEBUG
            glEnable(GL_DEBUG_OUTPUT);//���õ������
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);//���õ������ͬ��
            glDebugMessageCallback(OpenGLMessageCallback, nullptr);//������Ϣ�ص�����

            //���õ�����Ϣ�������ʽ
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        #endif

        glEnable(GL_BLEND);//������
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//��Ϻ���

        glEnable(GL_DEPTH_TEST);//������Ȳ���
        glEnable(GL_LINE_SMOOTH);//��������ƽ��
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);//�����ӿڣ�x,y ������Ϊ��λ��ָ�����ӿڵ����½�λ�á�width��height ��ʾ����ӿھ��εĿ�Ⱥ͸߶ȣ����ݴ��ڵ�ʵʱ�仯�ػ洰�ڡ�
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    //������������
    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        vertexArray->Bind();//�󶨶�������
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();//��ȡ��������
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);//���������Σ������������������ͣ�ƫ����
    }

    // ����
    void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
    {
        vertexArray->Bind();//�󶨶�������
        glDrawArrays(GL_LINES, 0, vertexCount);//�����߶Σ�ͼԪ���ͣ���ʼ��������������
    }

    // �����߿�
    void OpenGLRendererAPI::SetLineWidth(float width)
    {
        glLineWidth(width);
    }
}