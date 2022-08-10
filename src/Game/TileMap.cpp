#include "stdafx.h"
#include "TileMap.h"
#include "World.h"
#include "shader.h"
//-----------------------------------------------------------------------------
sg_pipeline pip;
//sg_bindings bind_plane;
//uint8_t file_buffer[2 * 1024 * 1024];
void failCallback();

//struct PlaneVertex
//{
//	PlaneVertex() = default;
//	PlaneVertex(hmm_vec3 p, hmm_vec2 tc) : pos(p), texCoord(tc) {}
//
//	hmm_vec3 pos;
//	hmm_vec2 texCoord;
//};
//
//PlaneVertex planeVertices[] =
//{
//	{{ -1.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
//	{{  1.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
//	{{ -1.0f, 0.0f,  1.0f}, {0.0f, 1.0f}},
//	{{  1.0f, 0.0f,  1.0f}, {1.0f, 1.0f}},
//};
//
//uint16_t indexBuf[] =
//{
//	0, 1, 2,
//	1, 3, 2
//};
//-----------------------------------------------------------------------------
void TileMap::Init()
{
	//const sg_buffer_desc planeBufferDesc = { .size = sizeof(planeVertices), .usage = SG_USAGE_DYNAMIC, /*.data = SG_RANGE(plane_vertices)*/};
	/*const sg_buffer_desc planeBufferDesc = { .type = SG_BUFFERTYPE_VERTEXBUFFER, .data = SG_RANGE(planeVertices) };
	sg_buffer plane_buffer = sg_make_buffer(&planeBufferDesc);
	bind_plane.vertex_buffers[0] = plane_buffer;

	const sg_buffer_desc planeIndexBufferDesc = { .type = SG_BUFFERTYPE_INDEXBUFFER, .data = SG_RANGE(indexBuf) };
	sg_buffer plane_indexbuffer = sg_make_buffer(&planeIndexBufferDesc);
	bind_plane.index_buffer = plane_indexbuffer;

	sg_image metal_img_id = sg_alloc_image();
	bind_plane.fs_images[SLOT_diffuse_texture] = metal_img_id;

	lopgl_image_request_t imageDesc1 = {
		.path = "../CoreData/textures/1mx1m.png",
		.img_id = metal_img_id,
		.buffer_ptr = file_buffer,
		.buffer_size = sizeof(file_buffer),
		.fail_callback = failCallback
	};
	lopgl_load_image(&imageDesc1);*/

	// create a pipeline object for offscreen pass
	sg_pipeline_desc offscreenPipeDesc = { 0 };
	offscreenPipeDesc.shader = sg_make_shader(offscreen_shader_desc(sg_query_backend()));
	offscreenPipeDesc.layout.attrs[ATTR_vs_offscreen_a_pos].format = SG_VERTEXFORMAT_FLOAT3;
	offscreenPipeDesc.layout.attrs[ATTR_vs_offscreen_a_tex_coords].format = SG_VERTEXFORMAT_FLOAT2;
	offscreenPipeDesc.index_type = SG_INDEXTYPE_UINT16;
	//offscreenPipeDesc.cull_mode = SG_CULLMODE_BACK;
	offscreenPipeDesc.depth = { .pixel_format = SG_PIXELFORMAT_DEPTH, .compare = SG_COMPAREFUNC_LESS, .write_enabled = true };

	offscreenPipeDesc.colors[0].blend = 
	{
		.enabled = true,
		.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
		.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.op_rgb = SG_BLENDOP_ADD,
		.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA,
		.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.op_alpha = SG_BLENDOP_ADD
	};

	pip = sg_make_pipeline(&offscreenPipeDesc);	

	m_tile.Init();
}
//-----------------------------------------------------------------------------
void TileMap::Close()
{
	m_tile.Close();
}
//-----------------------------------------------------------------------------
void TileMap::Update(float deltaTime)
{
	static int currentFrame = 0;
	const int MAX_FRAMES_IN_FLIGHT = 2;
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	//sg_update_buffer(bind_plane.vertex_buffers[0], SG_RANGE(planeVertices));
}
//-----------------------------------------------------------------------------
void TileMap::Draw(const World& world)
{
	sg_apply_pipeline(pip);
	//sg_apply_bindings(&bind_plane);

	hmm_mat4 view = world.GetCameraManager().GetCameraViewMatrix();
	hmm_mat4 projection = HMM_Perspective(45.0f, (float)sapp_width() / (float)sapp_height(), 0.1f, 1000.0f);

	vs_params_t vs_params =
	{
		.model = HMM_Mat4d(1.0f),
		.view = view,
		.projection = projection
	};
	sg_range rangeData = { &vs_params, sizeof(vs_params_t) };
	sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &rangeData);

	//sg_draw(0, 6, 1);

	m_tile.Draw();
}
//-----------------------------------------------------------------------------