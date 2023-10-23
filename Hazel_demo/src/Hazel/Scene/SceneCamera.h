#pragma once

#include "Hazel/Renderer/Camera.h"

namespace Hazel {

    class SceneCamera : public Camera
    {
    public:
        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetOrthographic(float size, float nearClip, float farClip);//��������ͶӰ��sizeΪ����ͶӰ�Ĵ�С��nearClipΪ���ü��棬farClipΪԶ�ü���

        void SetViewportSize(uint32_t width, uint32_t height);//�����ӿڴ�С��widthΪ��ȣ�heightΪ�߶�

        float GetOrthographicSize() const { return m_OrthographicSize; }//��ȡ����ͶӰ�Ĵ�С
        void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); };//��������ͶӰ�Ĵ�С

    private:
        void RecalculateProjection();//���¼���ͶӰ����
    private:
        float m_OrthographicSize = 10.0f;//����ͶӰ�Ĵ�С
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;//����ͶӰ�Ľ��ü����Զ�ü���

        float m_AspectRatio = 0.0f;//��߱�
    };
}