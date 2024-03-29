#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Core/Timestep.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel {

    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false, bool reset = false);//创建一个正交相机控制器

        void OnUpdate(Timestep ts);//更新相机控制器
        void OnEvent(Event& e);//处理事件

        void OnResize(float width, float height);//处理窗口大小改变事件

        OrthographicCamera& GetCamera() { return m_Camera; }
        const OrthographicCamera& GetCamera() const { return m_Camera; }

        float GetZoomLevel() const { return m_ZoomLevel; }//获取缩放等级
        void SetZoomLevel(float level) { m_ZoomLevel = level; }//设置缩放等级
    private:
        bool OnMouseScrolled(MouseScrolledEvent& e);//处理鼠标滚轮事件
        bool OnWindowResized(WindowResizeEvent& e);//处理窗口大小改变事件
    private:
        float m_AspectRatio;//宽高比
        float m_ZoomLevel = 1.0f;//缩放等级
        OrthographicCamera m_Camera;//正交相机

        bool m_Rotation;//是否旋转
        bool m_Reset;//是否复位

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };//相机位置
        float m_CameraRotation = 0.0f;//相机旋转
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;//相机平移速度,相机旋转速度
    };
}