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
    Hazel::ShaderLibrary m_ShaderLibrary;//��ɫ����
    Hazel::Ref<Hazel::Shader> m_Shader;//��ɫ��
    Hazel::Ref<Hazel::VertexArray> m_VertexArray;//��������

    Hazel::Ref<Hazel::Shader> m_FlatColorShader;//ƽ����ɫ��ɫ��
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;//�����ζ�������

    Hazel::Ref<Hazel::Texture2D> m_Texture, m_LJRLogoTexture;//����

    Hazel::OrthographicCameraController m_CameraController;//�������������
    glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };//��������ɫ
};
