#pragma once

#include "Engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <gl/GL.h>

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(45.0f, 1280.0f / 720.0f)
	{
		m_VertexArray.reset(new VertexArray);

		float vertices[] = {
			// position              // texture coord
			// SOUTH
			-0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,     1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,     1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,     0.0f, 1.0f,

			// NORTH
			-0.5f, -0.5f, 0.5f,      0.0f, 0.0f,
			 0.5f, -0.5f, 0.5f,      1.0f, 0.0f,
			 0.5f,  0.5f, 0.5f,      1.0f, 1.0f,
			-0.5f,  0.5f, 0.5f,      0.0f, 1.0f,

			// WEST
			-0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,     1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,     1.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,     0.0f, 1.0f,

			// EAST
			 0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,     1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,     1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,     0.0f, 1.0f,

			// TOP
			-0.5f,  0.5f, -0.5f,     0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,     0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,     1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,     1.0f, 0.0f,

			// BOTTOM
			-0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,     0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,     1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,     1.0f, 0.0f,
			
		};
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(new VertexBuffer{vertices, sizeof(vertices)});
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[] = {
			// SOUTH
			0, 1, 2,
			0, 2, 3,

			// NORTH
			4, 5, 6,
			4, 6, 7,

			// EAST
			8, 9, 10,
			8, 10, 11,

			// WEST
			12, 13, 14,
			12, 14, 15,

			// TOP
			16, 17, 18,
			16, 18, 19,

			// BOTTOM
			20, 21, 22,
			20, 22, 23
		};
		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(new IndexBuffer{indices, sizeof(indices) / sizeof(unsigned int)});
		m_VertexArray->SetIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			out vec2 v_TexCoord;

			uniform mat4 u_Transform;
			uniform mat4 u_ViewProjection;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			out vec4 fragColor;

			in vec2 v_TexCoord;

			uniform vec4 u_Color;
			uniform sampler2D texture1;

			void main()
			{
				fragColor = texture(texture1, v_TexCoord);
			}
		)";

		m_Shader.reset(new Shader{vertexSrc, fragmentSrc});
		m_Shader->Bind();

		Texture texture1("image/container.jpg"); // auto to slot 0
		m_Shader->UploadUniformInt("u_Texture", 0);

		m_CoordVA.reset(new VertexArray);

		float coordVertices[] = {
			// origin
			0.0f, 0.0f, 0.0f,	1.0f ,1.0f, 1.0f, 1.0f,
			// 0.0f, 0.0f, 0.0f,	0.0f ,1.0f, 0.0f, 1.0f,
			// 0.0f, 0.0f, 0.0f, 	0.0f ,0.0f, 1.0f, 1.0f,

			// x, y, z
			3.0f, 0.0f, 0.0f,	1.0f ,0.0f, 0.0f, 1.0f,
			0.0f, 3.0f, 0.0f,	0.0f ,1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 3.0f, 	0.0f ,0.0f, 1.0f, 1.0f,
		};
		
		layout = {
			{ShaderDataType::Float3, "a_Pos"},
			{ShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer.reset(new VertexBuffer(coordVertices, sizeof(coordVertices)));
		vertexBuffer->SetLayout(layout);
		m_CoordVA->AddVertexBuffer(vertexBuffer);

		unsigned int coordIndices[] = {
			0, 1, 
			1, 0,
			0, 2,
			2, 0, 
			0, 3,
			3, 0,
			0, 1
		};
		indexBuffer.reset(new IndexBuffer{indices, sizeof(coordIndices) / sizeof(unsigned int)});
		m_CoordVA->SetIndexBuffer(indexBuffer);
		
		vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Pos;
			layout(location = 1) in vec4 a_Color;

			out vec4 v_Color;

			uniform mat4 u_Transform;
			uniform mat4 u_ViewProjection;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Pos, 1.0f);
			}
		)";

		fragmentSrc = R"(
			#version 330 core
			out vec4 fragColor;

			in vec4 v_Color;

			void main()
			{
				fragColor = v_Color;
			}
		)"; 

		m_CoordShader.reset(new Shader{vertexSrc, fragmentSrc});
		m_CoordShader->Bind();

		RenderCommand::Init();
	}

	virtual void OnUpdate(Timestep ts) override
	{
		if (Input::IsKeyPressed(KEY_W)) 
			m_CameraPosition += (ts * m_CameraMoveSpeed) * m_Camera.GetFront();
		else if (Input::IsKeyPressed(KEY_S)) 
			m_CameraPosition -= (ts * m_CameraMoveSpeed) * m_Camera.GetFront();

		if (Input::IsKeyPressed(KEY_A)) 
			m_CameraPosition -= glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) * (ts * m_CameraMoveSpeed);
		else if (Input::IsKeyPressed(KEY_D)) 
			m_CameraPosition += glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) * (ts * m_CameraMoveSpeed);

		if (Input::IsKeyPressed(KEY_UP))
			m_Camera.AddPitch(m_CameraRotateSpeed * ts);
		else if (Input::IsKeyPressed(KEY_DOWN))
			m_Camera.AddPitch(-m_CameraRotateSpeed * ts);

		if (Input::IsKeyPressed(KEY_RIGHT))
			m_Camera.AddYaw(m_CameraRotateSpeed * ts);
		else if (Input::IsKeyPressed(KEY_LEFT))
			m_Camera.AddYaw(-m_CameraRotateSpeed * ts);

		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();

        Renderer::BeginScene(m_Camera);

		m_Camera.SetPosition(m_CameraPosition);

		// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		m_Shader->Bind();
		Texture texture1("image/container.jpg"); // auto to slot 0
		m_Shader->UploadUniformInt("u_Texture", 0);
		for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, m_CubePositions[i]);
            float angle = 20.0f * i; 
            transform = glm::rotate(transform, (float)glfwGetTime() + glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            Renderer::Submit(m_Shader, m_VertexArray, transform);
        }
		m_Shader->UnBind();

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		m_CoordShader->Bind();
		m_CoordVA->Bind();
		Renderer::Submit(m_CoordShader, m_CoordVA, glm::mat4(1.0f), GL_LINE_LOOP);
		m_CoordShader->UnBind();

        Renderer::EndScene();
	}

	virtual void OnEvent(Event& e) override
	{
		EventDispatcher dispatcher(e);
		if (m_MouseNavMode)
		{
			dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(ExampleLayer::onMouseMoved));
			dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(ExampleLayer::onMouseScrolled));
		}
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ExampleLayer::onKeyPressed));
	}

	bool onMouseMoved(const MouseMovedEvent &e) 
	{
		float xoffset = e.GetX() - m_LastCamDirectionX;
		float yoffset = m_LastCamDirectionY - e.GetY();

		m_LastCamDirectionX = e.GetX();
		m_LastCamDirectionY = e.GetY();

		const float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		m_Camera.AddYaw(xoffset);
		m_Camera.AddPitch(yoffset);

		return true;
	}

	bool onMouseScrolled(const MouseScrolledEvent &e)
	{
		float yoffset = e.GetYOffset();
		const float sensitivity = 0.1f;
		yoffset *= sensitivity;
		m_Camera.AddFOV(-yoffset);
		// LOG_INFO("Camera FOV: {0}", m_Camera.GetFOV());
		return true;
	}

	bool onKeyPressed(const KeyPressedEvent &e)
	{
		if (e.GetKeyCode() == KEY_F1)
		{
			if (!m_MouseNavMode) {
				glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			m_MouseNavMode = !m_MouseNavMode;

			return true;
		}
		return false;
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::DragFloat("Camera Speed", &m_CameraMoveSpeed, m_CameraMoveSpeed);
		ImGui::End();
	}

private:
	Ref<VertexArray> m_VertexArray;
    Ref<Shader> m_Shader;

	Ref<VertexArray> m_CoordVA;
	Ref<Shader> m_CoordShader;

    PerspectiveCamera m_Camera;
	glm::vec3 m_CameraPosition{0.0f, 0.0f, 3.0f};
	float m_CameraMoveSpeed = 10.0f;
	float m_CameraRotateSpeed = 180.0f;
	float m_LastCamDirectionX{ 0.0f };
	float m_LastCamDirectionY{ 0.0f };
	bool m_MouseNavMode = false;

	std::vector<glm::vec3> m_CubePositions{
		glm::vec3( 0.0f,  0.0f,  0.0f), 
		glm::vec3( 2.0f,  5.0f, -15.0f), 
		glm::vec3(-1.5f, -2.2f, -2.5f),  
		glm::vec3(-3.8f, -2.0f, -12.3f),  
		glm::vec3( 2.4f, -0.4f, -3.5f),  
		glm::vec3(-1.7f,  3.0f, -7.5f),  
		glm::vec3( 1.3f, -2.0f, -2.5f),  
		glm::vec3( 1.5f,  2.0f, -2.5f), 
		glm::vec3( 1.5f,  0.2f, -1.5f), 
		glm::vec3(-1.3f,  1.0f, -1.5f)  
	};
};

class Sandbox : public Application
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