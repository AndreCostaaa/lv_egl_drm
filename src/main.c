#include <lvgl.h>
#include <demos/lv_demo.h>
#include <unistd.h>

int main(void)
{
	const char *device = "/dev/dri/card0";
	lv_init();

	lv_display_t *disp = lv_linux_drm_create();
	if (!disp == NULL) {
		fprintf(stderr, "Failed to initialize drm\n");
		return EXIT_FAILURE;
	}
	lv_linux_drm_set_file(disp, device, -1);

	lv_demo_benchmark();

	while (1) {
		uint32_t time_until_next = lv_timer_handler();
		if (time_until_next == LV_NO_TIMER_READY) {
			time_until_next = LV_DEF_REFR_PERIOD;
		}
		usleep(time_until_next * 1000);
	}
	return 0;
}
