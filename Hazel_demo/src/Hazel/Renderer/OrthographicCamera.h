#pragma once

#include "glm/glm.hpp"

namespace Hazel {
    
    // This class is used to create a 2D camera.
    class OrthographicCamera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);//This constructor is used to create a 2D camera.

        void SetProjection(float left, float right, float bottom, float top);//This function sets the projection of the camera.

        const glm::vec3& GetPosition() const { return m_Position; }//This function returns the position of the camera.
        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }//This function sets the position of the camera.

        float GetRotation() const { return m_Rotation; }//This function returns the rotation of the camera.
        void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }//This function sets the rotation of the camera.

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
    private:
        void RecalculateViewMatrix();//This function recalculates the view matrix of the camera.
    private:
        glm::mat4 m_ProjectionMatrix;//This is the projection matrix of the camera.
        glm::mat4 m_ViewMatrix;//This is the view matrix of the camera.
        glm::mat4 m_ViewProjectionMatrix;//This is the view projection matrix of the camera.

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float m_Rotation = 0.0f;
    };
}
