#pragma once

#include "World.h"

//struct lopgl_fp_cam_desc_t {
//	hmm_vec3 position;
//	hmm_vec3 world_up;
//	float yaw;
//	float pitch;
//	float zoom;
//	// limits
//	float min_pitch;
//	float max_pitch;
//	float min_zoom;
//	float max_zoom;
//	// control options
//	float movement_speed;
//	float aim_speed;
//	float zoom_speed;
//};

//struct fp_cam
//{
//	// camera attributes
//	hmm_vec3 position;
//	hmm_vec3 world_up;
//	float yaw;
//	float pitch;
//	float zoom;
//	// limits
//	float min_pitch;
//	float max_pitch;
//	float min_zoom;
//	float max_zoom;
//	// control options
//	float movement_speed;
//	float aim_speed;
//	float zoom_speed;
//	// control state
//	bool enable_aim;
//	bool move_forward;
//	bool move_backward;
//	bool move_left;
//	bool move_right;
//	// internal state
//	hmm_vec3 front;
//	hmm_vec3 up;
//	hmm_vec3 right;
//};

struct _cubemap_request_t
{
	sg_image img_id;
	uint8_t* buffer;
	int buffer_offset;
	int fetched_sizes[6];
	int finished_requests;
	bool failed;
	lopgl_fail_callback_t fail_callback;
};

struct lopgl_state_t
{
	//fp_cam fp_cam;
	//bool fp_enabled;
	bool show_help;
	bool hide_ui;
	bool first_mouse;
	hmm_vec2 last_mouse;
	uint64_t time_stamp;
	uint64_t frame_time;
	_cubemap_request_t cubemap_req;
} _lopgl;

//inline void update_fp_cam_vectors(struct fp_cam* camera) {
//	// Calculate the new Front vector
//	hmm_vec3 front;
//	front.X = cosf(HMM_ToRadians(camera->yaw)) * cosf(HMM_ToRadians(camera->pitch));
//	front.Y = sinf(HMM_ToRadians(camera->pitch));
//	front.Z = sinf(HMM_ToRadians(camera->yaw)) * cosf(HMM_ToRadians(camera->pitch));
//	camera->front = HMM_NormalizeVec3(front);
//	// Also re-calculate the Right and Up vector
//	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//	camera->right = HMM_NormalizeVec3(HMM_Cross(camera->front, camera->world_up));
//	camera->up = HMM_NormalizeVec3(HMM_Cross(camera->right, camera->front));
//}

//void lopgl_set_fp_cam(lopgl_fp_cam_desc_t desc) 
//{
//	// camera attributes
//	_lopgl.fp_cam.position = desc.position;
//	_lopgl.fp_cam.world_up = desc.world_up;
//	_lopgl.fp_cam.yaw = desc.yaw;
//	_lopgl.fp_cam.pitch = desc.pitch;
//	_lopgl.fp_cam.zoom = desc.zoom;
//	// limits
//	_lopgl.fp_cam.min_pitch = desc.min_pitch;
//	_lopgl.fp_cam.max_pitch = desc.max_pitch;
//	_lopgl.fp_cam.min_zoom = desc.min_zoom;
//	_lopgl.fp_cam.max_zoom = desc.max_zoom;
//	// control options
//	_lopgl.fp_cam.movement_speed = desc.movement_speed;
//	_lopgl.fp_cam.aim_speed = desc.aim_speed;
//	_lopgl.fp_cam.zoom_speed = desc.zoom_speed;
//	// control state
//	_lopgl.fp_cam.enable_aim = false;
//	_lopgl.fp_cam.move_forward = false;
//	_lopgl.fp_cam.move_backward = false;
//	_lopgl.fp_cam.move_left = false;
//	_lopgl.fp_cam.move_right = false;
//
//	update_fp_cam_vectors(&_lopgl.fp_cam);
//}

