#include "hzpch.h"
#include "EditorCamera.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseCodes.h"

#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL // for glm::quat
#include <glm/gtx/quaternion.hpp>

namespace Hazel {

    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
    {
        UpdateView();//更新视图
    }

    void EditorCamera::UpdateProjection()
    {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;//宽高比
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);//投影矩阵
    }

    void EditorCamera::UpdateView()
    {
        //m_Yaw = m_Pitch = 0.0f;//偏航角，俯仰角
        m_Position = CalculatePosition();//计算位置

        glm::quat orientation = GetOrientation();//获取方向
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);//视图矩阵
        m_ViewMatrix = glm::inverse(m_ViewMatrix);//逆矩阵
    }

    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f);//x方向平移下限
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;//x因子

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;//y因子

        return { xFactor, yFactor };
    }

    float EditorCamera::RotationSpeed() const
    {
        return 0.8f;
    }
    float EditorCamera::ZoomSpeed() const
    {
        float distance = m_Distance * 0.2f;//缩放后的距离
        distance = std::max(distance, 0.0f);//缩放距离不能小于0
        float speed = distance * distance;//速度
        speed = std::min(speed, 100.0f);//速度上限100.0f
        return speed;
    }

    void EditorCamera::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(Key::LeftAlt))
        {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };//鼠标位置
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;//鼠标移动距离
            m_InitialMousePosition = mouse;//更新鼠标位置

            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
                MousePan(delta);//鼠标中键平移
            else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
                MouseRotate(delta);//鼠标左键旋转
            else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
                MouseZoom(delta.y);//鼠标右键缩放
        }

        UpdateView();//更新视图
    }

    void EditorCamera::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(EditorCamera::OnMouseScrolled));//鼠标滚轮事件分发
    }

    bool EditorCamera::OnMouseScrolled(MouseScrolledEvent& e)
    {
        float delta = e.GetYOffset() * 0.1f;//滚轮滚动距离
        MouseZoom(delta);//鼠标滚轮缩放
        UpdateView();//更新视图
        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();//平移速度
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;//平移x
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;//平移y
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;//偏航角符号
        m_Yaw += yawSign * delta.x * RotationSpeed();//偏航角
        m_Pitch += delta.y * RotationSpeed();//俯仰角
    }

    void EditorCamera::MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed();//缩放距离
        if (m_Distance < 1.0f)//缩放距离不能小于1.0f
        {
            m_FocalPoint += GetForwardDirection();//前方向
            m_Distance = 1.0f;
        }
    }

    glm::vec3 EditorCamera::GetUpDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));//上方向
    }

    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));//右方向
    }

    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));//前进方向
    }

    glm::vec3 EditorCamera::CalculatePosition() const
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;//计算位置
    }

    glm::quat EditorCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));//获取方向
    }

}