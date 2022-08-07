#include "stdafx.h"
//-----------------------------------------------------------------------------
#include "shader.h"
static bool show_test_window = true;
static bool show_another_window = false;

/* application state */
static struct {
	struct {
		sg_pass pass;
		sg_pass_desc pass_desc;
		sg_pass_action pass_action;
		sg_pipeline pip;
		sg_bindings bind_cube;
		sg_bindings bind_plane;
		sg_bindings bind_transparent;
	} offscreen;
	struct {
		sg_pass_action pass_action;
		sg_pipeline pip;
		sg_bindings bind;
	} display;
	uint8_t file_buffer[2 * 1024 * 1024];

	hmm_vec3 vegetation[5];
} state;

static void fail_callback() {
	state.display.pass_action.colors[0] = { .action = SG_ACTION_CLEAR, .value = { 1.0f, 0.0f, 0.0f, 1.0f } };
}

/* called initially and when window size changes */
void create_offscreen_pass(int width, int height) {
	/* destroy previous resource (can be called for invalid id) */
	sg_destroy_pass(state.offscreen.pass);
	sg_destroy_image(state.offscreen.pass_desc.color_attachments[0].image);
	sg_destroy_image(state.offscreen.pass_desc.depth_stencil_attachment.image);

	/* create offscreen rendertarget images and pass */
	sg_image_desc color_img_desc = { 0 };
	color_img_desc.render_target = true;
	color_img_desc.width = width;
	color_img_desc.height = height;
#ifdef SOKOL_D3D11
	color_img_desc.pixel_format = SG_PIXELFORMAT_BGRA8;
#elif
	color_img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
#endif
	/* Webgl 1.0 does not support repeat for textures that are not a power of two in size */
	color_img_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
	color_img_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
	color_img_desc.min_filter = SG_FILTER_LINEAR;
	color_img_desc.mag_filter = SG_FILTER_LINEAR;
	color_img_desc.label = "color-image";
	sg_image color_img = sg_make_image(&color_img_desc);

	sg_image_desc depth_img_desc = color_img_desc;
	depth_img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
	depth_img_desc.label = "depth-image";
	sg_image depth_img = sg_make_image(&depth_img_desc);

	state.offscreen.pass_desc = sg_pass_desc{ 0 };
	state.offscreen.pass_desc.color_attachments[0].image = color_img;
	state.offscreen.pass_desc.depth_stencil_attachment.image = depth_img;
	state.offscreen.pass_desc.label = "offscreen-pass";

	state.offscreen.pass = sg_make_pass(&state.offscreen.pass_desc);

	/* also need to update the fullscreen-quad texture bindings */
	state.display.bind.fs_images[SLOT_diffuse_texture] = color_img;
}

void init(void)
{
	lopgl_setup();

	/* a render pass with one color- and one depth-attachment image */
	create_offscreen_pass(sapp_width(), sapp_height());

	/* a pass action to clear offscreen framebuffer */
	state.offscreen.pass_action.colors[0] = { .action = SG_ACTION_CLEAR, .value = {0.1f, 0.1f, 0.1f, 1.0f} };

	/* a pass action for rendering the fullscreen-quad */
	state.display.pass_action = sg_pass_action{ 0 };
	state.display.pass_action.colors[0].action = SG_ACTION_DONTCARE;
	state.display.pass_action.depth.action = SG_ACTION_DONTCARE;
	state.display.pass_action.stencil.action = SG_ACTION_DONTCARE;

	float cube_vertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	sg_buffer_desc cubBufferDesc = {
	.size = sizeof(cube_vertices),
	.data = SG_RANGE(cube_vertices),
	.label = "cube-vertices" };
	sg_buffer cube_buffer = sg_make_buffer(&cubBufferDesc);

	float plane_vertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	sg_buffer_desc planeBufferDesc = {
	.size = sizeof(plane_vertices),
	.data = SG_RANGE(plane_vertices),
	.label = "plane-vertices" };
	sg_buffer plane_buffer = sg_make_buffer(&planeBufferDesc);

#ifdef SOKOL_D3D11
	float quad_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  -1.0f,  0.0f, 1.0f,
		-1.0f, 1.0f,  0.0f, 0.0f,
		1.0f, 1.0f,  1.0f, 0.0f,

		-1.0f,  -1.0f,  0.0f, 1.0f,
		1.0f, 1.0f,  1.0f, 0.0f,
		1.0f,  -1.0f,  1.0f, 1.0f
	};
