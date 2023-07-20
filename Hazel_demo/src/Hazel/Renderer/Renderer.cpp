#include "hzpch.h"
#include "Renderer.h"

namespace Hazel {

    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;//����һ���������ݵ�������

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();//��ȡ�ӽ�ͶӰ����
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
    {
        shader->Bind();//����ɫ��
        shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);//�ϴ��ӽ�ͶӰ����

        vertexArray->Bind();//�󶨶�������
        RenderCommand::DrawIndexed(vertexArray);//���ƶ�������
    }
}
