#include <Hazel.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
        m_VertexArray.reset(Hazel::VertexArray::Create());//创建一个顶点数组

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,//顶点坐标，颜色
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,//顶点坐标，颜色
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f//顶点坐标，颜色
        };

        Hazel::Ref<Hazel::VertexBuffer> vertexBuffer;//顶点缓冲区
        vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));//创建一个顶点缓冲区
        Hazel::BufferLayout layout = {//创建一个布局
            { Hazel::ShaderDataType::Float3, "a_Position" },//顶点坐标
            { Hazel::ShaderDataType::Float4, "a_Color" }//顶点颜色
        };
        vertexBuffer->SetLayout(layout);//设置顶点缓冲区的布局
        m_VertexArray->AddVertexBuffer(vertexBuffer);//将顶点缓冲区添加到顶点数组中

        uint32_t indices[3] = { 0, 1, 2 };//索引
        Hazel::Ref<Hazel::IndexBuffer> indexBuffer;//索引缓冲区
        indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));//创建一个索引缓冲区
        m_VertexArray->SetIndexBuffer(indexBuffer);//设置顶点数组的索引缓冲区

        m_SquareVA.reset(Hazel::VertexArray::Create());//创建一个方形顶点数组

        float squareVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,//顶点坐标，纹理坐标
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,//顶点坐标，纹理坐标
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,//顶点坐标，纹理坐标
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f//顶点坐标，纹理坐标
        };

        Hazel::Ref<Hazel::VertexBuffer> squareVB;
        squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({
            { Hazel::ShaderDataType::Float3, "a_Position" },//顶点坐标
            { Hazel::ShaderDataType::Float2, "a_TexCoord" }//纹理坐标
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        Hazel::Ref<Hazel::IndexBuffer> squareIB;
        squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);

        //存储顶点着色器代码
        std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		//片段着色器代码
        std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

        m_Shader = Hazel::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);//创建一个包含位置颜色的着色器

        //shader
        std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		//片段着色器代码
        std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

        m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);//创建一个纯色着色器

        auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

        m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
        m_LJRLogoTexture = Hazel::Texture2D::Create("assets/textures/LJRLogo.png");

        std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
        std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

	}

	void OnUpdate(Hazel::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		//渲染出图
        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });//设置清屏颜色
        Hazel::RenderCommand::Clear();//清屏

        Hazel::Renderer::BeginScene(m_CameraController.GetCamera());//开始渲染场景

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));//缩放

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();//绑定shader
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);//上传颜色

		//渲染出20*20个方形
		for(int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		//绑定纹理与提交
        m_Texture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_LJRLogoTexture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//三角形提交
		//Hazel::Renderer::Submit(m_Shader, m_VertexArray);//提交顶点数组

		Hazel::Renderer::EndScene();//结束渲染场景

	}

	//渲染GUI
	virtual void OnImGuiRender() override
	{
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));//设置方形颜色
        ImGui::End();
	}

	void OnEvent(Hazel::Event& e) override
	{
        m_CameraController.OnEvent(e);//相机控制器事件
	}
private:
	Hazel::ShaderLibrary m_ShaderLibrary;//着色器库
    Hazel::Ref<Hazel::Shader> m_Shader;//shader
    Hazel::Ref<Hazel::VertexArray> m_VertexArray;//顶点数组

    Hazel::Ref<Hazel::Shader> m_FlatColorShader;//纯色shader
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;//方形顶点数组

	Hazel::Ref<Hazel::Texture2D> m_Texture, m_LJRLogoTexture;//纹理,logo纹理

	Hazel::OrthographicCameraController m_CameraController;//相机控制器
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };//方形颜色
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}