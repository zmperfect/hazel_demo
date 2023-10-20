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
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
        
        FramebufferSpecification fbSpec;//֡�������淶
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);//����֡������

        m_ActiveScene = CreateRef<Scene>();//��������

        auto square = m_ActiveScene->CreateEntity();//����ʵ��
        m_ActiveScene->Reg().emplace<TransformComponent>(square);//���任������ӵ�ʵ��
        m_ActiveScene->Reg().emplace<SpriteRendererComponent>(square, glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });//��������Ⱦ��������ӵ�ʵ��

        m_SquareEntity = square;//������ʵ��
    }

    void EditorLayer::OnDetach()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        // �ӿھ۽�ʱUpdate
        if(m_ViewportFocused)
            m_CameraController.OnUpdate(ts);

        // Render
        Renderer2D::ResetStats();//������Ⱦ��ͳ������
        m_Framebuffer->Bind();//��֡������
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }ָ����RGBA����������͸���ȣ���Χ��0~1
        RenderCommand::Clear();//�����ɫ����������Ȼ�����

        Renderer2D::BeginScene(m_CameraController.GetCamera());

        //Update scene
        m_ActiveScene->OnUpdate(ts);

        Renderer2D::EndScene();

        m_Framebuffer->Unbind();

        // Resize
        // �޸������ӿڴ�СʱHazelnut�ĺ�ɫ��˸
        //�����ɡ���С��֡��������Ⱦ�����¡���С��ImGuiPanel�ϣ��������¡���С�洢��m_ViewportSize�С���һ֡�����ȵ���֡�������Ĵ�С����Ϊ�ڸ��º���Ⱦ֮ǰ��m_ViewportSize��m_Framebuffer.Width/Height��ͬ���⽫���´Ӳ���Ⱦ�գ���ɫ��֡��������
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        }

    }

    void EditorLayer::OnImGuiRender()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        //ע�⣺��dockingEnabled����Ϊtrue������dockspace
        
        static bool dockspaceOpen = true;//dockspace�Ƿ��
        static bool opt_fullscreen_persistant = true;//�Ƿ�ȫ�� 
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;//dockspace�ı�־

        //����ʹ��ImGuiWindowFlags_NoDocking��־ʹ�����ڲ���ͣ������
        //��Ϊ�����Խ�Ŀ��˴�֮���ܻ��ҡ�
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();//��ȡ���ӿ�
            ImGui::SetNextWindowPos(viewport->Pos);//������һ�����ڵ�λ��
            ImGui::SetNextWindowSize(viewport->Size);//������һ�����ڵĴ�С
            ImGui::SetNextWindowViewport(viewport->ID);//������һ�����ڵ��ӿ�
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);//���ô���Բ��
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);//���ô��ڱ߿��С
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;//���ô��ڱ�־
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;//���ô��ڱ�־
        }

        //��ʹ��ImGuiDockNodeFlags_PassthruCentralNode��־ʱ������ϣ��dockspace�ڴ����������������
        if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

        //Importance����ʹBegin()����false��Ҳ�����
        //��Ϊ��������dockspaceһֱ���ڻ�Ծ״̬��
        //���dockspace���ڷǻ�Ծ״̬������ͣ�������Ļ���ڽ�ʧȥ���ǵĸ����ڲ���ȡ��ͣ����
        //���ǲ��ܱ�������ںͷǻͣ��֮��ĶԽӹ�ϵ������ 
        //�κ�dockspace/���õĸ��Ķ��ᵼ�´��ڱ�����limbo�У���Զ���ɼ���
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));//���ô������
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);//��ʼdockspace
        ImGui::PopStyleVar();//�����������

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);//��������Բ�Ǻͱ߿��С

        //DockSpace
        ImGuiIO& io = ImGui::GetIO();//��ȡImGuiIO
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");//��ȡdockspace��ID
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);//����dockspace
        }

        if (ImGui::BeginMenuBar())//��ʼ�˵���
        {
            if (ImGui::BeginMenu("File"))
            {
                //����ȫ�������������ƶ����������ڵ�ǰ�棬
                //���û�и���ϸ�Ĵ������/z���ƣ����������޷�������
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
                if (ImGui::MenuItem("Exit")) Application::Get().Close();//�˳�
                ImGui::EndMenu();//�����˵�
            }
            ImGui::EndMenuBar();//�����˵���
        }

        ImGui::Begin("Settings");//��ʼ����

        auto stats = Renderer2D::GetStats();//��ȡ��Ⱦ��ͳ������
        //��ʾ��Ⱦ��ͳ������
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());


        auto& squareColor = m_ActiveScene->Reg().get<SpriteRendererComponent>(m_SquareEntity).Color;//��ȡ������ɫ
        ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));//�༭��ɫ

        ImGui::End();//��������

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });//���ô������
        ImGui::Begin("Viewport");//��ʼ�ӿ�

        m_ViewportFocused = ImGui::IsWindowFocused();//�ӿ��Ƿ�۽�
        m_ViewportHovered = ImGui::IsWindowHovered();//�ӿ��Ƿ���ͣ
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);//��ֹ�¼�

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//��ȡ�ӿ�����С
        
        //�����ӿڴ�С
        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
        {
            m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

            m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
        }

        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//��ȡ����ID
        ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();
        ImGui::PopStyleVar();//�����������

        ImGui::End();

    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);
    }

}