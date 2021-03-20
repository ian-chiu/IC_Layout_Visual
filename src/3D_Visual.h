#pragma once

#include "Engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <gl/GL.h>
#include <array>

// #include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
        // -----------Point lights-----------
        PointLight pointLightBase{
            { 0.0f, 0.0f, 0.0f },
            { 0.2f, 0.2f, 0.2f },
            { 0.7f, 0.7f, 0.7f },
            { 1.0f, 1.0f, 1.0f }
        };
        for (PointLight &pointLight : m_PointLights) {
            pointLight = pointLightBase;
        }
        m_PointLights[0].Position = glm::vec3( 0.0f,  0.0f,  15.0f);
        m_PointLights[1].Position = glm::vec3( -15.0f, -15.0f, 15.0f);
        m_PointLights[2].Position = glm::vec3( 15.0f,  -15.0f, 15.0f);
        m_PointLights[3].Position = glm::vec3( 0.0f,  0.0f, -15.0f);

        //----------------------read data---------------------
		std::ifstream inputFile("../../data/circuit3.cut");
		if (!inputFile) 
		{
			std::cout << "Cannot open input file.\n"; 
			exit(EXIT_FAILURE);
		}

		std::string line;

		while (std::getline(inputFile, line))
		{
			std::istringstream iss{ line };
			std::string token;
			iss >> token;
			if (token[0] == ';')
				continue;
			else if (token == "0")
			{
				glm::vec2 bl, tr;
				iss >> bl.x;
				iss >> bl.y;
				iss >> tr.x;
				iss >> tr.y;
				m_ChipBoundary.reset(bl, tr);
			}
			else 
			{
				Polygon polygon;
				glm::vec2 bl, tr;
				iss >> bl.x;
				iss >> bl.y;
				iss >> tr.x;
				iss >> tr.y;
				polygon.Boundary.reset(bl, tr);
				iss >> polygon.NetId;
				iss >> polygon.LayerId;
				iss >> polygon.PolyType;
				m_Polygons.push_back(polygon);
			}
		}

		// ------------------CUBE------------------------
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

	virtual void OnAttach() override 
	{
		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer.reset(new Framebuffer(fbSpec));
	}

	virtual void OnUpdate(Timestep ts) override
	{
        m_Framebuffer->Bind();
		m_CameraController.OnUpdate(ts);

        // --------------render----------------------
		RenderCommand::SetClearColor(glm::vec4(m_DirLight.Diffuse, 1));
        RenderCommand::Clear();

        Renderer::BeginScene(m_CameraController.GetCamera());

        m_CubeShader->Bind();
		m_CubeShader->UploadUniformFloat3("u_CameraPosition", m_CameraController.GetCamera().GetPosition());
		m_CubeShader->UploadMaterial(m_Material);
        m_CubeShader->UploadDirLight(m_DirLight);
        for (int i = 0; i < m_PointLights.size(); i++)
            m_CubeShader->UploadPointLight(i, m_PointLights[i]);

		for (const auto &polygon : m_Polygons)
		{
			// float translateY = polygon.LayerId / 100.0f;
			// float translateX = (polygon.Boundary.BottomLeft.x + polygon.Boundary.TopRight.x - 2 * m_ChipBoundary.BottomLeft.x) / 2.0f / m_ChipBoundary.Width;
			// float translateZ = (polygon.Boundary.BottomLeft.y + polygon.Boundary.TopRight.y - 2 * m_ChipBoundary.TopRight.y) / 2.0f / m_ChipBoundary.Height;
			glm::vec3 minp{
				(polygon.Boundary.BottomLeft.x - m_ChipBoundary.BottomLeft.x) / m_ChipBoundary.Width * m_WorldScale, 
				(polygon.LayerId / 100.0f - 0.0003f / 2.0f) * m_WorldScale, 
				(polygon.Boundary.BottomLeft.y - m_ChipBoundary.BottomLeft.y) / m_ChipBoundary.Height * m_WorldScale
			};
			glm::vec3 maxp{
				(polygon.Boundary.TopRight.x - m_ChipBoundary.BottomLeft.x) / m_ChipBoundary.Width * m_WorldScale, 
				(polygon.LayerId / 100.0f + 0.0003f / 2.0f) * m_WorldScale, 
				(polygon.Boundary.TopRight.y - m_ChipBoundary.BottomLeft.y) / m_ChipBoundary.Height * m_WorldScale
			};
			Frustum currentFrustum{m_CameraController.GetCamera().GetViewProjectionMatrix()};
			if (currentFrustum.IsBoxVisible(minp, maxp))
			{
				// LOG_INFO("BOX VISIBLE!");
				glm::mat4 transform{1.0f};
				// transform = glm::scale(transform, glm::vec3(m_WorldScale));
				transform = glm::translate(transform, (maxp + minp) / 2.0f);
				transform = glm::scale(transform, maxp - minp);
				Renderer::Submit(m_CubeShader, m_CubeVA, transform);
			}
		}

        Renderer::EndScene();
		m_Framebuffer->Unbind();
	}

	virtual void OnEvent(Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

    virtual void OnImGuiRender() override
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Settings");
		float speed = m_CameraController.GetTranslationSpeed();
		if (ImGui::DragFloat("Camera Speed", &speed, 1.0f, 1.0f, 100000.0f))
		{
			m_CameraController.SetTranslationSpeed(speed);
		}
		if (ImGui::DragFloat("World Scale", &m_WorldScale, 1.0f, 0.1f, 100000.0f))
		{
			// m_Camera.setFar(m_WorldScale / 2.0f);
		}
		float far = m_CameraController.GetCamera().GetFar();
		if (ImGui::DragFloat("Far", &far, 1.0f, 0.1f, 100000.0f))
		{
			m_CameraController.GetCamera().setFar(far);
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
		}
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::PopStyleVar();
		ImGui::End();

		ImGui::End();
	}

private:
	Ref<Framebuffer> m_Framebuffer;
	PerspectiveCameraController m_CameraController;
	glm::vec2 m_ViewportSize;

	Ref<VertexArray> m_CubeVA;
    Ref<Shader> m_CubeShader;
	Material m_Material{
		{ 201.0f / 255.0f, 159.0f / 255.0f, 105.0f / 255.0f },
		{ 201.0f / 255.0f, 159.0f / 255.0f, 105.0f / 255.0f },
		{ 0.5f, 0.5f, 0.5f },
		8.0f
	};

    DirLight m_DirLight{
        { -0.2f, -1.0f, -0.3f },
		{ 0.2f, 0.2f, 0.2f },
		{ 0.7f, 0.7f, 0.7f },
		{ 1.0f, 1.0f, 1.0f }
    };

    std::array<PointLight, 4> m_PointLights;

	struct Boundary
	{
		Boundary() = default;
		Boundary(glm::vec2 bl, glm::vec2 tr)
			: BottomLeft(bl), TopRight(tr), Width(tr.x - bl.x), Height(tr.y - bl.y)
		{
		}
		void reset(glm::vec2 bl, glm::vec2 tr)
		{
			BottomLeft = bl;
			TopRight = tr;
			Width = tr.x - bl.x;
			Height = tr.y - bl.y;
		}
		glm::vec2 BottomLeft;
		glm::vec2 TopRight;
		float Width;
		float Height;
	};
	struct Polygon
	{
		Boundary Boundary;
		int NetId;
		int LayerId;
		std::string PolyType;
	};
	std::vector<Polygon> m_Polygons;
	Boundary m_ChipBoundary;
	float m_WorldScale = 100.0f;
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