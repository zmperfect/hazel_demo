#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel {

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
    {
    }

    void EditorLayer::OnAttach()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
        
        FramebufferSpecification fbSpec;//帧缓冲区规范
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);//创建帧缓冲区

        m_ActiveScene = CreateRef<Scene>();//创建场景

        // Entity
        auto square = m_ActiveScene->CreateEntity("Green Square");//创建绿色方形实体
        square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });//添加精灵渲染器组件

        m_SquareEntity = square;//正方形实体

        m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");//创建相机实体
        m_CameraEntity.AddComponent<CameraComponent>(glm::ortho(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f));//添加相机组件

        m_SecondCamera = m_ActiveScene->CreateEntity("Clip-Space Entity");//创建剪辑空间实体
        auto& cc = m_SecondCamera.AddComponent<CameraComponent>(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f));//添加相机组件
        cc.Primary = false;//不是主相机
    }

    void EditorLayer::OnDetach()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        // Resize
        // 修复调整视口大小时Hazelnut的黑色闪烁
        //将“旧”大小的帧缓冲区渲染到“新”大小的ImGuiPanel上，并将“新”大小存储在m_ViewportSize中。下一帧将首先调整帧缓冲区的大小，因为在更新和渲染之前，m_ViewportSize与m_Framebuffer.Width/Height不同。这将导致从不渲染空（黑色）帧缓冲区。
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        }

        // 视口聚焦时Update
        if(m_ViewportFocused)
            m_CameraController.OnUpdate(ts);

        // Render
        Renderer2D::ResetStats();//重置渲染器统计数据
        m_Framebuffer->Bind();//绑定帧缓冲区
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }指的是RGBA，即红绿蓝透明度，范围是0~1
        RenderCommand::Clear();//清除颜色缓冲区和深度缓冲区

        //Update scene
        m_ActiveScene->OnUpdate(ts);

        m_Framebuffer->Unbind();

    }

    void EditorLayer::OnImGuiRender()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        //注意：将dockingEnabled设置为true来启动dockspace
        
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
                if (ImGui::MenuItem("Exit")) Application::Get().Close();//退出
                ImGui::EndMenu();//结束菜单
            }
            ImGui::EndMenuBar();//结束菜单栏
        }

        ImGui::Begin("Settings");//开始设置

        auto stats = Renderer2D::GetStats();//获取渲染器统计数据
        //显示渲染器统计数据
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        if (m_SquareEntity)//如果有方块实体
        {
            ImGui::Separator();//分割线
            auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;//获取方块实体的标签
            ImGui::Text("%s", tag.c_str());//显示标签
            
            auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;//获取方块实体的颜色
            ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));//编辑方块颜色
            ImGui::Separator();//分割线
        }

        ImGui::DragFloat3("Camera Transform",
            glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3])); //编辑相机位置

        if(ImGui::Checkbox("Camera A", &m_PrimaryCamera))//编辑相机是否为主相机
        {
            m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
            m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
        }

        ImGui::End();//结束设置

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });//设置窗口填充
        ImGui::Begin("Viewport");//开始视口

        m_ViewportFocused = ImGui::IsWindowFocused();//视口是否聚焦
        m_ViewportHovered = ImGui::IsWindowHovered();//视口是否悬停
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);//阻止事件

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//获取视口面板大小
        
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };//设置视口大小

        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//获取纹理ID
        ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();
        ImGui::PopStyleVar();//弹出窗口填充

        ImGui::End();

    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);
    }

}