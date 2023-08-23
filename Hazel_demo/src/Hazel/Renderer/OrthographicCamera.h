#pragma once

#include "glm/glm.hpp"

namespace Hazel {
    
    class OrthographicCamera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);//����һ���������

        void SetProjection(float left, float right, float bottom, float top);//���������ͶӰ����

        const glm::vec3& GetPosition() const { return m_Position; }//���������λ��
        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }//���������λ��

        float GetRotation() const { return m_Rotation; }//�����������ת�Ƕ�
        void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }//�����������ת�Ƕ�

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
    private:
        void RecalculateViewMatrix();//���¼����������ͼ����
    private:
        glm::mat4 m_ProjectionMatrix;//�����ͶӰ����
        glm::mat4 m_ViewMatrix;//�������ͼ����
        glm::mat4 m_ViewProjectionMatrix;//�������ͼͶӰ����

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float m_Rotation = 0.0f;
    };
}
