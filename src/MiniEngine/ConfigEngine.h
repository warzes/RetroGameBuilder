#pragma once

//=========================================================================
// Use Config
//=========================================================================

//=========================================================================
// Base Config
//=========================================================================

#if defined(__EMSCRIPTEN__ )
#	define PLATFORM_DESKTOP 0
#	define PLATFORM_ANDROID 0
#	define PLATFORM_WEB 1
#elif defined(__ANDROID__)
#	define PLATFORM_DESKTOP 0
#	define PLATFORM_ANDROID 1
#	define PLATFORM_WEB 0
#else
#	define PLATFORM_DESKTOP 1
#	define PLATFORM_ANDROID 0
#	define PLATFORM_WEB 0
#endif