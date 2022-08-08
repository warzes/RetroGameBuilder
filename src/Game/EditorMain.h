#pragma once

static bool show_test_window = true;
static bool show_another_window = false;

void EditorInit()
{
	// gui
	// use sokol-imgui with all default-options (we're not doing multi-sampled rendering or using non-default pixel formats)
	simgui_desc_t simgui_desc = { };
	simgui_setup(&simgui_desc);
}

void EditorUpdate(const sapp_event* e)
{
	simgui_handle_event(e);
}

void EditorDraw()
{
	// draw ui
	{
		const int width = sapp_width();
		const int height = sapp_height();
		simgui_new_frame({ width, height, sapp_frame_duration(), sapp_dpi_scale() });

		// 1. Show a simple window
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		//ImGui::ColorEdit3("clear color", &ApplicationState.mainFrame.passAction.colors[0].value.r);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("w: %d, h: %d, dpi_scale: %.1f", sapp_width(), sapp_height(), sapp_dpi_scale());
		if (ImGui::Button(sapp_is_fullscreen() ? "Switch to windowed" : "Switch to fullscreen")) {
			sapp_toggle_fullscreen();
		}

		// 2. Show another simple window, this time using an explicit Begin/End pair
		if (show_another_window) {
			ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello");
			ImGui::End();
		}

		// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowDemoWindow()
		if (show_test_window) {
			ImGui::SetNextWindowPos(ImVec2(460, 20), ImGuiCond_FirstUseEver);
			ImGui::ShowDemoWindow();
		}

		// the sokol_gfx draw pass
		simgui_render();
	}
}

void EditorClose()
{
	simgui_shutdown();
}