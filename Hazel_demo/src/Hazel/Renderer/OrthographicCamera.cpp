#include "hzpch.h"
#include "Hazel/Renderer/OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

    // This constructor is used to create a 2D camera.
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
    {
        //视角矩阵初始化为单位矩阵
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
    {
        //视角矩阵的计算
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        //视角投影矩阵
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    // This function recalculates the view matrix of the camera.
    void OrthographicCamera::RecalculateViewMatrix()
    {
        //视角矩阵的计算
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

        //视角矩阵的逆矩阵
        m_ViewMatrix = glm::inverse(transform);
        //视角投影矩阵
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

}