#else
	float quad_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
	};
#endif
	sg_buffer_desc quadBufferDesc = {
	.size = sizeof(quad_vertices),
	.data = SG_RANGE(quad_vertices),
	.label = "quad-vertices" };
	sg_buffer quad_buffer = sg_make_buffer(&quadBufferDesc);

	state.offscreen.bind_cube.vertex_buffers[0] = cube_buffer;
	state.offscreen.bind_plane.vertex_buffers[0] = plane_buffer;

	/* resource bindings to render an fullscreen-quad */
	state.display.bind.vertex_buffers[0] = quad_buffer;

	/* create a pipeline object for offscreen pass */
	sg_pipeline_desc offscreenPipeDesc = { 0 };
	offscreenPipeDesc.shader = sg_make_shader(offscreen_shader_desc(sg_query_backend()));
	offscreenPipeDesc.layout.attrs[ATTR_vs_offscreen_a_pos].format = SG_VERTEXFORMAT_FLOAT3;
	offscreenPipeDesc.layout.attrs[ATTR_vs_offscreen_a_tex_coords].format = SG_VERTEXFORMAT_FLOAT2;
	offscreenPipeDesc.depth = { .pixel_format = SG_PIXELFORMAT_DEPTH, .compare = SG_COMPAREFUNC_LESS, .write_enabled = true };

	offscreenPipeDesc.colors[0].blend = {
		.enabled = true,
		.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
		.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.op_rgb = SG_BLENDOP_ADD,
		.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA,
		.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.op_alpha = SG_BLENDOP_ADD
	};

	state.offscreen.pip = sg_make_pipeline(&offscreenPipeDesc);

	/* and another pipeline-state-object for the display pass */
	sg_pipeline_desc displayPipeDesc = { 0 };
	displayPipeDesc.shader = sg_make_shader(display_shader_desc(sg_query_backend()));
	displayPipeDesc.layout.attrs[ATTR_vs_display_a_pos].format = SG_VERTEXFORMAT_FLOAT2;
	displayPipeDesc.layout.attrs[ATTR_vs_display_a_tex_coords].format = SG_VERTEXFORMAT_FLOAT2;
	displayPipeDesc.label = "display-pipeline";
	state.display.pip = sg_make_pipeline(&displayPipeDesc);

	sg_image container_img_id = sg_alloc_image();
	state.offscreen.bind_cube.fs_images[SLOT_diffuse_texture] = container_img_id;
	sg_image metal_img_id = sg_alloc_image();
	state.offscreen.bind_plane.fs_images[SLOT_diffuse_texture] = metal_img_id;
	sg_image grass_img_id = sg_alloc_image();
	state.offscreen.bind_transparent.fs_images[SLOT_diffuse_texture] = grass_img_id;


	lopgl_image_request_t imageDesc1 = {
	.path = "../CoreData/textures/metal.png",
	.img_id = metal_img_id,
	.buffer_ptr = state.file_buffer,
	.buffer_size = sizeof(state.file_buffer),
	.fail_callback = fail_callback
	};
	lopgl_load_image(&imageDesc1);

	lopgl_image_request_t imageDesc2 = {
	.path = "../CoreData/textures/container.jpg",
	.img_id = container_img_id,
	.buffer_ptr = state.file_buffer,
	.buffer_size = sizeof(state.file_buffer),
	.fail_callback = fail_callback
	};
	lopgl_load_image(&imageDesc2);


	lopgl_image_request_t imageDesc3 = {
	.path = "../CoreData/textures/transparent_window.png",
	.img_id = grass_img_id,
	.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
	.wrap_v = SG_WRAP_CLAMP_TO_EDGE,
	.buffer_ptr = state.file_buffer,
	.buffer_size = sizeof(state.file_buffer),
	.fail_callback = fail_callback
	};
	lopgl_load_image(&imageDesc3);


	state.vegetation[0] = HMM_Vec3(-1.5f, 0.0f, -0.48f);
	state.vegetation[1] = HMM_Vec3(1.5f, 0.0f, 0.51f);
	state.vegetation[2] = HMM_Vec3(0.0f, 0.0f, 0.7f);
	state.vegetation[3] = HMM_Vec3(-0.3f, 0.0f, -2.3f);
	state.vegetation[4] = HMM_Vec3(0.5f, 0.0f, -0.6f);

	float transparent_vertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	sg_buffer_desc transparent_bufferDesc = {
	.size = sizeof(transparent_vertices),
	.data = SG_RANGE(transparent_vertices),
	.label = "transparent-vertices"
	};

	sg_buffer transparent_buffer = sg_make_buffer(&transparent_bufferDesc);
	state.offscreen.bind_transparent.vertex_buffers[0] = transparent_buffer;



	// gui

	// use sokol-imgui with all default-options (we're not doing
	// multi-sampled rendering or using non-default pixel formats)
	simgui_desc_t simgui_desc = { };
	simgui_setup(&simgui_desc);
}

