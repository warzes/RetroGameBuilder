#include "stdafx.h"
#include "TileMap.h"
#include "World.h"
#include "shader.h"
//-----------------------------------------------------------------------------
sg_pipeline pip;
void failCallback();
//-----------------------------------------------------------------------------
void TileMap::Init()
{
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

	static bool first = true;
	if (first == true)
	{
		first = false;
		return;
	}

	static float ttt1 = 0;
	static float ttt2 = 0;
	static float ttt3 = 0;
	static float ttt4 = 0;
	static bool reverse = false;

	if (!reverse)
	{
		ttt1 += rand() % 100 / 10000.0f;
		ttt2 = ttt1;
		//ttt2 += rand() % 100 / 10000.0f;
		//ttt3 += rand() % 100 / 10000.0f;
		//ttt4 += rand() % 100 / 10000.0f;

		HeightTile heightTop = { ttt1, ttt2, ttt3, ttt4 };
		HeightTile heightBottom = { 0.1, 0.2, 0.3, 0.4 };
		m_tile.SetHeights(heightTop, heightBottom);

		if (ttt1 > 0.5f || ttt2 > 0.5f || ttt3 > 0.5f || ttt4 > 0.5f)
		{
			reverse = true;
		}
	}
	else
	{
		ttt1 -= rand() % 100 / 10000.0f;
		ttt2 = ttt1;
		//ttt2 -= rand() % 100 / 10000.0f;
		//ttt3 -= rand() % 100 / 10000.0f;
		//ttt4 -= rand() % 100 / 10000.0f;


		HeightTile heightTop = { ttt1, ttt2, ttt3, ttt4 };
		HeightTile heightBottom = { 0.1, 0.2, 0.3, 0.4 };
		m_tile.SetHeights(heightTop, heightBottom);

		if (ttt1 < 0.0f || ttt2 < 0.0f || ttt3 < 0.0f || ttt4 < 0.0f)
		{
			reverse = false;
		}
	}


	//sg_update_buffer(bind_plane.vertex_buffers[0], SG_RANGE(planeVertices));
}
//-----------------------------------------------------------------------------
void TileMap::Draw(const World& world)
{
	sg_apply_pipeline(pip);

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

	m_tile.Draw();
}
//-----------------------------------------------------------------------------