//void update_fp_camera(struct fp_cam* camera, float delta_time) 
//{
//	float velocity = camera->movement_speed * delta_time;
//	if (camera->move_forward) {
//		hmm_vec3 offset = HMM_MultiplyVec3f(camera->front, velocity);
//		camera->position = HMM_AddVec3(camera->position, offset);
//	}
//	if (camera->move_backward) {
//		hmm_vec3 offset = HMM_MultiplyVec3f(camera->front, velocity);
//		camera->position = HMM_SubtractVec3(camera->position, offset);
//	}
//	if (camera->move_left) {
//		hmm_vec3 offset = HMM_MultiplyVec3f(camera->right, velocity);
//		camera->position = HMM_SubtractVec3(camera->position, offset);
//	}
//	if (camera->move_right) {
//		hmm_vec3 offset = HMM_MultiplyVec3f(camera->right, velocity);
//		camera->position = HMM_AddVec3(camera->position, offset);
//	}
//}

//hmm_mat4 view_matrix_fp(struct fp_cam* camera) 
//{
//	hmm_vec3 target = HMM_AddVec3(camera->position, camera->front);
//	return HMM_LookAt(camera->position, target, camera->up);
//}

float lopgl_fov() {
	return 45.0f;
}

void failCallback();

const char* help_fp() {
	return  "Forward:\t'W' '\xf0'\n"
		"Left:\t\t'A' '\xf2\'\n"
		"Back:\t\t'S' '\xf1\'\n"
		"Right:\t\t'D' '\xf3\'\n"
		"Look:\t\tleft-mouse-btn\n"
		"Zoom:\t\tmouse-scroll\n";
}

const char* help_orbital() {
	return  "Look:\t\tleft-mouse-btn\n"
		"Zoom:\t\tmouse-scroll\n";
}

void renderHelp(const World& world) {
	if (_lopgl.hide_ui) {
		return;
	}

	sdtx_canvas(sapp_width() * 0.5f, sapp_height() * 0.5f);
	sdtx_origin(0.25f, 0.25f);
	sdtx_home();

	if (!_lopgl.show_help) {
		sdtx_color4b(0xff, 0xff, 0xff, 0xaf);
		sdtx_puts("Show help:\t'H'");
	}
	else {
		sdtx_color4b(0x00, 0xff, 0x00, 0xaf);
		sdtx_puts("Hide help:\t'H'\n\n");
		sdtx_printf("Frame Time:\t%.3f\n\n", stm_ms(_lopgl.frame_time));
		sdtx_printf("Orbital Cam\t[%c]\n", world.GetCameraType() == CameraType::Orbital ? ' ' : '*');
		sdtx_printf("FP Cam\t\t[%c]\n\n", world.GetCameraType() == CameraType::Free ? '*' : ' ');
		sdtx_puts("Switch Cam:\t'C'\n\n");

		if (world.GetCameraType() == CameraType::Free) {
			sdtx_puts(help_fp());
		}
		else {
			sdtx_puts(help_orbital());
		}

		sdtx_puts("\nExit:\t\t'ESC'");
	}

	sdtx_draw();
}

sg_pipeline pip;
sg_bindings bind_cube;
sg_bindings bind_plane;
sg_bindings bind_transparent;
uint8_t file_buffer[2 * 1024 * 1024];
hmm_vec3 vegetation[5];

