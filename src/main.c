#include <lvgl.h>
#include <lv_demos.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#if LV_USE_EVDEV

static void set_mouse_cursor_icon(lv_indev_t * indev, lv_display_t * display)
{
    /* Set the cursor icon */
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t * cursor_obj = lv_image_create(lv_display_get_screen_active(display));
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(indev, cursor_obj);
}

static void discovery_cb(lv_indev_t * indev, lv_evdev_type_t type, void * user_data)
{
    LV_LOG_USER("new '%s' device discovered", type == LV_EVDEV_TYPE_REL ? "REL" :
                type == LV_EVDEV_TYPE_ABS ? "ABS" :
                type == LV_EVDEV_TYPE_KEY ? "KEY" :
                "unknown");

    lv_display_t * disp = user_data;
    lv_indev_set_display(indev, disp);

    if(type == LV_EVDEV_TYPE_REL) {
        set_mouse_cursor_icon(indev, disp);
    }
}
#endif /*LV_USE_EVDEV*/

static size_t drm_mode_select_cb(lv_display_t * disp, const lv_linux_drm_mode_t * modes, size_t mode_count)
{
    LV_UNUSED(disp);
    size_t preferred_mode = mode_count;
    for(size_t i = 0; i < mode_count; i++) {
        int32_t width = lv_linux_drm_mode_get_horizontal_resolution(&modes[i]);
        int32_t height = lv_linux_drm_mode_get_vertical_resolution(&modes[i]);
        int32_t refresh = lv_linux_drm_mode_get_refresh_rate(&modes[i]);
        bool preferred = lv_linux_drm_mode_is_preferred(&modes[i]);

        LV_LOG_USER("Mode %zu %dx%d@%d %d %d %d", i, width, height, refresh, preferred, modes[i].vsync_start,
                    modes[i].vsync_end);
        if(preferred) {
            preferred_mode = i;
        }
    }
    return preferred_mode;
}


int main(void)
{
    lv_init();
    lv_display_t * disp = lv_linux_drm_create();
    if(!disp) {
        fprintf(stderr, "Failed to initialize drm\n");
        return EXIT_FAILURE;
    }
    uint32_t start_tick = lv_tick_get();
    char * device = lv_linux_drm_find_device_path();
    lv_linux_drm_set_mode_cb(disp, drm_mode_select_cb);
    lv_linux_drm_set_file(disp, device, -1);

#if LV_USE_EVDEV
    lv_evdev_discovery_start(discovery_cb, disp);
#endif

    LV_LOG_USER("DRM init time %u", lv_tick_get() - start_tick);
#if LV_USE_DEMO_GLTF
    uint32_t demo_tick = lv_tick_get();
    lv_demo_gltf("A:phoenix_bird_animated.glb");
    uint32_t end = lv_tick_get();
    LV_LOG_USER("demo init time %u (%u)", end - demo_tick, end - start_tick);
#else
    lv_demo_benchmark();
#endif


    while(1) {
        uint32_t x = lv_timer_handler();
        usleep(x * 1000);
    }
    lv_deinit();
    return 0;
}
