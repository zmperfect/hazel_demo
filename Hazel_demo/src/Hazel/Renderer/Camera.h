#pragma once

#include <glm/glm.hpp>

namespace Hazel {

    class Camera
    {
    public:
        Camera() = default;
        Camera(const glm::mat4& projection)
            : m_Projection(projection) {}  //根据投影矩阵建立相机

        virtual ~Camera() = default;

        const glm::mat4& GetProjection() const { return m_Projection; }//获取投影矩阵
    protected:
        glm::mat4 m_Projection = glm::mat4(1.0f);//投影矩阵
    };
}