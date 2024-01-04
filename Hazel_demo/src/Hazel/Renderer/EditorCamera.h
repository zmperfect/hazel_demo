#pragma once

#include "Camera.h"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Hazel {

    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);//编辑相机

        void OnUpdate(Timestep ts);//更新
        void OnEvent(Event& e);//事件

        inline float GetDistance() const { return m_Distance; }//获取距离
        inline void SetDistance(float distance) { m_Distance = distance; }//设置距离

        inline void SetViewportSize(float width, float height) { m_AspectRatio = width / height; UpdateProjection(); }//设置视口大小

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }//获取视图矩阵
        glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }//获取视图投影矩阵

        glm::vec3 GetUpDirection() const;//获取向上方向
        glm::vec3 GetRightDirection() const;//获取向右方向
        glm::vec3 GetForwardDirection() const;//获取向前方向
        const glm::vec3& GetPosition() const { return m_Position; }//获取位置
        glm::quat GetOrientation() const;//获取方向

        float GetPitch() const { return m_Pitch; }//获取俯仰角
        float GetYaw() const { return m_Yaw; }//获取偏航角
    private:
        void UpdateProjection();//更新投影
        void UpdateView();//更新视图

        bool OnMouseScrolled(MouseScrolledEvent& e);//鼠标滚动事件

        void MousePan(const glm::vec2& delta);//鼠标平移
        void MouseRotate(const glm::vec2& delta);//鼠标旋转
        void MouseZoom(float delta);//鼠标缩放

        glm::vec3 CalculatePosition() const;//计算位置

        std::pair<float, float> PanSpeed() const;//平移速度
        float RotationSpeed() const;//旋转速度
        float ZoomSpeed() const;//缩放速度

    private:
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;//视野，宽高比，近裁剪面，远裁剪面

        glm::mat4 m_ViewMatrix;//视图矩阵
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };//位置
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };//焦点

        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };//初始鼠标位置

        float m_Distance = 10.0f;//距离
        float m_Pitch = 0.0f, m_Yaw = 0.0f;//俯仰角，偏航角

        float m_ViewportWidth = 1280, m_ViewportHeight = 720;//视口宽度，视口高度
    };
}