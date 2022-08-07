#pragma once

#include "sokol_config.h"
#include "sokol_gfx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* response fail callback function signature */
typedef void(*lopgl_fail_callback_t)();

/* request parameters passed to lopgl_load_image() */
typedef struct lopgl_image_request_t {
	uint32_t _start_canary;
	const char* path;                       /* filesystem path or HTTP URL (required) */
	sg_image img_id;
	sg_wrap wrap_u;
	sg_wrap wrap_v;
	void* buffer_ptr;                       /* buffer pointer where data will be loaded into */
	uint32_t buffer_size;                   /* buffer size in number of bytes */
	lopgl_fail_callback_t fail_callback;    /* response callback function pointer (required) */
	uint32_t _end_canary;
} lopgl_image_request_t;

extern void lopgl_load_image(const lopgl_image_request_t* request);

#ifdef __cplusplus
} /* extern "C" */
#endif