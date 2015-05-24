#ifndef image_h
#define image_h

#include <assert.h>
#include "common_header.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ENUM_IMAGE_FORMAT {
	IMG_FORMAT_NULL = 0,
	IMG_FORMAT_RGBA,
	IMG_FORMAT_BGRA,
	IMG_FORMAT_RGB,
	IMG_FORMAT_BGR,
};

class image_data {
public:
	uint width;
	uint height;
	enum ENUM_IMAGE_FORMAT format;
	uchar *pixels;

	image_data()
		:width(0), height(0), format(IMG_FORMAT_NULL), 
		pixels(nullptr) {
		
	}

	~image_data() {
		if (pixels != nullptr) {
			delete[] pixels;
			pixels = nullptr;
		}
	}
};

void img_lib_init();
void img_lib_dispose();
void img_load_image(const char *path, image_data *data);
void img_save_image(const char *path, image_data *data);
void img_gray_scale(image_data *data);

#ifdef __cplusplus
}
#endif

#endif