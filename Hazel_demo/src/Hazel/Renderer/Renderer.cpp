#include "hzpch.h"
#include "Hazel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Hazel/Renderer/Renderer2D.h"

namespace Hazel {

    Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();//����һ���������ݵ�������

    void Renderer::Init()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        RenderCommand::Init();//��ʼ����Ⱦ����
        Renderer2D::Init();//��ʼ��2D��Ⱦ��
    }

    void Renderer::Shutdown()
    {
        Renderer2D::Shutdown();//�ر�2D��Ⱦ��
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);//�����ӿڣ�x,y ������Ϊ��λ��ָ�����ӿڵ����½�λ�á�width��height ��ʾ����ӿھ��εĿ�Ⱥ͸߶ȣ����ݴ��ڵ�ʵʱ�仯�ػ洰�ڡ�
    }

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();//��ȡ�ӽ�ͶӰ����
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
    {
        shader->Bind();//����ɫ��
        shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);//�ϴ��ӽ�ͶӰ����
        shader->SetMat4("u_Transform", transform);//�ϴ��任����

        vertexArray->Bind();//�󶨶�������
        RenderCommand::DrawIndexed(vertexArray);//���ƶ�������
    }
}
