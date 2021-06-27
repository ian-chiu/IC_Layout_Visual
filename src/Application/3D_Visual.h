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
	ExampleLayer();
	virtual void OnAttach() override; 
	virtual void OnUpdate(Timestep ts) override;
	virtual void OnEvent(Event& e) override;
    virtual void OnImGuiRender() override;
private:
	bool onKeyPressed(KeyPressedEvent &e);

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