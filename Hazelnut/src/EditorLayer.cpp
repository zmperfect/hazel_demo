#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Scene/SceneSerializer.h"

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

#if 0
        // Entity
        auto square = m_ActiveScene->CreateEntity("Green Square");//创建绿色方形实体
        square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });//添加精灵渲染器组件

        auto redSquare = m_ActiveScene->CreateEntity("Red Square");//创建红色方形实体
        redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });//添加精灵渲染器组件

        m_SquareEntity = square;//正方形实体

        m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");//创建相机实体A
        m_CameraEntity.AddComponent<CameraComponent>();//添加相机组件

        m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");//创建相机实体B
        auto& cc = m_SecondCamera.AddComponent<CameraComponent>();//添加相机组件
        cc.Primary = false;//不是主相机

        //脚本类测试
        class CameraController : public ScriptableEntity
        {
        public:
            virtual void OnCreate() override
            {
                auto& translation = GetComponent<TransformComponent>().Translation;//获取变换组件的变换向量
                translation.x = rand() % 10 - 5.0f;
            }

            virtual void OnDestroy() override
            {
            }

            virtual void OnUpdate(Timestep ts) override
            {
                auto& translation = GetComponent<TransformComponent>().Translation;

                float speed = 5.0f;

                if (Input::IsKeyPressed(Key::A))
                    translation.x -= speed * ts;
                if(Input::IsKeyPressed(Key::D))
                    translation.x += speed * ts;
                if (Input::IsKeyPressed(Key::W))
                    translation.y += speed * ts;
                if (Input::IsKeyPressed(Key::S))
                    translation.y -= speed * ts;
            }
        };

        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();//绑定主相机控制器

        m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();//绑定剪辑空间控制器
#endif

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);//设置场景层次面板的上下文

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

            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
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
        ImGuiStyle& style = ImGui::GetStyle();//获取ImGuiStyle
        float minWinSizeX = style.WindowMinSize.x;//获取窗口最小宽度
        style.WindowMinSize.x = 370.0f;//设置窗口最小宽度

        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)//如果启用了停靠
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");//获取dockspace的ID
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);//设置dockspace
        }

        style.WindowMinSize.x = minWinSizeX;//恢复窗口最小宽度

        if (ImGui::BeginMenuBar())//开始菜单栏
        {
            if (ImGui::BeginMenu("File"))
            {
                //禁用全屏将允许窗口移动到其他窗口的前面，
                //如果没有更精细的窗口深度/z控制，我们现在无法撤消。
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1

                if (ImGui::MenuItem("Serialize"))//序列化
                {
                    SceneSerializer serializer(m_ActiveScene);//创建场景序列化器
                    serializer.Serialize("assets/scenes/Example.hazel");//序列化场景
                }

                if (ImGui::MenuItem("Deserialize"))//反序列化
                {
                    SceneSerializer serializer(m_ActiveScene);//创建场景序列化器
                    serializer.Deserialize("assets/scenes/Example.hazel");//反序列化场景
                }

                if (ImGui::MenuItem("Exit")) Application::Get().Close();//退出
                ImGui::EndMenu();//结束菜单
            }
            ImGui::EndMenuBar();//结束菜单栏
        }

        m_SceneHierarchyPanel.OnImGuiRender();//场景层次面板的ImGui渲染

        ImGui::Begin("Stats");//开始显示统计数据

        auto stats = Renderer2D::GetStats();//获取渲染器统计数据
        //显示渲染器统计数据
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::End();//结束设置

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });//设置窗口填充
        ImGui::Begin("Viewport");//开始视口

        m_ViewportFocused = ImGui::IsWindowFocused();//视口是否聚焦
        m_ViewportHovered = ImGui::IsWindowHovered();//视口是否悬停
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);//阻止事件

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//获取视口面板大小
        
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };//设置视口大小

        uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//获取纹理ID
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });//显示纹理
        ImGui::End();
        ImGui::PopStyleVar();//弹出窗口填充

        ImGui::End();

    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);
    }

}