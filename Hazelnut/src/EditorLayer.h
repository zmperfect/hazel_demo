#pragma once

#include "Hazel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

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

        void OnOverlayRender();//覆盖渲染

        void NewScene();
        void OpenScene();
        void OpenScene(const std::filesystem::path& path);
        void SaveScene();
        void SaveSceneAs();

        void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);//序列化场景

        void OnScenePlay();
        void OnSceneStop();

        void OnDuplicateEntity();//复制实体

        // UI Panels
        void UI_Toolbar();
    private:
        Hazel::OrthographicCameraController m_CameraController;

        // Temp
        Ref<VertexArray> m_SquareVA;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;//帧缓冲区

        Ref<Scene> m_ActiveScene;//活动场景
        Ref<Scene> m_EditorScene;//编辑器场景
        std::filesystem::path m_EditorScenePath;//活动场景文件路径
        Entity m_SquareEntity;//正方形实体
        Entity m_CameraEntity;//相机实体
        Entity m_SecondCamera;//第二个相机实体

        Entity m_HoveredEntity;//悬停实体

        bool m_PrimaryCamera = true;//主相机

        EditorCamera m_EditorCamera;//编辑器相机

        Ref<Texture2D> m_CheckerboardTexture;

        bool m_ViewportFocused = false, m_ViewportHovered = false;//视口聚焦，视口悬停
        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };//视口大小
        glm::vec2 m_ViewportBounds[2];//视口边界

        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

        int m_GizmoType = -1;//Gizmo类型

        bool m_ShowPhysicsColliders = false;//显示物理碰撞器

        enum class SceneState
        {
            Edit = 0, Play = 1, Pause = 2
        };
        SceneState m_SceneState = SceneState::Edit;// 场景状态

        // 面板
        SceneHierarchyPanel m_SceneHierarchyPanel;//场景层次面板
        ContentBrowserPanel m_ContentBrowserPanel;//内容浏览面板

        // 图标
        Ref<Texture2D> m_IconPlay, m_IconStop;
    };
}