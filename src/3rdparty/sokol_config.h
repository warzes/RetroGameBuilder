#pragma once

/*
Commits on Aug 10, 2022
add a static analyzer helper script for Windows (clang --analyze based)
*/

#define SOKOL_WIN32_FORCE_MAIN

#define SOKOL_GLCORE33
//#define SOKOL_GLES2
//#define SOKOL_GLES3
//#define SOKOL_D3D11
//#define SOKOL_METAL
//#define SOKOL_WGPU

#define SOKOL_LOG(s) { puts(s); }