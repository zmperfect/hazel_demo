#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
		: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{
    Hazel::FramebufferSpecification fbSpec;//帧缓冲区规范
    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);//创建帧缓冲区
}

void Sandbox2D::OnAttach()
{
    HZ_PROFILE_FUNCTION();//获取函数签名

    m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
    HZ_PROFILE_FUNCTION();//获取函数签名
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
    HZ_PROFILE_FUNCTION();//获取函数签名

    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    Hazel::Renderer2D::ResetStats();//重置渲染器统计数据
    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        m_Framebuffer->Bind();//绑定帧缓冲区
        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }指的是RGBA，即红绿蓝透明度，范围是0~1
        Hazel::RenderCommand::Clear();//清除颜色缓冲区和深度缓冲区
    }

    {
        static float rotation = 0.0f;//旋转角度
        rotation += ts * 50.0f;

        HZ_PROFILE_SCOPE("Renderer Draw");
        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());//开始渲染场景
        Hazel::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });//绘制旋转矩形，位置，大小，旋转角度，颜色
        Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
        Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
        Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
        Hazel::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20.0f);
        Hazel::Renderer2D::EndScene();//结束渲染场景

        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());//开始渲染场景
        for (float y = -0.5f; y < 5.0f; y += 0.5f)
        {
            for (float x = -0.5f; x < 5.0f; x += 0.5f)
            {
                glm::vec4 color = { (x + 0.5f) / 5.0f, 0.4f, (y + 0.5f) / 5.0f, 0.5f };//颜色
                Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);//绘制矩形，位置，大小，颜色
            }
        }
        Hazel::Renderer2D::EndScene();//结束渲染场景
        m_Framebuffer->Unbind();//解绑帧缓冲区
    }
}

void Sandbox2D::OnImGuiRender()
{
    HZ_PROFILE_FUNCTION();//获取函数签名

    //注意：将dockingEnabled设置为true来启动dockspace
    static bool dockingEnabled = true;
    if (dockingEnabled)
    {
        static bool dockspaceOpen = true;//dockspace是否打开
        static bool opt_fullscreen_persistant = true;//是否全屏 
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;//dockspace的标志


        //我们使用ImGuiWindowFlags_NoDocking标志使父窗口不可停靠到，
        //因为两个对接目标彼此之间会很混乱。
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();//获取主视口
            ImGui::SetNextWindowPos(viewport->Pos);//设置下一个窗口的位置
            ImGui::SetNextWindowSize(viewport->Size);//设置下一个窗口的大小
            ImGui::SetNextWindowViewport(viewport->ID);//设置下一个窗口的视口
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);//设置窗口圆角
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);//设置窗口边框大小
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;//设置窗口标志
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;//设置窗口标志
        }

        //当使用ImGuiDockNodeFlags_PassthruCentralNode标志时，我们希望dockspace在窗口中填充整个区域。
        if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        //Importance：即使Begin()返回false，也会继续
        //因为我们想让dockspace一直处于活跃状态。
        //如果dockspace处于非活跃状态，所有停靠到它的活动窗口将失去它们的父窗口并被取消停靠。
        //我们不能保留活动窗口和非活动停靠之间的对接关系，否则 
        //任何dockspace/设置的更改都会导致窗口被困在limbo中，永远不可见。
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));//设置窗口填充
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);//开始dockspace
        ImGui::PopStyleVar();//弹出窗口填充

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);//弹出窗口圆角和边框大小

        //DockSpace
        ImGuiIO& io = ImGui::GetIO();//获取ImGuiIO
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");//获取dockspace的ID
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);//设置dockspace
        }

        if (ImGui::BeginMenuBar())//开始菜单栏
        {
            if (ImGui::BeginMenu("File"))
            {
                //禁用全屏将允许窗口移动到其他窗口的前面，
                //如果没有更精细的窗口深度/z控制，我们现在无法撤消。
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
                if (ImGui::MenuItem("Exit")) Hazel::Application::Get().Close();//退出
                ImGui::EndMenu();//结束菜单
            }
            ImGui::EndMenuBar();//结束菜单栏
        }

        ImGui::Begin("Settings");//开始设置

        auto stats = Hazel::Renderer2D::GetStats();//获取渲染器统计数据
        //显示渲染器统计数据
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));//编辑颜色

        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//获取纹理ID
        ImGui::Image((void*)textureID, ImVec2{ 1280, 720 });//显示纹理
        ImGui::End();

        ImGui::End();//结束设置
    }
    else
    {
        ImGui::Begin("Settings");//开始设置

        auto stats = Hazel::Renderer2D::GetStats();//获取渲染器统计数据
        //显示渲染器统计数据
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));//编辑颜色

        uint32_t textureID = m_CheckerboardTexture->GetRendererID();//获取纹理ID
        ImGui::Image((void*)textureID, ImVec2{ 1280, 720 });//显示纹理
        ImGui::End();//结束设置
    }
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
    m_CameraController.OnEvent(e);
}
