#include "stdafx.h"
#include "shader.h"
#include "SceneTemp.h"
//-----------------------------------------------------------------------------
struct AppState
{
	AppState()
	{
		// a pass action to clear offscreen framebuffer
		offscreen.passAction.colors[0] = { .action = SG_ACTION_CLEAR, .value = {0.2f, 0.8f, 1.0f, 1.0f} };

		// a pass action for rendering the fullscreen-quad
		display.passAction.colors[0].action = SG_ACTION_DONTCARE;
		display.passAction.depth.action = SG_ACTION_DONTCARE;
		display.passAction.stencil.action = SG_ACTION_DONTCARE;
	}

	void CreateDisplay()
	{
		constexpr float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
#ifdef SOKOL_D3D11
		// positions   // texCoords
		- 1.0f,  -1.0f,  0.0f, 1.0f,
		-1.0f, 1.0f,  0.0f, 0.0f,
		1.0f, 1.0f,  1.0f, 0.0f,

		-1.0f,  -1.0f,  0.0f, 1.0f,
		1.0f, 1.0f,  1.0f, 0.0f,
		1.0f,  -1.0f,  1.0f, 1.0f
#else
		// positions   // texCoords
		- 1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
#endif
		};
		const sg_buffer_desc quadBufferDesc = { .data = SG_RANGE(quadVertices), .label = "quad-vertices" };
		sg_buffer quadVuffer = sg_make_buffer(&quadBufferDesc);

		// resource bindings to render an fullscreen-quad
		ApplicationState.display.bind.vertex_buffers[0] = quadVuffer;

