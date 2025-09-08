#include <lvgl.h>
#include <lv_demos.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char *device = NULL;
	if (argc < 2) {
		device = "/dev/dri/card0";
	} else {
		device = argv[1];
	}

	lv_display_t *disp = lv_linux_drm_create();
	if (!disp) {
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
