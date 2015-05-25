#include <assert.h>
#include <math.h>
#include <vector>

#include "common_header.h"
#include "converter.h"

static std::vector<float> s_normal_temp;

static uint _get_image_stride(uint width, uint bpp) {
	assert(bpp % 8 == 0);
	uint byteCount = bpp / 8;
	uint stride = (width * byteCount + 3) & ~3;
	return stride;
}

static inline int _combine_rgb(uchar r, uchar g, uchar b) {
	return ((r << 16) & 0xff0000) +
		((g << 8) & 0x00ff00) +
		(b & 0x0000ff);
}

static void _convert(image_data *height_img) {
	uint byte = 0;
	uint bpp = 0;

	ENUM_IMAGE_FORMAT format = height_img->format;
	switch (format) {
	case IMG_FORMAT_RGB:
	case IMG_FORMAT_BGR:
		byte = 3;
		bpp = 24;
		break;
	case IMG_FORMAT_RGBA:
	case IMG_FORMAT_BGRA:
		byte = 4;
		bpp = 32;
		break;
	default:
		assert(false);
		break;
	}

	uchar *pixels = height_img->pixels;
	uint stride = _get_image_stride(height_img->width, bpp);
	for (int row = 0; row < height_img->height; row++) {
		for (int col = 0; col < stride; col+=byte) {
			if (col + byte > stride)
				continue;
			int i = row * stride + col;
			int hg = pixels[i];

			//above.
			int ha = 0;
			if (row != 0) {
				i = (row - 1) * stride + col;
				ha = pixels[i];
			}
			//right.
			int hr = 0;
			if (col + byte * 2 <= stride) {
				i = row * stride + col + 1;
				hr = pixels[i];
			}

			int diff_ga = hg - ha;
			int diff_gr = hg - hr;
			float nr = diff_ga * 0.5 + 128;
			float ng = diff_gr * 0.5 + 128;
			float nb = 255;
			
			if (format == IMG_FORMAT_RGB || format == IMG_FORMAT_RGBA) {
				s_normal_temp.push_back(nr);
				s_normal_temp.push_back(ng);
				s_normal_temp.push_back(nb);
			}
			else if (format == IMG_FORMAT_BGR || format == IMG_FORMAT_BGRA) {
				s_normal_temp.push_back(nb);
				s_normal_temp.push_back(ng);
				s_normal_temp.push_back(nr);
			}
		}
	}
}

image_data * convert(image_data *height_img) {
	image_data *normal_img = new image_data();
	normal_img->width = height_img->width;
	normal_img->height = height_img->height;
	uint stride = _get_image_stride(normal_img->width, 24);
	uint size = normal_img->height * stride;
	normal_img->pixels = new uchar[size];

	s_normal_temp.clear();
	img_gray_scale(height_img);
	_convert(height_img);

	uchar *pixels = normal_img->pixels;
	for (int row = 0; row < normal_img->height; row++) {
		for (int col = 0; col < stride; col+=3) {
			if (col + 3 > stride)
				continue;
			int i = row * stride + col;
			pixels[i] = s_normal_temp[i];
			pixels[i + 1] = s_normal_temp[i + 1];
			pixels[i + 2] = s_normal_temp[i + 2];
		}
	}

	return normal_img;
}