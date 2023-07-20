#include "hzpch.h"
#include "Renderer.h"

namespace Hazel {

    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;//创建一个场景数据的作用域

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();//获取视角投影矩阵
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
    {
        shader->Bind();//绑定着色器
        shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);//上传视角投影矩阵

        vertexArray->Bind();//绑定顶点数组
        RenderCommand::DrawIndexed(vertexArray);//绘制顶点数组
    }
}
