#include "hzpch.h"
#include "Hazel/Renderer/OrthographicCameraController.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"

namespace Hazel {

    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation, bool reset)
        : m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation = true), m_Reset(reset = true)//rotation默认关闭，改为true则开启相机旋转功能，复位功能同理
    {
    }

    void OrthographicCameraController::OnUpdate(Timestep ts)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        if (Input::IsKeyPressed(HZ_KEY_A))
        {
            m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//相机位置的x坐标减去cos(相机旋转角度)乘以相机平移速度乘以时间步长，即相机向左移动
            m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//相机位置的y坐标减去sin(相机旋转角度)乘以相机平移速度乘以时间步长，即相机向左移动
        }
        else if (Input::IsKeyPressed(HZ_KEY_D))
        {
            m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//相机位置的x坐标加上cos(相机旋转角度)乘以相机平移速度乘以时间步长，即相机向右移动
            m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//相机位置的y坐标加上sin(相机旋转角度)乘以相机平移速度乘以时间步长，即相机向右移动
        }

        if (Input::IsKeyPressed(HZ_KEY_W))
        {
            m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//相机位置的x坐标加上-sin(相机旋转角度)乘以相机平移速度乘以时间步长，即相机向上移动
            m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//相机位置的y坐标加上cos(相机旋转角度)乘以相机平移速度乘以时间步长，即相机向上移动
        }
        else if (Input::IsKeyPressed(HZ_KEY_S))
        {
            m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//相机位置的x坐标减去-sin(相机旋转角度)乘以相机平移速度乘以时间步长，即相机向下移动
            m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;//相机位置的y坐标减去cos(相机旋转角度)乘以相机平移速度乘以时间步长，即相机向下移动
        }

        if (m_Rotation)
        {
            if(Input::IsKeyPressed(HZ_KEY_Q))
                m_CameraRotation += m_CameraRotationSpeed * ts;//相机旋转角度加上相机旋转速度乘以时间步长，即相机逆时针旋转
            else if (Input::IsKeyPressed(HZ_KEY_E))
                m_CameraRotation -= m_CameraRotationSpeed * ts;//相机旋转角度减去相机旋转速度乘以时间步长，即相机顺时针旋转

            if(m_CameraRotation > 180.0f)
                m_CameraRotation -= 360.0f;//如果相机旋转角度大于180度，相机旋转角度减去360度
            else if (m_CameraRotation <= -180.0f)
                m_CameraRotation += 360.0f;

            m_Camera.SetRotation(m_CameraRotation);//设置相机旋转角度
        }

        if (m_Reset) 
        {
            if (Input::IsKeyPressed(HZ_KEY_Z))
            {
                m_CameraPosition = { 0.0f, 0.0f, 0.0f };//相机位置复位
                m_CameraRotation = 0.0f;//相机旋转角度复位
                m_ZoomLevel = 1.0f;//相机缩放等级复位
            }
        }

        m_Camera.SetPosition(m_CameraPosition);//设置相机位置

        m_CameraTranslationSpeed = m_ZoomLevel;//相机平移速度等于相机缩放等级
    }

    void OrthographicCameraController::OnEvent(Event& e)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        EventDispatcher dispatcher(e);//创建事件分发器
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));//分发鼠标滚轮事件
        dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));//分发窗口大小改变事件
    }

    void OrthographicCameraController::OnResize(float width, float height)
    {
        m_AspectRatio = width / height;//宽高比等于宽度除以高度
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);//设置相机投影矩阵
    }

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        m_ZoomLevel -= e.GetYOffset() * 0.25f;//相机缩放等级减去鼠标滚轮的y轴偏移量乘以0.25
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);//相机缩放等级等于相机缩放等级和0.25中的最大值
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);//设置相机投影矩阵
        return false;
    }

    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        OnResize((float)e.GetWidth(), (float)e.GetHeight());//调用OnResize函数
        return false;
    }
}