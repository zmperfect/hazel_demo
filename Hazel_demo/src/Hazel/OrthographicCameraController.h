#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Core/Timestep.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel {

    // This class is used to control the 2D camera.
    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false);//����һ���������������

        void OnUpdate(Timestep ts);//�������������
        void OnEvent(Event& e);//�����¼�

        OrthographicCamera& GetCamera() { return m_Camera; }
        const OrthographicCamera& GetCamera() const { return m_Camera; }
    private:
        bool OnMouseScrolled(MouseScrolledEvent& e);//�����������¼�
        bool OnWindowResized(WindowResizeEvent& e);//�����ڴ�С�ı��¼�
    private:
        float m_AspectRatio;//��߱�
        float m_ZoomLevel = 1.0f;//���ŵȼ�
        OrthographicCamera m_Camera;//�������

        bool m_Rotation;//�Ƿ���ת

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };//���λ��
        float m_CameraRotation = 0.0f;//�����ת
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;//���ƽ���ٶ�,�����ת�ٶ�
    };
}