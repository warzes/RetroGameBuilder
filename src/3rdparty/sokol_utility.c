#include "sokol_utility.h"
#include "sokol_fetch.h"
#define STB_IMAGE_IMPLEMENTATION
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#include "stb_image.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#undef STB_IMAGE_IMPLEMENTATION

typedef struct {
    sg_image img_id;
    sg_wrap wrap_u;
    sg_wrap wrap_v;
    lopgl_fail_callback_t fail_callback;
} lopgl_img_request_data;

/* The fetch-callback is called by sokol_fetch.h when the data is loaded,
   or when an error has occurred.
*/
static void image_fetch_callback(const sfetch_response_t* response) 
{
    lopgl_img_request_data req_data = *(lopgl_img_request_data*)response->user_data;

    if (response->fetched) {
        /* the file data has been fetched, since we provided a big-enough
           buffer we can be sure that all data has been loaded here
        */
        int img_width, img_height, num_channels;
        const int desired_channels = 4;
        stbi_uc* pixels = stbi_load_from_memory(
            response->buffer_ptr,
            (int)response->fetched_size,
            &img_width, &img_height,
            &num_channels, desired_channels);
        if (pixels) {
            /* initialize the sokol-gfx texture */
            sg_init_image(req_data.img_id, &(sg_image_desc){
                .width = img_width,
                    .height = img_height,
                    /* set pixel_format to RGBA8 for WebGL */
                    .pixel_format = SG_PIXELFORMAT_RGBA8,
                    .wrap_u = req_data.wrap_u,
                    .wrap_v = req_data.wrap_v,
                    .min_filter = SG_FILTER_NEAREST,
                    .mag_filter = SG_FILTER_NEAREST,
                    .data.subimage[0][0] = {
                        .ptr = pixels,
                        .size = img_width * img_height * desired_channels,
                }
            });
            stbi_image_free(pixels);
        }
    }
    else if (response->failed) {
        req_data.fail_callback();
    }
}


void lopgl_load_image(const lopgl_image_request_t* request) {
    lopgl_img_request_data req_data = {
        .img_id = request->img_id,
        .wrap_u = request->wrap_u,
        .wrap_v = request->wrap_v,
        .fail_callback = request->fail_callback
    };

    sfetch_send(&(sfetch_request_t) {
        .path = request->path,
            .callback = image_fetch_callback,
            .buffer_ptr = request->buffer_ptr,
            .buffer_size = request->buffer_size,
            .user_data_ptr = &req_data,
            .user_data_size = sizeof(req_data)
    });
}
