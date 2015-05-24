#include <stdio.h>

#include "image.h"

static image_data s_image;

int main(int argc, char *argv[]) {
	img_lib_init();
	img_lib_dispose();
	printf("Done!");
	getchar();
	return 0;
}