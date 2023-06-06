#include "hzpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"
#include "GLFW/glfw3.h"

#include "Hazel/Application.h"

namespace Hazel {
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		//在1.6版本后，显式创建Imgui上下文是必须的，这用于更好的管理Imgui的资源
		ImGui::CreateContext();
		//设置一下主题风格
		ImGui::StyleColorsDark();

		//建立上下文所必须的前置条件，负责获得Imgui上下文和系统之间I/O的接口
		ImGuiIO& io = ImGui::GetIO();
		//这里主要根据实际API的支持来暴露功能，这里暴露了两个功能，可以获得平台鼠标，以及可以操作鼠标位置
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		// TEMP
		// 这里临时是用了GLFW的KeyCode，这一部分会单独放进一个KeyCode文件中，并且用我们自己的KeyCode来实现跨平台
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		//初始化OpenGL3的Implementation，根据版本号来决定哪些功能可以被开放，详情可以看imgui源代码
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{

	}

	void ImGuiLayer::OnUpdate()
	{
		// CreateContext只需要做一次，就可以赋予给一个全局的单例，所以此时已经有了Context
		// IO还是要重新Get，本质上是返回了全局的Context指针所指的Context中的IO接口
		ImGuiIO& io = ImGui::GetIO();

		// 获取Application，为了拿到后面的窗口大小
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		// 这里就完全是临时的Hack，因为此时还没有做Time系统和DeltaTime步进，所以强行Quick & Dirty的给做了一个DeltaTime
		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		// 这里开始，标记了Render的内容，NewFrame提供了需要Render前的一切基本前置需求，在此之后可以任意Submit指令，直到下一个Render会将其Render出来。
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		// 将这个Demo Window渲染出来，用OpenGL的实现方式
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::OnEvent(Event& event)
	{

	}

}