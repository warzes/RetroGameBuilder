#pragma once

struct SceneFrameRender
{
	void Init(int width, int height)
	{
		passAction.colors[0] = { .action = SG_ACTION_CLEAR, .value = {0.2f, 0.8f, 1.0f, 1.0f} };

		// destroy previous resource (can be called for invalid id)
		Close();

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

		passDesc.color_attachments[0].image = color_img;
		passDesc.depth_stencil_attachment.image = depth_img;
		passDesc.label = "offscreen-pass";

		pass = sg_make_pass(&passDesc);
	}
	
	void Close()
	{
		sg_destroy_pass(pass);
		sg_destroy_image(passDesc.color_attachments[0].image);
		sg_destroy_image(passDesc.depth_stencil_attachment.image);
	}

	sg_image GetImage() const
	{
		return passDesc.color_attachments[0].image;
	}

	void SetFrame()
	{
		sg_begin_pass(pass, &passAction);
	}

	sg_pass pass = { 0 };
	sg_pass_desc passDesc = { 0 };
	sg_pass_action passAction = { 0 };
};