#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Scene/SceneSerializer.h"

#include "Hazel/Utils/PlatformUtils.h"

#include "ImGuizmo.h"

#include "Hazel/Math/Math.h"

namespace Hazel {

    extern const std::filesystem::path g_AssetPath;

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
    {
    }

    void EditorLayer::OnAttach()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");//创建棋盘纹理
        m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");//创建播放按钮纹理
        m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");//创建停止按钮纹理
    
        FramebufferSpecification fbSpec;//帧缓冲区规范
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };//帧缓冲区纹理格式，颜色RGBA8，颜色RED_INTEGER，深度
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);//创建帧缓冲区

        m_ActiveScene = CreateRef<Scene>();//创建场景

        auto commandLineArgs = Application::Get().GetCommandLineArgs();//获取命令行参数
        if (commandLineArgs.Count > 1)
        {
            auto sceneFilePath = commandLineArgs[1];//获取命令行参数的第一个参数
            SceneSerializer serializer(m_ActiveScene);//场景序列器存储场景
            serializer.Deserialize(sceneFilePath);//反序列化
        }

        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);//创建编辑器相机

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
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//调整帧缓冲区大小
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);//相机控制器调整大小

            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);//相机视口大小
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//场景视口大小调整
        }

        // Render
        Renderer2D::ResetStats();//重置渲染器统计数据
        m_Framebuffer->Bind();//绑定帧缓冲区
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }指的是RGBA，即红绿蓝透明度，范围是0~1
        RenderCommand::Clear();//清除颜色缓冲区和深度缓冲区

        //Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);//清除帧缓冲区的附件

        switch (m_SceneState)
        {
            case SceneState::Edit:
            {
                if (m_ViewportFocused)
                    m_CameraController.OnUpdate(ts);

                m_EditorCamera.OnUpdate(ts);

                m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
                break;
            }
            case SceneState::Play:
            {
                m_ActiveScene->OnUpdateRuntime(ts);
                break;
            }
        }

        auto[mx, my] = ImGui::GetMousePos();//获取鼠标位置
        mx -= m_ViewportBounds[0].x;//减去视口边界
        my -= m_ViewportBounds[0].y;//减去视口边界
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];//视口大小
        my = viewportSize.y - my;//视口大小减去鼠标位置
        int mouseX = (int)mx;//鼠标X
        int mouseY = (int)my;//鼠标Y

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);//读取像素
            m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
        }

        OnOverlayRender();//覆盖渲染

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
                if (ImGui::MenuItem("New", "Ctrl+N"))//新建
                    NewScene();

                if (ImGui::MenuItem("Open...", "Ctrl+O"))//打开
                    OpenScene();

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))//另存为
                    SaveSceneAs();

                if (ImGui::MenuItem("Exit")) Application::Get().Close();//退出
                ImGui::EndMenu();//结束菜单
            }
            ImGui::EndMenuBar();//结束菜单栏
        }

        // 面板渲染
        m_SceneHierarchyPanel.OnImGuiRender();//场景层次面板的ImGui渲染
        m_ContentBrowserPanel.OnImGuiRender();//内容浏览面板的ImGui渲染

        ImGui::Begin("Stats");//开始显示统计数据

        std::string name = "None";//名称
        if (m_HoveredEntity)//如果悬停实体存在
            name = m_HoveredEntity.GetComponent<TagComponent>().Tag;//获取悬停实体的标签组件的标签
        ImGui::Text("Hovered Entity: %s", name.c_str());//显示悬停实体的名称

        auto stats = Renderer2D::GetStats();//获取渲染器统计数据
        //显示渲染器统计数据
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::End();//结束设置

        // Settings
        ImGui::Begin("Settings");
        ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
        ImGui::End();

        // Viewport
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });//设置窗口填充
        ImGui::Begin("Viewport");//开始视口
        // 窗口设置
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();//获取窗口内容区域最小值
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();//获取窗口内容区域最大值
        auto viewportOffset = ImGui::GetWindowPos();//获取窗口位置
        m_ViewportBounds[0] = { viewportOffset.x + viewportMinRegion.x, viewportOffset.y + viewportMinRegion.y };//设置视口最小边界
        m_ViewportBounds[1] = { viewportOffset.x + viewportMaxRegion.x, viewportOffset.y + viewportMaxRegion.y };//设置视口最大边界

        m_ViewportFocused = ImGui::IsWindowFocused();//视口是否聚焦
        m_ViewportHovered = ImGui::IsWindowHovered();//视口是否悬停
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);//阻止事件,当视口不聚焦和不悬停时

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//获取视口面板大小
        
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };//设置视口大小

        uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);//获取纹理ID
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });//显示纹理

        // 拖放
        if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) //接受拖放的数据
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath) / path);
			}
			ImGui::EndDragDropTarget();
		}


        //Gizmos
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();//获取选中的实体
        if (selectedEntity && m_GizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);//设置正交投影
            ImGuizmo::SetDrawlist();//设置绘制列表

            ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);//设置矩形

            // Camera
            
            //从活动场景中获取相机实体
            //auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
            //const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;//获取相机组件
            //const glm::mat4& cameraProjection = camera.GetProjection();//获取投影矩阵
            //glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());//获取相机变换矩阵

            //Editor Camera
            const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();//获取投影矩阵
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();//获取相机变换矩阵

            // Entity Transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();//获取实体的变换组件
            glm::mat4 transform = tc.GetTransform();//获取变换矩阵

            // Snapping
            bool snap = Input::IsKeyPressed(Key::LeftControl);//是否按下了Ctrl
            float snapValue = 0.5f;//平移/缩放可捕捉到0.5米
            // Snap to 45 degrees for rotation
            if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
                snapValue = 45.0f;

            float snapValues[3] = { snapValue, snapValue, snapValue };//捕捉值

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);//这个函数会根据用户的输入（如鼠标拖动）来修改 transform矩阵，从而改变对象的位置、旋转或缩放。

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;//平移、旋转、缩放
                Math::DecomposeTransform(transform, translation, rotation, scale);//分解变换矩阵

                glm::vec3 deltaRotation = rotation - tc.Rotation;//旋转差值
                tc.Translation = translation;//设置平移
                tc.Rotation += deltaRotation;//设置旋转
                tc.Scale = scale;//设置缩放
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();//弹出窗口填充

        UI_Toolbar();//工具栏

        ImGui::End();

    }

    // 创建一个 ImGui 工具栏，工具栏中有一个图像按钮，点击这个按钮可以在编辑状态和播放状态之间切换。
    void EditorLayer::UI_Toolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        float size = ImGui::GetWindowHeight() - 4.0f;
        Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
        if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
        {
            if (m_SceneState == SceneState::Edit)
                OnScenePlay();
            else if (m_SceneState == SceneState::Play)
                OnSceneStop();
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);//相机控制器的事件
        m_EditorCamera.OnEvent(e);//编辑器相机的事件

        EventDispatcher dispatcher(e);//事件分发器
        dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));//按键按下事件
        dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));//鼠标按下事件
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        //快捷键
        if (e.GetRepeatCount() > 0)
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);//是否按下了Ctrl
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);//是否按下了Shift
        switch (e.GetKeyCode())
        {
            case Key::N:
            {
                if (control)
                    NewScene();

                break;
            }
            case Key::O:
            {
                if (control)
                    OpenScene();

                break;
            }
            case Key::S:
            {
                if (control)
                {
                    if (shift)
                        SaveSceneAs();
                    else
                        SaveScene();
                }

                break;
            }

            // 场景快捷键
            case Key::D:
            {
                if (control)
                    OnDuplicateEntity();

                break;
            }

            //Gizmos的快捷键
            case Key::Q:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = -1;
                break;
            }
            case Key::W: 
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            }
            case Key::E:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            }
            case Key::R:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
            }
        }
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == Mouse::ButtonLeft)
        {
            if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
                m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
        }
        return false;
    }

    void EditorLayer::OnOverlayRender()
    {
        if (m_SceneState == SceneState::Play)
        {
            Entity camera = m_ActiveScene->GetPrimaryCameraEntity();//获取主相机实体
            Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());//开始场景
        }
        else
        {
            Renderer2D::BeginScene(m_EditorCamera);//开始场景
        }

        if (m_ShowPhysicsColliders)
        {
            // Box Colliders
            {
                auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
                for (auto entity : view)
                {
                    auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

                    glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);// collider中心的变换
                    glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);// collider的缩放

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                        * glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                        * glm::scale(glm::mat4(1.0f), scale);

                    Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));//绘制矩形
                }
            }

            // Circle Colliders
            {
                auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
                for (auto entity : view)
                {
                    auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

                    glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
                    glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                        * glm::scale(glm::mat4(1.0f), scale);

                    Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
                }
            }
        }

        Renderer2D::EndScene();//结束场景
    }

    void EditorLayer::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);//设置场景层次面板的上下文

        m_EditorScenePath = std::filesystem::path();//编辑器场景路径为空
    }

    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");//按照过滤打开后缀为.hazel的文件
        if (!filepath.empty())
            OpenScene(filepath);
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        // 检查场景状态
        if (m_SceneState != SceneState::Edit)
            OnSceneStop();

        // 检查文件后缀
        if (path.extension().string() != ".hazel")
        {
            HZ_WARN("Could not load {0} because it is not a Hazel scene file.", path.filename().string());
            return;
        }

        Ref<Scene> newScene = CreateRef<Scene>();//创建新场景
        SceneSerializer serializer(newScene);//场景序列器存储场景
        if (serializer.Deserialize(path.string()))//反序列化
        {
            m_EditorScene = newScene;//编辑器场景
            m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//编辑器场景视口大小调整
            m_SceneHierarchyPanel.SetContext(m_EditorScene);//设置场景层次面板的上下文

            m_ActiveScene = m_EditorScene;//活动场景
            m_EditorScenePath = path;//编辑器场景路径
        }
    }

    void EditorLayer::SaveScene()
    {
        // 每次加载场景时，m_EditorScenePath都会被设置为加载的场景文件的路径。根据这个路径，我们可以知道场景是否已经保存过。
        if (!m_EditorScenePath.empty())
            SerializeScene(m_ActiveScene, m_EditorScenePath);
        else
            SaveSceneAs();
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");//按照过滤保存后缀为.hazel的文件
        if (!filepath.empty())
        {
            SerializeScene(m_ActiveScene, filepath);//序列化场景
            m_EditorScenePath = filepath;//编辑器场景路径
        }
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
    {
        SceneSerializer serializer(scene);//场景序列器存储场景
        serializer.Serialize(path.string());//序列化
    }

    void EditorLayer::OnScenePlay()
    {
        m_SceneState = SceneState::Play;

        m_ActiveScene = Scene::Copy(m_EditorScene);//复制编辑器场景
        m_ActiveScene->OnRuntimeStart();
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);//设置场景层次面板的上下文
    }

    void EditorLayer::OnSceneStop()
    {
        m_SceneState = SceneState::Edit;

        m_ActiveScene->OnRuntimeStop();
        m_ActiveScene = m_EditorScene;

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

    }

    void EditorLayer::OnDuplicateEntity()
    {
        if (m_SceneState != SceneState::Edit)
            return;

        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity)
            m_EditorScene->DuplicateEntity(selectedEntity);
    }

}