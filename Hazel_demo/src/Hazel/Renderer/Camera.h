#pragma once

#include <glm/glm.hpp>

namespace Hazel {

    class Camera
    {
    public:
        Camera(const glm::mat4& projection)
            : m_Projection(projection) {}  //����ͶӰ���������

        const glm::mat4& GetProjection() const { return m_Projection; }//��ȡͶӰ����
    private:
        glm::mat4 m_Projection;
    };
}