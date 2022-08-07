#pragma once

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "HandmadeMath.h"
#include "lopgl_fast_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
    TODO:
        - add asserts to check setup has been called
        - add default values for all structs
        - use canary?
        - improve structure and add/update documentation
        - add support for obj's with multiple materials
        - add define to select functionality and reduce binary size
        - use get/set to configure cameras
*/

typedef struct lopgl_orbital_cam_desc {
    hmm_vec3 target;
    hmm_vec3 up;
    float pitch;
    float heading;
    float distance;
    // camera limits
    float min_pitch;
    float max_pitch;
    float min_dist;
    float max_dist;
    // control options
    float rotate_speed;
    float zoom_speed;
} lopgl_orbital_cam_desc_t;

typedef struct lopgl_fp_cam_desc {
    hmm_vec3 position;
    hmm_vec3 world_up;
    float yaw;
    float pitch;
    float zoom;
    // limits
    float min_pitch;
    float max_pitch;
    float min_zoom;
    float max_zoom;
    // control options
    float movement_speed;
    float aim_speed;
    float zoom_speed;
} lopgl_fp_cam_desc_t;

/* response fail callback function signature */
typedef void(*lopgl_fail_callback_t)();

typedef struct lopgl_obj_response_t {
    uint32_t _start_canary;
    fastObjMesh* mesh;
    void* user_data_ptr;
    uint32_t _end_canary;
} lopgl_obj_response_t;

typedef void(*lopgl_obj_request_callback_t)(lopgl_obj_response_t*);

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

/* request parameters passed to sfetch_send() */
typedef struct lopgl_obj_request_t {
    uint32_t _start_canary;
    const char* path;                       /* filesystem path or HTTP URL (required) */
    void* buffer_ptr;                       /* buffer pointer where data will be loaded into */
    uint32_t buffer_size;                   /* buffer size in number of bytes */
    const void* user_data_ptr;              /* pointer to a POD user-data block which will be memcpy'd(!) (optional) */
    lopgl_obj_request_callback_t callback;
    lopgl_fail_callback_t fail_callback;    /* response callback function pointer (required) */
    uint32_t _end_canary;
} lopgl_obj_request_t;

typedef struct lopgl_cubemap_request_t {
    uint32_t _start_canary;
    const char* path_right;                 /* filesystem path or HTTP URL (required) */
    const char* path_left;                  /* filesystem path or HTTP URL (required) */
    const char* path_top;                   /* filesystem path or HTTP URL (required) */
    const char* path_bottom;                /* filesystem path or HTTP URL (required) */
    const char* path_front;                 /* filesystem path or HTTP URL (required) */
    const char* path_back;                  /* filesystem path or HTTP URL (required) */
    sg_image img_id;
    uint8_t* buffer_ptr;                       /* buffer pointer where data will be loaded into */
    uint32_t buffer_offset;                 /* buffer offset in number of bytes */
    lopgl_fail_callback_t fail_callback;    /* response callback function pointer (required) */
    uint32_t _end_canary;
} lopgl_cubemap_request_t;

extern void lopgl_setup();

extern void lopgl_update();

extern void lopgl_shutdown();

extern lopgl_orbital_cam_desc_t lopgl_get_orbital_cam_desc();

extern lopgl_fp_cam_desc_t lopgl_get_fp_cam_desc();

extern void lopgl_set_orbital_cam(lopgl_orbital_cam_desc_t* desc);

extern void lopgl_set_fp_cam(lopgl_fp_cam_desc_t* desc);

extern hmm_mat4 lopgl_view_matrix();

extern float lopgl_fov();

extern hmm_vec3 lopgl_camera_position();

extern hmm_vec3 lopgl_camera_direction();

extern void lopgl_handle_input(const sapp_event* e);

extern void lopgl_render_help();

extern void lopgl_load_image(const lopgl_image_request_t* request);

extern void lopgl_load_obj(const lopgl_obj_request_t* request);

#ifdef __cplusplus
} /* extern "C" */
#endif