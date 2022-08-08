#pragma once

#include "Config.h"

#if defined(_MSC_VER)
#	pragma warning(push, 0)
//#	pragma warning(disable : 5039)
#endif

#include "3rdparty/sokol_config.h"
#include <3rdparty/sokol_app.h>
#include <3rdparty/sokol_gfx.h>
#include <3rdparty/sokol_time.h>
#include <3rdparty/sokol_fetch.h>
#include <3rdparty/sokol_debugtext.h>
#include <3rdparty/HandmadeMath.h>
#include <3rdparty/sokol_utility.h>

#if EDITOR_ON
#	include <3rdparty/imgui.h>
#	include <3rdparty/sokol_imgui.h>
#endif

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

