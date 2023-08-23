#pragma once

#include "glm/glm.hpp"

namespace Hazel {
    
    class OrthographicCamera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);//创建一个正交相机

        void SetProjection(float left, float right, float bottom, float top);//设置相机的投影矩阵

        const glm::vec3& GetPosition() const { return m_Position; }//返回相机的位置
        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }//设置相机的位置

        float GetRotation() const { return m_Rotation; }//返回相机的旋转角度
        void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }//设置相机的旋转角度

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
    private:
        void RecalculateViewMatrix();//重新计算相机的视图矩阵
    private:
        glm::mat4 m_ProjectionMatrix;//相机的投影矩阵
        glm::mat4 m_ViewMatrix;//相机的视图矩阵
        glm::mat4 m_ViewProjectionMatrix;//相机的视图投影矩阵

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float m_Rotation = 0.0f;
    };
}
