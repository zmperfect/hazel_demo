#include "hzpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

    SceneCamera::SceneCamera()
    {
        RecalculateProjection();
    }

    void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Perspective;//投影类型为透视投影
        m_PerspectiveFOV = verticalFOV;//垂直视角
        m_PerspectiveNear = nearClip;//近裁剪面
        m_PerspectiveFar = farClip;//远裁剪面
        RecalculateProjection();//重新计算投影矩阵
    }

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Orthographic;//投影类型为正交投影
        m_OrthographicSize = size;//正交投影的大小
        m_OrthographicNear = nearClip;//正交投影的近裁剪面
        m_OrthographicFar = farClip;//正交投影的远裁剪面
        RecalculateProjection();//重新计算投影矩阵
    }

    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = (float)width / (float)height;
        RecalculateProjection();
    }

    void SceneCamera::RecalculateProjection()
    {
        if (m_ProjectionType == ProjectionType::Perspective)
        {
            m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
        }
        else
        {
            //计算正交相机的上下左右边界
            float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoBottom = -m_OrthographicSize * 0.5f;
            float orthoTop = m_OrthographicSize * 0.5f;
            m_Projection = glm::ortho(orthoLeft, orthoRight,
                orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);//计算正交投影矩阵
        }
    }

}