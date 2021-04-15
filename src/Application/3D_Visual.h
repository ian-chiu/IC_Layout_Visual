// create data base file in another thread and logging window
// subwindow 2d
#pragma once

#include "Engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <gl/GL.h>
#include <array>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include <future>

#include "FrustumCull.h"
#include "LayoutDatabase.h"

#include "nfd.h"

struct ExampleAppLog
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            ImGuiListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
};


class ExampleLayer : public Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
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
		while (!m_Futures.empty() && is_ready(m_Futures.front())) 
		{
			if (m_Futures.front().get())
				m_Log.AddLog("Successfully write the layout to the database.\n\n");
			else 
				m_Log.AddLog("Cannot write the layout to database!\n\n");
            m_Futures.pop_back();
        }
        m_Framebuffer->Bind();
		m_CameraController.OnUpdate(ts, m_CameraMode);
		m_DirLight.Direction = m_CameraController.GetCamera().GetFront();

        // --------------render----------------------
		RenderCommand::SetClearColor(glm::vec4(m_DirLight.Diffuse, 1));
        RenderCommand::Clear();

        Renderer::BeginScene(m_CameraController.GetCamera());

        m_CubeShader->Bind();
		m_CubeShader->UploadUniformFloat3("u_CameraPosition", m_CameraController.GetCamera().GetPosition());
        m_CubeShader->UploadDirLight(m_DirLight);
		m_CubeShader->UploadMaterial(m_Material);

		int currentLayerId = 0;
		for (const auto &polygon : m_Polygons)
		{
			if (currentLayerId != polygon.LayerId) 
			{
				m_CubeShader->UploadMaterial(m_LayerColors[polygon.LayerId - 1]);
				currentLayerId = polygon.LayerId;
			}
			
			glm::vec3 minp{
				(polygon.Boundary.BottomLeft.x - m_ChipBoundary.BottomLeft.x) / m_ChipBoundary.Max * m_WorldScale, 
				(polygon.LayerId / 100.0f - 0.0003 / 2.0f) * m_WorldScale, 
				(polygon.Boundary.BottomLeft.y - m_ChipBoundary.BottomLeft.y) / m_ChipBoundary.Max * m_WorldScale
			};
			glm::vec3 maxp{
				(polygon.Boundary.TopRight.x - m_ChipBoundary.BottomLeft.x) / m_ChipBoundary.Max * m_WorldScale, 
				(polygon.LayerId / 100.0f + 0.0003 / 2.0f) * m_WorldScale, 
				(polygon.Boundary.TopRight.y - m_ChipBoundary.BottomLeft.y) / m_ChipBoundary.Max * m_WorldScale
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
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ExampleLayer::onKeyPressed));
		if (m_CameraMode)
			m_CameraController.OnEvent(e);
	}

    virtual void OnImGuiRender() override
	{
		static bool settingsOpen = true;
		static bool viewportOpen = true;
		static bool dockspaceOpen = true;
		static bool logOpen = true;
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
				if (ImGui::MenuItem("Import"))
				{
					nfdchar_t *nfd_file_path = NULL;
					nfdresult_t result = NFD_OpenDialog( "db", NULL, &nfd_file_path );
					if ( result == NFD_OKAY )
					{
						fs::path db_path{nfd_file_path};
						std::string db_name = db_path.stem().string() + ".db";
						m_Database.Connect(db_name);
						m_Database.GetAllPolygons(m_Polygons);
						m_Database.GetChipBoundary(m_ChipBoundary);
						glm::vec3 cameraPosition = glm::vec3(m_ChipBoundary.Width / m_ChipBoundary.Max / 2.0f, 0.15, m_ChipBoundary.Height / m_ChipBoundary.Max / 2.0f) * m_WorldScale;
						m_CameraController.SetCameraPosition(cameraPosition);
						m_CameraController.GetCamera().SetPitch(-90.0f);
						m_CameraController.GetCamera().SetYaw(0.0f);
						m_LayerIdSet.resize(m_Database.GetLayerCount(), true);
						m_LayerColors.resize(m_Database.GetLayerCount(), m_Material);

						for (int i = 0; i < m_LayerColors.size(); i++) {
							m_LayerColors[i].Diffuse = m_DefaultColors[i % m_LayerColors.size()];
							m_LayerColors[i].Ambient = m_DefaultColors[i % m_LayerColors.size()];
						}
					}
					else if ( result == NFD_CANCEL )
					{
						// puts("User pressed cancel.");
					}
					else 
					{
						printf("Error: %s\n", NFD_GetError() );
						std::exit(-1);
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Settings"))
					settingsOpen = true;
				if (ImGui::MenuItem("Viewport"))
					viewportOpen = true;
				if (ImGui::MenuItem("Log"))
					logOpen = true;
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Create Database from Layout"))
				{
					nfdchar_t *nfd_file_path = NULL;
					nfdresult_t result = NFD_OpenDialog( "cut", NULL, &nfd_file_path );
					if ( result == NFD_OKAY )
					{
						fs::path input_file_path{nfd_file_path};
						std::string db_name = input_file_path.stem().string() + ".db";
						if (!m_Database.Exists(db_name))
						{
							m_Log.AddLog("Create database %s...\n", db_name.c_str());
							m_Database.createDataBase(db_name);
						}
						else
						{
							m_Log.AddLog("%s is already existed! Rewrite data from the layout.\n", db_name.c_str());
							m_Database.Connect(db_name);
						}
						m_Log.AddLog("Write %s to database %s...\n", nfd_file_path, db_name.c_str());
						m_Futures.push_back(
							std::async(std::launch::async, &LayoutDatabase::WriteFromLayout, &m_Database, input_file_path)
						);
					}
					else if ( result == NFD_CANCEL )
					{
						// puts("User pressed cancel.");
					}
					else 
					{
						printf("Error: %s\n", NFD_GetError() );
						std::exit(-1);
					}
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		if (settingsOpen)
		{
			ImGui::Begin("Settings", &settingsOpen);
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
				m_CameraController.GetCamera().SetFar(far);
			}

			bool layerIdSet[256] = { false };
			float layerColors[256][3] = { 0.0f };
			for (int i = 0; i < m_LayerIdSet.size(); i++)
			{
				layerIdSet[i] = m_LayerIdSet[i];
				layerColors[i][0] = m_LayerColors[i].Diffuse.r;
				layerColors[i][1] = m_LayerColors[i].Diffuse.g;
				layerColors[i][2] = m_LayerColors[i].Diffuse.b;
			}
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Layers"))
			{
				for (int i = 0; i < m_LayerIdSet.size(); i++)
				{
					std::string label = "Layer " + std::to_string(i + 1);
					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					if (ImGui::TreeNode(label.c_str()))
					{
						if (ImGui::Checkbox("show", &layerIdSet[i]))
						{
							m_LayerIdSet[i] = layerIdSet[i];
							m_Database.FilterLayers(m_Polygons, m_LayerIdSet);
						}
						ImGui::SameLine();
						if (ImGui::ColorEdit3("Color", (float*)&layerColors[i], ImGuiColorEditFlags_NoInputs))
						{
							m_LayerColors[i].Diffuse.r = layerColors[i][0];
							m_LayerColors[i].Diffuse.g = layerColors[i][1];
							m_LayerColors[i].Diffuse.b = layerColors[i][2];
							m_LayerColors[i].Ambient.r = layerColors[i][0];
							m_LayerColors[i].Ambient.g = layerColors[i][1];
							m_LayerColors[i].Ambient.b = layerColors[i][2];
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			ImGui::End();
		}
		
		if (viewportOpen)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport", &viewportOpen);
			ImGui::PopStyleVar();
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
			{
				m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
				m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

				m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
			}
			uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			
			ImGui::End();
		}

		if (logOpen)
		{
			// For the demo: add a debug button _BEFORE_ the normal log window contents
			// We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
			// Most of the contents of the window will be added by the log.Draw() call.
			ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
			ImGui::Begin("Log", &logOpen);
			ImGui::End();

			// Actually call in the regular Log helper (which will Begin() into the same window as we just did)
			m_Log.Draw("Log", &logOpen);
		}
		
		// ImGui::ShowDemoWindow();

		ImGui::End();
	}
private:
	bool onKeyPressed(KeyPressedEvent &e)
	{
		if (e.GetKeyCode() == KEY_C)
		{
			m_CameraMode = !m_CameraMode;
		    if (m_CameraMode) {
		        glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		    }
		    else {
		        glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		    }
		    return true;
		}
		if (e.GetKeyCode() == KEY_ESCAPE)
		{
			m_CameraMode = false;
			glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			return true;
		}
		return false;
	}

	template<typename R>
	bool is_ready(std::future<R> const& f)
	{ 
		return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; 
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
		{ 0.3f, 0.3f, 0.3f },
		8.0f
	};

    DirLight m_DirLight{
        { -0.2f, -1.0f, -0.3f },
		{ 0.4f, 0.4f, 0.4f },
		{ 0.7f, 0.7f, 0.7f },
		{ 1.0f, 1.0f, 1.0f }
    };

	LayoutDatabase m_Database;
	std::vector<Polygon> m_Polygons;
	Boundary m_ChipBoundary;
	float m_WorldScale = 100.0f;
	std::vector<bool> m_LayerIdSet;
	std::vector<Material> m_LayerColors;

	bool m_CameraMode = false;
	ExampleAppLog m_Log{};

	std::vector<std::future<bool>> m_Futures{};
	std::vector<glm::vec3> m_DefaultColors{
		{ 44.0f / 255.0f, 60.0f / 255.0f, 102.0f / 255.0f },
		{ 198.0f / 255.0f, 144.0f / 255.0f, 102.0f / 255.0f },
		{ 163.0f / 255.0f, 75.0f / 255.0f, 75.0f / 255.0f },
		{ 5.0f / 255.0f, 128.0f / 255.0f, 19.0f / 255.0f },
		{ 198.0f / 255.0f, 74.0f / 255.0f, 247.0f / 255.0f },
		{ 204.0f / 255.0f, 172.0f / 255.0f, 90.0f / 255.0f },
		{ 37.0f / 255.0f, 184.0f / 255.0f, 199.0f / 255.0f },
		{ 106.0f / 255.0f, 196.0f / 255.0f, 71.0f / 255.0f },
		{ 201.0f / 255.0f, 95.0f / 255.0f, 125.0f / 255.0f },
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