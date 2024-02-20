#pragma once

#include "Hazel.h"
#include "Panels/SceneHierarchyPanel.h"

#include "Hazel/Renderer/EditorCamera.h"

namespace Hazel {

    class EditorLayer : public Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void OnUpdate(Timestep ts) override;
        virtual void OnImGuiRender() override;
        void OnEvent(Event& e) override;
    private:
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void NewScene();
        void OpenScene();
        void SaveSceneAs();
    private:
        Hazel::OrthographicCameraController m_CameraController;

        // Temp
        Ref<VertexArray> m_SquareVA;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;//֡������

        Ref<Scene> m_ActiveScene;//�����
        Entity m_SquareEntity;//������ʵ��
        Entity m_CameraEntity;//���ʵ��
        Entity m_SecondCamera;//�ڶ������ʵ��

        Entity m_HoveredEntity;//��ͣʵ��

        bool m_PrimaryCamera = true;//�����

        EditorCamera m_EditorCamera;//�༭�����

        Ref<Texture2D> m_CheckerboardTexture;

        bool m_ViewportFocused = false, m_ViewportHovered = false;//�ӿھ۽����ӿ���ͣ
        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };//�ӿڴ�С
        glm::vec2 m_ViewportBounds[2];//�ӿڱ߽�

        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

        int m_GizmoType = -1;//Gizmo����

        // ���
        SceneHierarchyPanel m_SceneHierarchyPanel;//����������
    };
}