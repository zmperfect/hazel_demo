#pragma once

#include "Hazel.h"
#include "Panels/SceneHierarchyPanel.h"

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

        void NewScene();
        void OpenScene();
        void SaveSceneAs();
    private:
        Hazel::OrthographicCameraController m_CameraController;

        // Temp
        Ref<VertexArray> m_SquareVA;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;//帧缓冲区

        Ref<Scene> m_ActiveScene;//活动场景
        Entity m_SquareEntity;//正方形实体
        Entity m_CameraEntity;//相机实体
        Entity m_SecondCamera;//第二个相机实体

        bool m_PrimaryCamera = true;//主相机

        Ref<Texture2D> m_CheckerboardTexture;

        bool m_ViewportFocused = false, m_ViewportHovered = false;//视口聚焦，视口悬停
        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };//视口大小

        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

        // 面板
        SceneHierarchyPanel m_SceneHierarchyPanel;//场景层次面板
    };
}