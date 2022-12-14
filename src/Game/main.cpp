#include "stdafx.h"
#include "shader.h"
#include "SceneTemp.h"
#include "MainFrameRender.h"
#include "SceneFrameRender.h"
#if EDITOR_ON
#include "EditorMain.h"
#endif
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#endif
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
extern "C"
{
	// NVIDIA: Force usage of NVidia GPU in case there is an integrated graphics unit as well, if we don't do this we risk getting the integrated graphics unit and hence a horrible performance
	// -> See "Enabling High Performance Graphics Rendering on Optimus Systems" http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	_declspec(dllexport) auto NvOptimusEnablement = 0x00000001UL;

	// AMD: Force usage of AMD GPU in case there is an integrated graphics unit as well, if we don't do this we risk getting the integrated graphics unit and hence a horrible performance
	// -> Named "Dynamic Switchable Graphics", found no official documentation, only https://community.amd.com/message/1307599#comment-1307599 - "Can an OpenGL app default to the discrete GPU on an Enduro system?"
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif
//-----------------------------------------------------------------------------
struct AppState
{
	SceneFrameRender sceneFrame;
	MainFrameRender mainFrame;
} ApplicationState;

World world;
//-----------------------------------------------------------------------------
void failCallback()
{
	ApplicationState.mainFrame.passAction.colors[0] = { .action = SG_ACTION_CLEAR, .value = { 1.0f, 0.0f, 0.0f, 1.0f } };
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
	sg_desc desc = { .context = sapp_sgcontext() };	
	desc.buffer_pool_size = 65000;

	sg_setup(&desc);
	stm_setup();
	sdtx_desc_t debugDesc = { 0 };
	debugDesc.fonts[0] = sdtx_font_cpc();
	sdtx_setup(&debugDesc);
	const sfetch_desc_t fetchDesc = { .max_requests = 8, .num_channels = 1, .num_lanes = 1 };
	sfetch_setup(&fetchDesc);

	ApplicationState.sceneFrame.Init(sapp_width(), sapp_height());
	ApplicationState.mainFrame.Init();
	ApplicationState.mainFrame.bind.fs_images[SLOT_diffuse_texture] = ApplicationState.sceneFrame.GetImage();

#if EDITOR_ON
	EditorInit();
#endif

	world.Init();
	SceneInit();
}
//-----------------------------------------------------------------------------
void GameAppFrame() noexcept
{
	// update
	sfetch_dowork();
	_lopgl.frame_time = stm_laptime(&_lopgl.time_stamp);
	world.Update(stm_ms(_lopgl.frame_time));

	// render game scene
	ApplicationState.sceneFrame.SetFrame();
	SceneDraw(world);
	world.Draw();
	sg_end_pass();

	// render main screen
	ApplicationState.mainFrame.SetFrame();
	{
		ApplicationState.mainFrame.Draw();

		renderHelp(world);
#if EDITOR_ON
		EditorDraw();
#endif		
	}
	sg_end_pass();

	sg_commit();
}
//-----------------------------------------------------------------------------
void GameAppInput(const sapp_event* e) noexcept
{
	if (e->type == SAPP_EVENTTYPE_RESIZED)
	{
		ApplicationState.sceneFrame.Init(sapp_width(), sapp_height());
		ApplicationState.mainFrame.bind.fs_images[SLOT_diffuse_texture] = ApplicationState.sceneFrame.GetImage();
	}
#if EDITOR_ON
	EditorUpdate(e);
#endif

	if (e->type == SAPP_EVENTTYPE_KEY_DOWN)
	{
		if (e->key_code == SAPP_KEYCODE_C)
		{
			if (world.GetCameraManager().GetCameraType() == CameraType::Free) world.GetCameraManager().SetCameraType(CameraType::Orbital);
			else world.GetCameraManager().SetCameraType(CameraType::Free);
		}
		else if (e->key_code == SAPP_KEYCODE_H)
		{
			_lopgl.show_help = !_lopgl.show_help;
		}
		else if (e->key_code == SAPP_KEYCODE_U)
		{
			_lopgl.hide_ui = !_lopgl.hide_ui;
		}
		else if (e->key_code == SAPP_KEYCODE_ESCAPE)
		{
			sapp_request_quit();
		}
	}

	hmm_vec2 mouseOffset = { 0.0f, 0.0f };
	if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE)
	{
		if (!_lopgl.first_mouse)
		{
			mouseOffset.X = e->mouse_x - _lopgl.last_mouse.X;
			mouseOffset.Y = _lopgl.last_mouse.Y - e->mouse_y;
		}
		else
		{
			_lopgl.first_mouse = false;
		}

		_lopgl.last_mouse.X = e->mouse_x;
		_lopgl.last_mouse.Y = e->mouse_y;
	}

	world.Input(e, mouseOffset);
}
//-----------------------------------------------------------------------------
void GameAppClose() noexcept
{
	ApplicationState.sceneFrame.Close();
	ApplicationState.mainFrame.Close();
#if EDITOR_ON
	EditorClose();
#endif
	world.Close();
	sg_shutdown();
}
//-----------------------------------------------------------------------------
sapp_desc sokol_main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	return
	{
		.init_cb = GameAppInit,
		.frame_cb = GameAppFrame,
		.cleanup_cb = GameAppClose,
		.event_cb = GameAppInput,
		.width = 1024,
		.height = 768,
		.window_title = "Game",
		.enable_clipboard = true,
		//.enable_dragndrop = true,
		.gl_force_gles2 = true,
		.ios_keyboard_resizes_canvas = false,
	};
}
//-----------------------------------------------------------------------------