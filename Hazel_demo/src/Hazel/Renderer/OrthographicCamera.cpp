#include "hzpch.h"
#include "Hazel/Renderer/OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

    // This constructor is used to create a 2D camera.
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
    {
        //�ӽǾ����ʼ��Ϊ��λ����
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
    {
        //�ӽǾ���ļ���
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        //�ӽ�ͶӰ����
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    // This function recalculates the view matrix of the camera.
    void OrthographicCamera::RecalculateViewMatrix()
    {
        //�ӽǾ���ļ���
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

        //�ӽǾ���������
        m_ViewMatrix = glm::inverse(transform);
        //�ӽ�ͶӰ����
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

}
