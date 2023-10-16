#include "hzpch.h"
#include "Hazel/Renderer/OrthographicCameraController.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"

namespace Hazel {

    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation, bool reset)
        : m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation = true), m_Reset(reset = true)//rotationĬ�Ϲرգ���Ϊtrue���������ת���ܣ���λ����ͬ��
    {
    }

    void OrthographicCameraController::OnUpdate(Timestep ts)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        if (Input::IsKeyPressed(HZ_KEY_A))
        {
            m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//���λ�õ�x�����ȥcos(�����ת�Ƕ�)�������ƽ���ٶȳ���ʱ�䲽��������������ƶ�
            m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//���λ�õ�y�����ȥsin(�����ת�Ƕ�)�������ƽ���ٶȳ���ʱ�䲽��������������ƶ�
        }
        else if (Input::IsKeyPressed(HZ_KEY_D))
        {
            m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//���λ�õ�x�������cos(�����ת�Ƕ�)�������ƽ���ٶȳ���ʱ�䲽��������������ƶ�
            m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//���λ�õ�y�������sin(�����ת�Ƕ�)�������ƽ���ٶȳ���ʱ�䲽��������������ƶ�
        }

        if (Input::IsKeyPressed(HZ_KEY_W))
        {
            m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//���λ�õ�x�������-sin(�����ת�Ƕ�)�������ƽ���ٶȳ���ʱ�䲽��������������ƶ�
            m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//���λ�õ�y�������cos(�����ת�Ƕ�)�������ƽ���ٶȳ���ʱ�䲽��������������ƶ�
        }
        else if (Input::IsKeyPressed(HZ_KEY_S))
        {
            m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//���λ�õ�x�����ȥ-sin(�����ת�Ƕ�)�������ƽ���ٶȳ���ʱ�䲽��������������ƶ�
            m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//���λ�õ�y�����ȥcos(�����ת�Ƕ�)�������ƽ���ٶȳ���ʱ�䲽��������������ƶ�
        }

        if (m_Rotation)
        {
            if(Input::IsKeyPressed(HZ_KEY_Q))
                m_CameraRotation += m_CameraRotationSpeed * ts;//�����ת�Ƕȼ��������ת�ٶȳ���ʱ�䲽�����������ʱ����ת
            else if (Input::IsKeyPressed(HZ_KEY_E))
                m_CameraRotation -= m_CameraRotationSpeed * ts;//�����ת�Ƕȼ�ȥ�����ת�ٶȳ���ʱ�䲽���������˳ʱ����ת

            if(m_CameraRotation > 180.0f)
                m_CameraRotation -= 360.0f;//��������ת�Ƕȴ���180�ȣ������ת�Ƕȼ�ȥ360��
            else if (m_CameraRotation <= -180.0f)
                m_CameraRotation += 360.0f;

            m_Camera.SetRotation(m_CameraRotation);//���������ת�Ƕ�
        }

        if (m_Reset) 
        {
            if (Input::IsKeyPressed(HZ_KEY_Z))
            {
                m_CameraPosition = { 0.0f, 0.0f, 0.0f };//���λ�ø�λ
                m_CameraRotation = 0.0f;//�����ת�Ƕȸ�λ
                m_ZoomLevel = 1.0f;//������ŵȼ���λ
            }
        }

        m_Camera.SetPosition(m_CameraPosition);//�������λ��

        m_CameraTranslationSpeed = m_ZoomLevel;//���ƽ���ٶȵ���������ŵȼ�
    }

    void OrthographicCameraController::OnEvent(Event& e)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        EventDispatcher dispatcher(e);//�����¼��ַ���
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));//�ַ��������¼�
        dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));//�ַ����ڴ�С�ı��¼�
    }

    void OrthographicCameraController::OnResize(float width, float height)
    {
        m_AspectRatio = width / height;//��߱ȵ��ڿ�ȳ��Ը߶�
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);//�������ͶӰ����
    }

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        m_ZoomLevel -= e.GetYOffset() * 0.25f;//������ŵȼ���ȥ�����ֵ�y��ƫ��������0.25
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);//������ŵȼ�����������ŵȼ���0.25�е����ֵ
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);//�������ͶӰ����
        return false;
    }

    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        OnResize((float)e.GetWidth(), (float)e.GetHeight());//����OnResize����
        return false;
    }
}