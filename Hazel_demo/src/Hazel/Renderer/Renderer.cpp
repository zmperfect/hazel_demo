#include "hzpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

    Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;//����һ���������ݵ�������

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();//��ȡ�ӽ�ͶӰ����
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
    {
        shader->Bind();//����ɫ��
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);//�ϴ��ӽ�ͶӰ����
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);//�ϴ��任����

        vertexArray->Bind();//�󶨶�������
        RenderCommand::DrawIndexed(vertexArray);//���ƶ�������
    }
}
