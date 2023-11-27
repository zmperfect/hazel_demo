#pragma once

#include "Hazel/Renderer/Camera.h"

namespace Hazel {

    class SceneCamera : public Camera
    {
    public:
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };//投影类型，透视投影和正交投影
    public:
        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetOrthographic(float size, float nearClip, float farClip);//设置正交投影，size为正交投影的大小，nearClip为近裁剪面，farClip为远裁剪面
        void SetPerspective(float verticalFOV, float nearClip, float farClip);//设置透视投影，verticalFOV为垂直视角，nearClip为近裁剪面，farClip为远裁剪面

        void SetViewportSize(uint32_t width, uint32_t height);//设置视口大小，width为宽度，height为高度

        float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }//获取透视投影的垂直视角
        void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); };//设置透视投影的垂直视角
        float GetPerspectiveNearClip() const { return m_PerspectiveNear; }//获取透视投影的近裁剪面
        void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); };//设置透视投影的近裁剪面
        float GetPerspectiveFarClip() const { return m_PerspectiveFar; }//获取透视投影的远裁剪面
        void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); };//设置透视投影的远裁剪面

        float GetOrthographicSize() const { return m_OrthographicSize; }//获取正交投影的大小
        void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); };//设置正交投影的大小
        float GetOrthographicNearClip() const { return m_OrthographicNear; }//获取正交投影的近裁剪面
        void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); };//设置正交投影的近裁剪面
        float GetOrthographicFarClip() const { return m_OrthographicFar; }//获取正交投影的远裁剪面
        void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); };//设置正交投影的远裁剪面

        ProjectionType GetProjectionType() const { return m_ProjectionType; }//获取投影类型
        void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); };//设置投影类型s
    private:
        void RecalculateProjection();//重新计算投影矩阵
    private:
        ProjectionType m_ProjectionType = ProjectionType::Orthographic;//投影类型

        float m_PerspectiveFOV = glm::radians(45.0f);//透视投影的垂直视角
        float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;//透视投影的近裁剪面和远裁剪面

        float m_OrthographicSize = 10.0f;//正交投影的大小
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;//正交投影的近裁剪面和远裁剪面

        float m_AspectRatio = 0.0f;//宽高比
    };
}