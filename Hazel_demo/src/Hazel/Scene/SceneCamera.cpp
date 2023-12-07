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
        m_ProjectionType = ProjectionType::Perspective;//ͶӰ����Ϊ͸��ͶӰ
        m_PerspectiveFOV = verticalFOV;//��ֱ�ӽ�
        m_PerspectiveNear = nearClip;//���ü���
        m_PerspectiveFar = farClip;//Զ�ü���
        RecalculateProjection();//���¼���ͶӰ����
    }

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Orthographic;//ͶӰ����Ϊ����ͶӰ
        m_OrthographicSize = size;//����ͶӰ�Ĵ�С
        m_OrthographicNear = nearClip;//����ͶӰ�Ľ��ü���
        m_OrthographicFar = farClip;//����ͶӰ��Զ�ü���
        RecalculateProjection();//���¼���ͶӰ����
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
            //��������������������ұ߽�
            float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoBottom = -m_OrthographicSize * 0.5f;
            float orthoTop = m_OrthographicSize * 0.5f;
            m_Projection = glm::ortho(orthoLeft, orthoRight,
                orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);//��������ͶӰ����
        }
    }

}