		// and another pipeline-state-object for the display pass
		sg_pipeline_desc displayPipeDesc = { 0 };
		displayPipeDesc.shader = sg_make_shader(display_shader_desc(sg_query_backend()));
		displayPipeDesc.layout.attrs[ATTR_vs_display_a_pos].format = SG_VERTEXFORMAT_FLOAT2;
		displayPipeDesc.layout.attrs[ATTR_vs_display_a_tex_coords].format = SG_VERTEXFORMAT_FLOAT2;
		displayPipeDesc.label = "display-pipeline";
		ApplicationState.display.pip = sg_make_pipeline(&displayPipeDesc);
	}

	struct
	{
		sg_pass pass = { 0 };
		sg_pass_desc passDesc = { 0 };
		sg_pass_action passAction = { 0 };
	} offscreen; // render texture target
	struct 
	{
		sg_pass_action passAction = { 0 };
		sg_pipeline pip = { 0 };
		sg_bindings bind = { 0 };
	} display; // main screen
} ApplicationState;
static bool show_test_window = true;
static bool show_another_window = false;
//-----------------------------------------------------------------------------
void failCallback() 
{
	ApplicationState.display.passAction.colors[0] = { .action = SG_ACTION_CLEAR, .value = { 1.0f, 0.0f, 0.0f, 1.0f } };
}
//-----------------------------------------------------------------------------
void createOffscreenPass(int width, int height)
{
	// destroy previous resource (can be called for invalid id)
	sg_destroy_pass(ApplicationState.offscreen.pass);
	sg_destroy_image(ApplicationState.offscreen.passDesc.color_attachments[0].image);
	sg_destroy_image(ApplicationState.offscreen.passDesc.depth_stencil_attachment.image);

	// create offscreen rendertarget images and pass
	const sg_image_desc color_img_desc = { 
		.render_target = true,
		.width = width,
		.height = height,
//#ifdef SOKOL_D3D11
//	.pixel_format = SG_PIXELFORMAT_BGRA8,
//#else
//	.pixel_format = SG_PIXELFORMAT_RGBA8,
//#endif
		.pixel_format = (sg_pixel_format)sapp_color_format(),

		// Webgl 1.0 does not support repeat for textures that are not a power of two in size
		.min_filter = SG_FILTER_NEAREST,
		.mag_filter = SG_FILTER_NEAREST,
		.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
		.wrap_v = SG_WRAP_CLAMP_TO_EDGE,

		.label = "color-image"
	};
	sg_image color_img = sg_make_image(&color_img_desc);

	sg_image_desc depth_img_desc = color_img_desc;
	depth_img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
	depth_img_desc.label = "depth-image";
	sg_image depth_img = sg_make_image(&depth_img_desc);

	ApplicationState.offscreen.passDesc.color_attachments[0].image = color_img;
	ApplicationState.offscreen.passDesc.depth_stencil_attachment.image = depth_img;
	ApplicationState.offscreen.passDesc.label = "offscreen-pass";

	ApplicationState.offscreen.pass = sg_make_pass(&ApplicationState.offscreen.passDesc);

	// also need to update the fullscreen-quad texture bindings
	ApplicationState.display.bind.fs_images[SLOT_diffuse_texture] = color_img;
}
//-----------------------------------------------------------------------------
sg_context_desc sapp_sgcontext() noexcept
{
	sg_context_desc desc;
	memset(&desc, 0, sizeof(desc));
	desc.color_format = (sg_pixel_format)sapp_color_format();
	desc.depth_format = (sg_pixel_format)sapp_depth_format();
	desc.sample_count = sapp_sample_count();
	desc.gl.force_gles2 = sapp_gles2();
	desc.metal.device = sapp_metal_get_device();
	desc.metal.renderpass_descriptor_cb = sapp_metal_get_renderpass_descriptor;
	desc.metal.drawable_cb = sapp_metal_get_drawable;
	desc.d3d11.device = sapp_d3d11_get_device();
	desc.d3d11.device_context = sapp_d3d11_get_device_context();
	desc.d3d11.render_target_view_cb = sapp_d3d11_get_render_target_view;
	desc.d3d11.depth_stencil_view_cb = sapp_d3d11_get_depth_stencil_view;
	desc.wgpu.device = sapp_wgpu_get_device();
	desc.wgpu.render_view_cb = sapp_wgpu_get_render_view;
	desc.wgpu.resolve_view_cb = sapp_wgpu_get_resolve_view;
	desc.wgpu.depth_stencil_view_cb = sapp_wgpu_get_depth_stencil_view;
	return desc;
}
//-----------------------------------------------------------------------------
void GameAppInit() noexcept
{
	const sg_desc desc = { .context = sapp_sgcontext()};
	sg_setup(&desc);

	// initialize sokol_time
	stm_setup(); 

	sdtx_desc_t debugDesc = { 0 };
	debugDesc.fonts[0] = sdtx_font_cpc();
	sdtx_setup(&debugDesc);

	// setup sokol-fetch
	const sfetch_desc_t fetchDesc = { .max_requests = 8, .num_channels = 1, .num_lanes = 1 };
	sfetch_setup(&fetchDesc);

	// a render pass with one color- and one depth-attachment image
	createOffscreenPass(sapp_width(), sapp_height());

	ApplicationState.CreateDisplay();

	SceneInit();

	// gui

	// use sokol-imgui with all default-options (we're not doing multi-sampled rendering or using non-default pixel formats)
	simgui_desc_t simgui_desc = { };
	simgui_setup(&simgui_desc);
}
//-----------------------------------------------------------------------------
void GameAppFrame() noexcept
{
	sfetch_dowork();

	_lopgl.frame_time = stm_laptime(&_lopgl.time_stamp);

	if (_lopgl.fp_enabled) 
		update_fp_camera(&_lopgl.fp_cam, stm_ms(_lopgl.frame_time));

	sg_begin_pass(ApplicationState.offscreen.pass, &ApplicationState.offscreen.passAction);
	SceneDraw();
	sg_end_pass();

	// render main screen
	sg_begin_default_pass(&ApplicationState.display.passAction, sapp_width(), sapp_height());
	{
		sg_apply_pipeline(ApplicationState.display.pip);
		sg_apply_bindings(&ApplicationState.display.bind);
		sg_draw(0, 6, 1);

		renderHelp();

		{
			const int width = sapp_width();
			const int height = sapp_height();
			simgui_new_frame({ width, height, sapp_frame_duration(), sapp_dpi_scale() });

			// 1. Show a simple window
			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
			static float f = 0.0f;
			ImGui::Text("Hello, world!");
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", &ApplicationState.display.passAction.colors[0].value.r);
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
	sg_end_pass();

	sg_commit();
}
//-----------------------------------------------------------------------------
void GameAppInput(const sapp_event* e) noexcept
{
	if (e->type == SAPP_EVENTTYPE_RESIZED) 
	{
		createOffscreenPass(e->framebuffer_width, e->framebuffer_height);
	}
	simgui_handle_event(e);

	SceneInput(e);
}
//-----------------------------------------------------------------------------
void GameAppClose() noexcept
{
	simgui_shutdown();
	sg_shutdown();
}
//-----------------------------------------------------------------------------