void frame(void) {
	lopgl_update();

	hmm_mat4 view = lopgl_view_matrix();
	hmm_mat4 projection = HMM_Perspective(lopgl_fov(), (float)sapp_width() / (float)sapp_height(), 0.1f, 100.0f);

	vs_params_t vs_params = {
	.view = view,
	.projection = projection
	};

	/* the offscreen pass, rendering an rotating, untextured cube into a render target image */
	sg_begin_pass(state.offscreen.pass, &state.offscreen.pass_action);

	sg_apply_pipeline(state.offscreen.pip);

	sg_apply_bindings(&state.offscreen.bind_cube);

	vs_params.model = HMM_Translate(HMM_Vec3(-1.0f, 0.0f, -1.0f));
	sg_range r1 = { &vs_params, sizeof(vs_params_t) };
	sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &r1);

	sg_draw(0, 36, 1);

	vs_params.model = HMM_Translate(HMM_Vec3(2.0f, 0.0f, 0.0f));
	r1 = { &vs_params, sizeof(vs_params_t) };
	sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &r1);
	sg_draw(0, 36, 1);

	sg_apply_bindings(&state.offscreen.bind_plane);

	vs_params.model = HMM_Mat4d(1.0f);
	r1 = { &vs_params, sizeof(vs_params_t) };
	sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &r1);
	sg_draw(0, 6, 1);

	sg_apply_bindings(&state.offscreen.bind_transparent);


	hmm_vec3 position = lopgl_camera_position();
	// simple bubble sort algorithm to sort vegetation from furthest to nearest
	for (int i = 1; i < 5; ++i) {
		for (int j = i - 1; j >= 0; --j) {
			hmm_vec3 translate0 = HMM_SubtractVec3(state.vegetation[j], position);
			hmm_vec3 translate1 = HMM_SubtractVec3(state.vegetation[j + 1], position);

			if (HMM_LengthSquaredVec3(translate0) < HMM_LengthSquaredVec3(translate1)) {
				hmm_vec3 temp = state.vegetation[j];
				state.vegetation[j] = state.vegetation[j + 1];
				state.vegetation[j + 1] = temp;
				break;
			}
		}
	}

	for (size_t i = 0; i < 5; i++) {
		vs_params.model = HMM_Translate(state.vegetation[i]);
		r1 = { &vs_params, sizeof(vs_params_t) };
		sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &r1);
		sg_draw(0, 6, 1);
	}

	sg_end_pass();

	/* and the display-pass, rendering a quad, using the previously rendered
	offscreen render-target as texture */
	sg_begin_default_pass(&state.display.pass_action, sapp_width(), sapp_height());
	sg_apply_pipeline(state.display.pip);
	sg_apply_bindings(&state.display.bind);
	sg_draw(0, 6, 1);

	lopgl_render_help();


	{
		const int width = sapp_width();
		const int height = sapp_height();
		simgui_new_frame({ width, height, sapp_frame_duration(), sapp_dpi_scale() });

		// 1. Show a simple window
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", &state.display.pass_action.colors[0].value.r);
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

	sg_end_pass();
	sg_commit();
}

void input(const sapp_event* e) {
	if (e->type == SAPP_EVENTTYPE_RESIZED) {
		create_offscreen_pass(e->framebuffer_width, e->framebuffer_height);
	}

	lopgl_handle_input(e);

	simgui_handle_event(e);
}

void cleanup(void) {
	simgui_shutdown();
	lopgl_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[])
{
	return 
	{
		.init_cb = init,
		.frame_cb = frame,
		.cleanup_cb = cleanup,
		.event_cb = input,
		.width = 1024,
		.height = 768,
		.window_title = "Game",
		.enable_clipboard = true,
		//.enable_dragndrop = true,
		.gl_force_gles2 = true,
		.ios_keyboard_resizes_canvas = false,
	};
}