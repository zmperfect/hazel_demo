#include "hzpch.h"
#include "Platform/Windows/WindowsWindow.h"

#include "Hazel/Core/Input.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/KeyEvent.h"

#include "Hazel/Renderer/Renderer.h"

#include <Platform/OpenGL/OpenGLContext.h>

namespace Hazel {

	static uint8_t s_GLFWWindowCount = 0;//����ȷ��GLFWֻ��ʼ��һ��

	//�򵥵��쳣��������glfwSetErrorCallback()
	static void GLFWErrorCallback(int error, const char* description)
	{
		HZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps & props)
	{
		return CreateScope<WindowsWindow>(props);
	}

	//���캯������Щ�����ģ�������׼��Init
	WindowsWindow::WindowsWindow(const WindowProps & props)
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		//���� ���������Ĳ���
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		//Logһ�£������ʲô����
		HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		// Make sure to initialize GLFW only once, along with the first window
		if (s_GLFWWindowCount == 0)
		{
			HZ_PROFILE_SCOPE("glfwInit");//����һ�����Լ�ʱ��

			//���GLFWû�г�ʼ��������������ʼ��
			int success = glfwInit();
			//�¸����ԣ�ȷ����ĳ�ʼ���ˣ�֮����Ҫ���и�ֵ�ٲ���Ե�ԭ������Ϊ
			//���ֱ�Ӷ���glfwInit()����Release����Ship Build�У�������Եĺ����ֱ�ӱ�ignore��
			//�����������İ汾����������г�ʼ����
			HZ_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			HZ_PROFILE_SCOPE("glfwCreateWindow");//����һ�����Լ�ʱ����ΪglfwCreateWindow

			#if defined(HZ_DEBUG)
				//�����Debug Build���Ϳ���OpenGL Debug Context
                if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
                    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			#endif

			//�������Ϣ�Ѿ���ȫ����glfwCreateWindow����һ��glfwWindow������ָ��
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}
		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVsync(true);

		// Set GLFW callbacks
		//���ﴫ��һ��Lambda���ʽ�հ�������ԭ������һ����Ӧ�¼���Ȼ��set��callback event����ͬ
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(static_cast<KeyCode>(key));
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), 1);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(static_cast<KeyCode>(keycode));//
				data.EventCallback(event);
			});


		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(static_cast<MouseCode>(button));
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		//ÿ��updateʱ������ǰ�ڶ����е��¼�
		glfwPollEvents();
		//ˢ����һ֡(�ϸ���˵�ǰ�Framebuffer��̨֡����ǰ̨����Framebuffer��ǰ֡������̨��
		//������Swap)
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVsync(bool enabled)
	{
		HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

		// If w/out brace, only 1 closest line will be executed
		// �����1/0�����ǿ�/�ص���˼��������һ֡Swap֮��Ҫ�ȶ��ٴ�Screen Update������
		// ����glfwSwapInterval�ڲ���װ�˶�Ӧƽ̨��vsync���������Ի����Ͼ��ǿ��ص���˼������
		// Ҫע����ǣ����ǿ��԰����interval����10��Ȼ���Ӵ���������޿�������ȻVSync on
		// �ɼ���VSync��ʵ����Ⱦ��֡��û���κι�ϵ��VSync������60ֻ֡����Ϊ�󲿷���ʾ��ˢ����60HZ������Sync����
		if (enabled)
			glfwSwapInterval(1);
		else 
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVsync() const
	{
		return m_Data.VSync;
	}
}