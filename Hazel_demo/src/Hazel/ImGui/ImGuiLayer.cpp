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
        HZ_PROFILE_FUNCTION();//获取函数签名

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
		//在1.6版本后，显式创建Imgui上下文是必须的，这用于更好的管理Imgui的资源
		ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);//从文件添加粗体
        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);//设置默认字体

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

        SetDarkThemeColors();//设置暗色主题颜色

        Application& app = Application::Get();//获取应用
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());//获取窗口

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);//初始化GLFW的Implementation

		//初始化OpenGL3的Implementation，根据版本号来决定哪些功能可以被开放，详情可以看imgui源代码
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
        HZ_PROFILE_FUNCTION();//获取函数签名

        ImGui_ImplOpenGL3_Shutdown();//关闭OpenGL3的Implementation
        ImGui_ImplGlfw_Shutdown();//关闭GLFW的Implementation
        ImGui::DestroyContext();//销毁Imgui上下文
	}

    void ImGuiLayer::OnEvent(Event& e)
    {
        //处理事件
        if (m_BlockEvents)
        {
        ImGuiIO& io = ImGui::GetIO();//获取Imgui的IO
        e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;//判断是否在鼠标事件中，且Imgui是否需要捕获鼠标
        e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;//判断是否在键盘事件中，且Imgui是否需要捕获键盘
        }
    }

	void ImGuiLayer::Begin()
	{
        HZ_PROFILE_FUNCTION();//获取函数签名

        ImGui_ImplOpenGL3_NewFrame();//初始化OpenGL3的Implementation
        ImGui_ImplGlfw_NewFrame();//初始化GLFW的Implementation
        ImGui::NewFrame();//初始化Imgui
	}

    void ImGuiLayer::End()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

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

    void ImGuiLayer::SetDarkThemeColors()
    {
        auto colors = ImGui::GetStyle().Colors;//获取颜色
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };//设置窗口背景颜色

        // Headers
        colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };//设置头部颜色
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };//设置头部悬停颜色
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };//设置头部激活颜色

        // Buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };//设置按钮颜色
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };//设置按钮悬停颜色
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };//设置按钮激活颜色

        // Frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };//设置框架背景颜色
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };//设置框架背景悬停颜色
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };//设置框架背景激活颜色

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };//设置标签颜色
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };//设置标签悬停颜色
        colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };//设置标签激活颜色

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };//设置标题背景颜色
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };//设置标题背景激活颜色
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };//设置标题背景折叠颜色
    }

}