void SceneInit()
{
	//_lopgl.fp_enabled = false;
	_lopgl.first_mouse = true;
	_lopgl.show_help = false;
	_lopgl.hide_ui = false;

	constexpr float cube_vertices[] = {
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
	const sg_buffer_desc cubBufferDesc = { .data = SG_RANGE(cube_vertices), .label = "cube-vertices" };
	sg_buffer cube_buffer = sg_make_buffer(&cubBufferDesc);

	constexpr float plane_vertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	const sg_buffer_desc planeBufferDesc = { .data = SG_RANGE(plane_vertices), .label = "plane-vertices" };
	sg_buffer plane_buffer = sg_make_buffer(&planeBufferDesc);

	constexpr float transparent_vertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	const sg_buffer_desc transparent_bufferDesc = { .data = SG_RANGE(transparent_vertices), .label = "transparent-vertices" };
	sg_buffer transparent_buffer = sg_make_buffer(&transparent_bufferDesc);

	bind_cube.vertex_buffers[0] = cube_buffer;
	bind_plane.vertex_buffers[0] = plane_buffer;
	bind_transparent.vertex_buffers[0] = transparent_buffer;

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

	pip = sg_make_pipeline(&offscreenPipeDesc);

	sg_image container_img_id = sg_alloc_image();
	bind_cube.fs_images[SLOT_diffuse_texture] = container_img_id;
	sg_image metal_img_id = sg_alloc_image();
	bind_plane.fs_images[SLOT_diffuse_texture] = metal_img_id;
	sg_image grass_img_id = sg_alloc_image();
	bind_transparent.fs_images[SLOT_diffuse_texture] = grass_img_id;


	lopgl_image_request_t imageDesc1 = {
		.path = "../CoreData/textures/metal.png",
		.img_id = metal_img_id,
		.buffer_ptr = file_buffer,
		.buffer_size = sizeof(file_buffer),
		.fail_callback = failCallback
	};
	lopgl_load_image(&imageDesc1);

	lopgl_image_request_t imageDesc2 = {
		.path = "../CoreData/textures/container.jpg",
		.img_id = container_img_id,
		.buffer_ptr = file_buffer,
		.buffer_size = sizeof(file_buffer),
		.fail_callback = failCallback
	};
	lopgl_load_image(&imageDesc2);


	lopgl_image_request_t imageDesc3 = {
		.path = "../CoreData/textures/transparent_window.png",
		.img_id = grass_img_id,
		.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
		.wrap_v = SG_WRAP_CLAMP_TO_EDGE,
		.buffer_ptr = file_buffer,
		.buffer_size = sizeof(file_buffer),
		.fail_callback = failCallback
	};
	lopgl_load_image(&imageDesc3);

	vegetation[0] = HMM_Vec3(-1.5f, 0.0f, -0.48f);
	vegetation[1] = HMM_Vec3(1.5f, 0.0f, 0.51f);
	vegetation[2] = HMM_Vec3(0.0f, 0.0f, 0.7f);
	vegetation[3] = HMM_Vec3(-0.3f, 0.0f, -2.3f);
	vegetation[4] = HMM_Vec3(0.5f, 0.0f, -0.6f);
}

void SceneDraw(const World& world)
{
	hmm_mat4 view = world.GetCameraViewMatrix();
	hmm_mat4 projection = HMM_Perspective(lopgl_fov(), (float)sapp_width() / (float)sapp_height(), 0.1f, 100.0f);

	vs_params_t vs_params = {
	.view = view,
	.projection = projection
	};

	/* the offscreen pass, rendering an rotating, untextured cube into a render target image */

	sg_apply_pipeline(pip);

	sg_apply_bindings(&bind_cube);

	vs_params.model = HMM_Translate(HMM_Vec3(-1.0f, 0.0f, -1.0f));
	sg_range r1 = { &vs_params, sizeof(vs_params_t) };
	sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &r1);

	sg_draw(0, 36, 1);

	vs_params.model = HMM_Translate(HMM_Vec3(2.0f, 0.0f, 0.0f));
	r1 = { &vs_params, sizeof(vs_params_t) };
	sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &r1);
	sg_draw(0, 36, 1);

	sg_apply_bindings(&bind_plane);

	vs_params.model = HMM_Mat4d(1.0f);
	r1 = { &vs_params, sizeof(vs_params_t) };
	sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &r1);
	sg_draw(0, 6, 1);

	sg_apply_bindings(&bind_transparent);

	hmm_vec3 position = world.GetPosition();

	// simple bubble sort algorithm to sort vegetation from furthest to nearest
	for (int i = 1; i < 5; ++i) {
		for (int j = i - 1; j >= 0; --j) {
			hmm_vec3 translate0 = HMM_SubtractVec3(vegetation[j], position);
			hmm_vec3 translate1 = HMM_SubtractVec3(vegetation[j + 1], position);

			if (HMM_LengthSquaredVec3(translate0) < HMM_LengthSquaredVec3(translate1)) {
				hmm_vec3 temp = vegetation[j];
				vegetation[j] = vegetation[j + 1];
				vegetation[j + 1] = temp;
				break;
			}
		}
	}

	for (size_t i = 0; i < 5; i++) {
		vs_params.model = HMM_Translate(vegetation[i]);
		r1 = { &vs_params, sizeof(vs_params_t) };
		sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &r1);
		sg_draw(0, 6, 1);
	}
}

