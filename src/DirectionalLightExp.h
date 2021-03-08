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
        //----------------------CUBE---------------------
		m_CubeVA.reset(new VertexArray);
		float vertices[] = {
			// Position             // normal
			// SOUTH                
			-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,

			// NORTH
			-0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,

			// WEST
			-0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,

			// EAST
			 0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,

			// TOP
			-0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,

			// BOTTOM
			-0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,
			
		};
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal"}
		};
		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(new VertexBuffer{vertices, sizeof(vertices)});
		vertexBuffer->SetLayout(layout);
		m_CubeVA->AddVertexBuffer(vertexBuffer);

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
		m_CubeVA->SetIndexBuffer(indexBuffer);
		m_CubeShader.reset(new Shader{"./assets/shader/Flat.glsl"});

		RenderCommand::Init();
	}

	virtual void OnUpdate(Timestep ts) override
	{
        // ---------------input----------------------
		processCameraControlInput(ts);

        // --------------render----------------------
		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();

        Renderer::BeginScene(m_Camera);
		m_Camera.SetPosition(m_CameraPosition);

        m_CubeShader->Bind();
		m_CubeShader->UploadUniformFloat3("u_CameraPosition", m_CameraPosition);
		m_CubeShader->UploadMaterial(m_Material);
		m_CubeShader->UploadDirLight(m_DirLight);
		for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, m_CubePositions[i]);
            float angle = 20.0f * i; 
            transform = glm::rotate(transform, (float)glfwGetTime() + glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            Renderer::Submit(m_CubeShader, m_CubeVA, transform);
        }

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

    virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::DragFloat("Camera Speed", &m_CameraMoveSpeed, m_CameraMoveSpeed);
		ImGui::End();
	}

private:
    void processCameraControlInput(Timestep ts) 
    {
        if (Input::IsKeyPressed(KEY_W)) 
			m_CameraPosition += (ts * m_CameraMoveSpeed) * m_Camera.GetFront();
		else if (Input::IsKeyPressed(KEY_S)) 
			m_CameraPosition -= (ts * m_CameraMoveSpeed) * m_Camera.GetFront();

		if (Input::IsKeyPressed(KEY_A)) 
			m_CameraPosition -= glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) * (ts * m_CameraMoveSpeed);
		else if (Input::IsKeyPressed(KEY_D)) 
			m_CameraPosition += glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp())) * (ts * m_CameraMoveSpeed);

        if (Input::IsKeyPressed(KEY_SPACE) || Input::IsKeyPressed(KEY_LEFT_SHIFT)) 
			m_CameraPosition += m_Camera.GetUp() * (ts * m_CameraMoveSpeed);
		else if (Input::IsKeyPressed(KEY_LEFT_CONTROL)) 
			m_CameraPosition -= m_Camera.GetUp() * (ts * m_CameraMoveSpeed);

		if (Input::IsKeyPressed(KEY_UP) )
			m_Camera.AddPitch(m_CameraRotateSpeed * ts);
		else if (Input::IsKeyPressed(KEY_DOWN))
			m_Camera.AddPitch(-m_CameraRotateSpeed * ts);

		if (Input::IsKeyPressed(KEY_RIGHT))
			m_Camera.AddYaw(m_CameraRotateSpeed * ts);
		else if (Input::IsKeyPressed(KEY_LEFT))
			m_Camera.AddYaw(-m_CameraRotateSpeed * ts);

		if (Input::IsKeyPressed(KEY_KP_ADD) && m_CameraMoveSpeed <= 50.0f)
			m_CameraMoveSpeed += 0.1f;
		else if (Input::IsKeyPressed(KEY_KP_SUBTRACT) && m_CameraMoveSpeed >= 1.0f)
			m_CameraMoveSpeed -= 0.1f;
    };

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

private:
	Ref<VertexArray> m_CubeVA;
    Ref<Shader> m_CubeShader;
	Material m_Material{
		{ 1.0f, 0.5f, 0.31f },
		{ 1.0f, 0.5f, 0.31f },
		{ 0.8f, 0.8f, 0.8f },
		32.0f
	};

	DirLight m_DirLight{
		{ -0.2f, -1.0f, -0.3f },
		{ 0.2f, 0.2f, 0.2f },
		{ 0.5f, 0.5f, 0.5f },
		{ 1.0f, 1.0f, 1.0f }
	};
	

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