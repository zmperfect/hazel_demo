#pragma once

#include "Hazel.h"

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer();
    virtual ~ExampleLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Hazel::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Hazel::Event& e) override;
private:
    Hazel::ShaderLibrary m_ShaderLibrary;//着色器库
    Hazel::Ref<Hazel::Shader> m_Shader;//着色器
    Hazel::Ref<Hazel::VertexArray> m_VertexArray;//顶点数组

    Hazel::Ref<Hazel::Shader> m_FlatColorShader;//平面颜色着色器
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;//正方形顶点数组

    Hazel::Ref<Hazel::Texture2D> m_Texture, m_LJRLogoTexture;//纹理

    Hazel::OrthographicCameraController m_CameraController;//正交相机控制器
    glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };//正方形颜色
};
