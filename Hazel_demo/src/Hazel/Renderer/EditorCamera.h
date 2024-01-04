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
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);//�༭���

        void OnUpdate(Timestep ts);//����
        void OnEvent(Event& e);//�¼�

        inline float GetDistance() const { return m_Distance; }//��ȡ����
        inline void SetDistance(float distance) { m_Distance = distance; }//���þ���

        inline void SetViewportSize(float width, float height) { m_AspectRatio = width / height; UpdateProjection(); }//�����ӿڴ�С

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }//��ȡ��ͼ����
        glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }//��ȡ��ͼͶӰ����

        glm::vec3 GetUpDirection() const;//��ȡ���Ϸ���
        glm::vec3 GetRightDirection() const;//��ȡ���ҷ���
        glm::vec3 GetForwardDirection() const;//��ȡ��ǰ����
        const glm::vec3& GetPosition() const { return m_Position; }//��ȡλ��
        glm::quat GetOrientation() const;//��ȡ����

        float GetPitch() const { return m_Pitch; }//��ȡ������
        float GetYaw() const { return m_Yaw; }//��ȡƫ����
    private:
        void UpdateProjection();//����ͶӰ
        void UpdateView();//������ͼ

        bool OnMouseScrolled(MouseScrolledEvent& e);//�������¼�

        void MousePan(const glm::vec2& delta);//���ƽ��
        void MouseRotate(const glm::vec2& delta);//�����ת
        void MouseZoom(float delta);//�������

        glm::vec3 CalculatePosition() const;//����λ��

        std::pair<float, float> PanSpeed() const;//ƽ���ٶ�
        float RotationSpeed() const;//��ת�ٶ�
        float ZoomSpeed() const;//�����ٶ�

    private:
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;//��Ұ����߱ȣ����ü��棬Զ�ü���

        glm::mat4 m_ViewMatrix;//��ͼ����
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };//λ��
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };//����

        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };//��ʼ���λ��

        float m_Distance = 10.0f;//����
        float m_Pitch = 0.0f, m_Yaw = 0.0f;//�����ǣ�ƫ����

        float m_ViewportWidth = 1280, m_ViewportHeight = 720;//�ӿڿ�ȣ��ӿڸ߶�
    };
}