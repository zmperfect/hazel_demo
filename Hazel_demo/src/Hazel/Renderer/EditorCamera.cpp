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
        UpdateView();//������ͼ
    }

    void EditorCamera::UpdateProjection()
    {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;//��߱�
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);//ͶӰ����
    }

    void EditorCamera::UpdateView()
    {
        //m_Yaw = m_Pitch = 0.0f;//ƫ���ǣ�������
        m_Position = CalculatePosition();//����λ��

        glm::quat orientation = GetOrientation();//��ȡ����
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);//��ͼ����
        m_ViewMatrix = glm::inverse(m_ViewMatrix);//�����
    }

    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f);//x����ƽ������
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;//x����

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;//y����

        return { xFactor, yFactor };
    }

    float EditorCamera::RotationSpeed() const
    {
        return 0.8f;
    }
    float EditorCamera::ZoomSpeed() const
    {
        float distance = m_Distance * 0.2f;//���ź�ľ���
        distance = std::max(distance, 0.0f);//���ž��벻��С��0
        float speed = distance * distance;//�ٶ�
        speed = std::min(speed, 100.0f);//�ٶ�����100.0f
        return speed;
    }

    void EditorCamera::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(Key::LeftAlt))
        {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };//���λ��
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;//����ƶ�����
            m_InitialMousePosition = mouse;//�������λ��

            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
                MousePan(delta);//����м�ƽ��
            else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
                MouseRotate(delta);//��������ת
            else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
                MouseZoom(delta.y);//����Ҽ�����
        }

        UpdateView();//������ͼ
    }

    void EditorCamera::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(EditorCamera::OnMouseScrolled));//�������¼��ַ�
    }

    bool EditorCamera::OnMouseScrolled(MouseScrolledEvent& e)
    {
        float delta = e.GetYOffset() * 0.1f;//���ֹ�������
        MouseZoom(delta);//����������
        UpdateView();//������ͼ
        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();//ƽ���ٶ�
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;//ƽ��x
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;//ƽ��y
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;//ƫ���Ƿ���
        m_Yaw += yawSign * delta.x * RotationSpeed();//ƫ����
        m_Pitch += delta.y * RotationSpeed();//������
    }

    void EditorCamera::MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed();//���ž���
        if (m_Distance < 1.0f)//���ž��벻��С��1.0f
        {
            m_FocalPoint += GetForwardDirection();//ǰ����
            m_Distance = 1.0f;
        }
    }

    glm::vec3 EditorCamera::GetUpDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));//�Ϸ���
    }

    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));//�ҷ���
    }

    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));//ǰ������
    }

    glm::vec3 EditorCamera::CalculatePosition() const
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;//����λ��
    }

    glm::quat EditorCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));//��ȡ����
    }

}