//static void aim_fp_camera(struct fp_cam* camera, hmm_vec2 mouse_offset) {
//	camera->yaw += mouse_offset.X * camera->aim_speed;
//	camera->pitch += mouse_offset.Y * camera->aim_speed;
//
//	camera->pitch = HMM_Clamp(camera->min_pitch, camera->pitch, camera->max_pitch);
//
//	update_fp_cam_vectors(camera);
//}

//static void zoom_fp_camera(struct fp_cam* camera, float yoffset) {
//	camera->zoom -= yoffset * camera->zoom_speed;
//	camera->zoom = HMM_Clamp(camera->min_zoom, camera->zoom, camera->max_zoom);
//}


//void handle_input_fp(struct fp_cam* camera, const sapp_event* e, hmm_vec2 mouse_offset)
//{
//	if (e->type == SAPP_EVENTTYPE_KEY_DOWN) {
//		if (e->key_code == SAPP_KEYCODE_W || e->key_code == SAPP_KEYCODE_UP) {
//			camera->move_forward = true;
//		}
//		else if (e->key_code == SAPP_KEYCODE_S || e->key_code == SAPP_KEYCODE_DOWN) {
//			camera->move_backward = true;
//		}
//		else if (e->key_code == SAPP_KEYCODE_A || e->key_code == SAPP_KEYCODE_LEFT) {
//			camera->move_left = true;
//		}
//		else if (e->key_code == SAPP_KEYCODE_D || e->key_code == SAPP_KEYCODE_RIGHT) {
//			camera->move_right = true;
//		}
//	}
//	else if (e->type == SAPP_EVENTTYPE_KEY_UP) {
//		if (e->key_code == SAPP_KEYCODE_W || e->key_code == SAPP_KEYCODE_UP) {
//			camera->move_forward = false;
//		}
//		else if (e->key_code == SAPP_KEYCODE_S || e->key_code == SAPP_KEYCODE_DOWN) {
//			camera->move_backward = false;
//		}
//		else if (e->key_code == SAPP_KEYCODE_A || e->key_code == SAPP_KEYCODE_LEFT) {
//			camera->move_left = false;
//		}
//		else if (e->key_code == SAPP_KEYCODE_D || e->key_code == SAPP_KEYCODE_RIGHT) {
//			camera->move_right = false;
//		}
//	}
//	else if (e->type == SAPP_EVENTTYPE_MOUSE_DOWN) {
//		if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
//			camera->enable_aim = true;
//		}
//	}
//	else if (e->type == SAPP_EVENTTYPE_MOUSE_UP) {
//		if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
//			camera->enable_aim = false;
//		}
//	}
//	else if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE) {
//		if (camera->enable_aim) {
//			aim_fp_camera(camera, mouse_offset);
//		}
//	}
//	else if (e->type == SAPP_EVENTTYPE_MOUSE_SCROLL) {
//		zoom_fp_camera(camera, e->scroll_y);
//	}
//}

void SceneInput(World& world, const sapp_event* e) {
	if (e->type == SAPP_EVENTTYPE_KEY_DOWN) {
		if (e->key_code == SAPP_KEYCODE_C) {
			if (world.GetCameraType() == CameraType::Free) world.SetCameraType(CameraType::Orbital);
			else world.SetCameraType(CameraType::Free);
		}
		else if (e->key_code == SAPP_KEYCODE_H) {
			_lopgl.show_help = !_lopgl.show_help;
		}
		else if (e->key_code == SAPP_KEYCODE_U) {
			_lopgl.hide_ui = !_lopgl.hide_ui;
		}
		else if (e->key_code == SAPP_KEYCODE_ESCAPE) {
			sapp_request_quit();
		}
	}

	hmm_vec2 mouse_offset = HMM_Vec2(0.0f, 0.0f);

	if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE) {
		if (!_lopgl.first_mouse) {
			mouse_offset.X = e->mouse_x - _lopgl.last_mouse.X;
			mouse_offset.Y = _lopgl.last_mouse.Y - e->mouse_y;
		}
		else {
			_lopgl.first_mouse = false;
		}

		_lopgl.last_mouse.X = e->mouse_x;
		_lopgl.last_mouse.Y = e->mouse_y;
	}

	world.Input(e, mouse_offset);
}