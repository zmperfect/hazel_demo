#pragma once

#include "Hazel/Renderer/Camera.h"

namespace Hazel {

    class SceneCamera : public Camera
    {
    public:
        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetOrthographic(float size, float nearClip, float farClip);//设置正交投影，size为正交投影的大小，nearClip为近裁剪面，farClip为远裁剪面

        void SetViewportSize(uint32_t width, uint32_t height);//设置视口大小，width为宽度，height为高度

        float GetOrthographicSize() const { return m_OrthographicSize; }//获取正交投影的大小
        void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); };//设置正交投影的大小

    private:
        void RecalculateProjection();//重新计算投影矩阵
    private:
        float m_OrthographicSize = 10.0f;//正交投影的大小
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;//正交投影的近裁剪面和远裁剪面

        float m_AspectRatio = 0.0f;//宽高比
    };
}