#include "hzpch.h"
#include "Hazel/ImGui/ImGuiLayer.h"

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include "Hazel/Core/Application.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Hazel {
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::OnAttach()
	{
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
		//��1.6�汾����ʽ����Imgui�������Ǳ���ģ������ڸ��õĹ���Imgui����Դ
		ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);

		//��ʼ��OpenGL3��Implementation�����ݰ汾����������Щ���ܿ��Ա����ţ�������Կ�imguiԴ����
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        ImGui_ImplOpenGL3_Shutdown();//�ر�OpenGL3��Implementation
        ImGui_ImplGlfw_Shutdown();//�ر�GLFW��Implementation
        ImGui::DestroyContext();//����Imgui������
	}

	void ImGuiLayer::Begin()
	{
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        ImGui_ImplOpenGL3_NewFrame();//��ʼ��OpenGL3��Implementation
        ImGui_ImplGlfw_NewFrame();//��ʼ��GLFW��Implementation
        ImGui::NewFrame();//��ʼ��Imgui
	}

    void ImGuiLayer::End()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

}