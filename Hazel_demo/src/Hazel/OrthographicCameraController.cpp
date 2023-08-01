#include "hzpch.h"
#include "OrthographicCameraController.h"

#include "Hazel/Input.h"
#include "Hazel/KeyCodes.h"

namespace Hazel {

    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
        : m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation = true)//rotationĬ�Ϲرգ���Ϊtrue���������ת����
    {
    }

    void OrthographicCameraController::OnUpdate(Timestep ts)
    {
        if(Input::IsKeyPressed(HZ_KEY_A))
            m_CameraPosition.x -= m_CameraTranslationSpeed * ts;//���λ�õ�x�����ȥ���ƽ���ٶȳ���ʱ�䲽��������������ƶ�
        else if (Input::IsKeyPressed(HZ_KEY_D))
            m_CameraPosition.x += m_CameraTranslationSpeed * ts;//���λ�õ�x����������ƽ���ٶȳ���ʱ�䲽��������������ƶ�

        if (Input::IsKeyPressed(HZ_KEY_W))
            m_CameraPosition.y += m_CameraTranslationSpeed * ts;//���λ�õ�y����������ƽ���ٶȳ���ʱ�䲽��������������ƶ�
        else if (Input::IsKeyPressed(HZ_KEY_S))
            m_CameraPosition.y -= m_CameraTranslationSpeed * ts;//���λ�õ�y�����ȥ���ƽ���ٶȳ���ʱ�䲽��������������ƶ�

        if (m_Rotation)
        {
            if(Input::IsKeyPressed(HZ_KEY_Q))
                m_CameraRotation += m_CameraRotationSpeed * ts;//�����ת�Ƕȼ��������ת�ٶȳ���ʱ�䲽�����������ʱ����ת
            else if (Input::IsKeyPressed(HZ_KEY_E))
                m_CameraRotation -= m_CameraRotationSpeed * ts;//�����ת�Ƕȼ�ȥ�����ת�ٶȳ���ʱ�䲽���������˳ʱ����ת

            m_Camera.SetRotation(m_CameraRotation);//���������ת�Ƕ�
        }

        m_Camera.SetPosition(m_CameraPosition);//�������λ��

        m_CameraTranslationSpeed = m_ZoomLevel;//���ƽ���ٶȵ���������ŵȼ�
    }

    void OrthographicCameraController::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);//�����¼��ַ���
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));//�ַ��������¼�
        dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));//�ַ����ڴ�С�ı��¼�
    }

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
    {
        m_ZoomLevel -= e.GetYOffset() * 0.25f;//������ŵȼ���ȥ�����ֵ�y��ƫ��������0.25
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);//������ŵȼ�����������ŵȼ���0.25�е����ֵ
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);//�������ͶӰ����
        return false;
    }

    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
    {
        m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();//��߱ȵ��ڴ��ڿ�ȳ��Դ��ڸ߶�
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);//�������ͶӰ����
        return false;
    }
}