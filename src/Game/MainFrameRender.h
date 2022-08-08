#pragma once

struct MainFrameRender
{
	void Init()
	{
		passAction.colors[0].action = SG_ACTION_DONTCARE;
		passAction.depth.action     = SG_ACTION_DONTCARE;
		passAction.stencil.action   = SG_ACTION_DONTCARE;

		constexpr float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
#ifdef SOKOL_D3D11
			// positions   // texCoords
			-1.0f, -1.0f,  0.0f, 1.0f,
			-1.0f,  1.0f,  0.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 0.0f,
			-1.0f, -1.0f,  0.0f, 1.0f,
			 1.0f,  1.0f,  1.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 1.0f
#else
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
#endif
		};
		const sg_buffer_desc quadBufferDesc = { .data = SG_RANGE(quadVertices), .label = "MainFrameRenderVertices" };
		quadVuffer = sg_make_buffer(&quadBufferDesc);
		bind.vertex_buffers[0] = quadVuffer;

		sg_pipeline_desc displayPipeDesc = { 0 };
		displayPipeDesc.shader = sg_make_shader(display_shader_desc(sg_query_backend()));
		displayPipeDesc.layout.attrs[ATTR_vs_display_a_pos].format = SG_VERTEXFORMAT_FLOAT2;
		displayPipeDesc.layout.attrs[ATTR_vs_display_a_tex_coords].format = SG_VERTEXFORMAT_FLOAT2;
		displayPipeDesc.label = "MainFrameRenderPipeline";
		pip = sg_make_pipeline(&displayPipeDesc);
	}

	void Close()
	{
		sg_destroy_buffer(quadVuffer);
		sg_destroy_pipeline(pip);
	}

	void SetFrame()
	{
		sg_begin_default_pass(&passAction, sapp_width(), sapp_height());
	}

	void Draw()
	{
		sg_apply_pipeline(pip);
		sg_apply_bindings(&bind);
		sg_draw(0, 6, 1);
	}
	
	sg_pass_action passAction = { 0 };
	sg_pipeline pip = { 0 };
	sg_bindings bind = { 0 };
	sg_buffer quadVuffer = {};
};