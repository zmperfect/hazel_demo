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

        m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
        
        Hazel::FramebufferSpecification fbSpec;//֡�������淶
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);//����֡������
    }

    void EditorLayer::OnDetach()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��
    }

    void EditorLayer::OnUpdate(Hazel::Timestep ts)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        // Update
        m_CameraController.OnUpdate(ts);

        // Render
        Hazel::Renderer2D::ResetStats();//������Ⱦ��ͳ������
        {
            HZ_PROFILE_SCOPE("Renderer Prep");
            m_Framebuffer->Bind();//��֡������
            Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//{ 0.1f, 0.1f, 0.1f, 1 }ָ����RGBA����������͸���ȣ���Χ��0~1
            Hazel::RenderCommand::Clear();//�����ɫ����������Ȼ�����
        }

        {
            static float rotation = 0.0f;//��ת�Ƕ�
            rotation += ts * 50.0f;

            HZ_PROFILE_SCOPE("Renderer Draw");
            Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());//��ʼ��Ⱦ����
            Hazel::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });//������ת���Σ�λ�ã���С����ת�Ƕȣ���ɫ
            Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
            Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
            Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
            Hazel::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20.0f);
            Hazel::Renderer2D::EndScene();//������Ⱦ����

            Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());//��ʼ��Ⱦ����
            for (float y = -0.5f; y < 5.0f; y += 0.5f)
            {
                for (float x = -0.5f; x < 5.0f; x += 0.5f)
                {
                    glm::vec4 color = { (x + 0.5f) / 5.0f, 0.4f, (y + 0.5f) / 5.0f, 0.5f };//��ɫ
                    Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);//���ƾ��Σ�λ�ã���С����ɫ
                }
            }
            Hazel::Renderer2D::EndScene();//������Ⱦ����
            m_Framebuffer->Unbind();//���֡������
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
                //����ȫ�����������ƶ����������ڵ�ǰ�棬
                //���û�и���ϸ�Ĵ������/z���ƣ����������޷�������
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
                if (ImGui::MenuItem("Exit")) Hazel::Application::Get().Close();//�˳�
                ImGui::EndMenu();//�����˵�
            }
            ImGui::EndMenuBar();//�����˵���
        }

        ImGui::Begin("Settings");//��ʼ����

        auto stats = Hazel::Renderer2D::GetStats();//��ȡ��Ⱦ��ͳ������
        //��ʾ��Ⱦ��ͳ������
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));//�༭��ɫ

        ImGui::End();//��������

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });//���ô������
        ImGui::Begin("Viewport");//��ʼ�ӿ�
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//��ȡ�ӿ�����С
        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))//����ӿڴ�С�ı�
        {
            m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);//����֡��������С
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };//�����ӿڴ�С
            m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);//���������������С
        }

        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//��ȡ����ID
        ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();
        ImGui::PopStyleVar();//�����������

        ImGui::End();

    }

    void EditorLayer::OnEvent(Hazel::Event& e)
    {
        m_CameraController.OnEvent(e);
    }

}