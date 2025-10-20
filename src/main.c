#include <lvgl.h>
#include <lv_demos.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

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
    LV_LOG_USER("DRM init time %u", lv_tick_get() - start_tick);
#if LV_USE_DEMO_GLTF
    uint32_t demo_tick = lv_tick_get();
    lv_demo_gltf("A:neo_virtual_city_touchup1.glb");
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
