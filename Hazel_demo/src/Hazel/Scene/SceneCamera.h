#pragma once

#include "Hazel/Renderer/Camera.h"

namespace Hazel {

    class SceneCamera : public Camera
    {
    public:
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };//ͶӰ���ͣ�͸��ͶӰ������ͶӰ
    public:
        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetOrthographic(float size, float nearClip, float farClip);//��������ͶӰ��sizeΪ����ͶӰ�Ĵ�С��nearClipΪ���ü��棬farClipΪԶ�ü���
        void SetPerspective(float verticalFOV, float nearClip, float farClip);//����͸��ͶӰ��verticalFOVΪ��ֱ�ӽǣ�nearClipΪ���ü��棬farClipΪԶ�ü���

        void SetViewportSize(uint32_t width, uint32_t height);//�����ӿڴ�С��widthΪ��ȣ�heightΪ�߶�

        float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }//��ȡ͸��ͶӰ�Ĵ�ֱ�ӽ�
        void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); };//����͸��ͶӰ�Ĵ�ֱ�ӽ�
        float GetPerspectiveNearClip() const { return m_PerspectiveNear; }//��ȡ͸��ͶӰ�Ľ��ü���
        void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); };//����͸��ͶӰ�Ľ��ü���
        float GetPerspectiveFarClip() const { return m_PerspectiveFar; }//��ȡ͸��ͶӰ��Զ�ü���
        void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); };//����͸��ͶӰ��Զ�ü���

        float GetOrthographicSize() const { return m_OrthographicSize; }//��ȡ����ͶӰ�Ĵ�С
        void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); };//��������ͶӰ�Ĵ�С
        float GetOrthographicNearClip() const { return m_OrthographicNear; }//��ȡ����ͶӰ�Ľ��ü���
        void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); };//��������ͶӰ�Ľ��ü���
        float GetOrthographicFarClip() const { return m_OrthographicFar; }//��ȡ����ͶӰ��Զ�ü���
        void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); };//��������ͶӰ��Զ�ü���

        ProjectionType GetProjectionType() const { return m_ProjectionType; }//��ȡͶӰ����
        void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); };//����ͶӰ����s
    private:
        void RecalculateProjection();//���¼���ͶӰ����
    private:
        ProjectionType m_ProjectionType = ProjectionType::Orthographic;//ͶӰ����

        float m_PerspectiveFOV = glm::radians(45.0f);//͸��ͶӰ�Ĵ�ֱ�ӽ�
        float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;//͸��ͶӰ�Ľ��ü����Զ�ü���

        float m_OrthographicSize = 10.0f;//����ͶӰ�Ĵ�С
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;//����ͶӰ�Ľ��ü����Զ�ü���

        float m_AspectRatio = 0.0f;//��߱�
    };
}