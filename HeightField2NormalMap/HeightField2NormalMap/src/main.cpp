#include <stdio.h>

#include "image.h"
#include "converter.h"

static image_data s_image;

int main(int argc, char *argv[]) {
	img_lib_init();
	img_load_image("E://test.jpg", &s_image);
	image_data *normal = convert(&s_image);
	img_save_image("E://test_normal.jpg", normal);
	img_lib_dispose();

	printf("Done!");
	getchar();
	return 0;
}