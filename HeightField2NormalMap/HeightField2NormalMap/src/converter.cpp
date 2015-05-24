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

static void _convert_rgb(image_data *height_img) {
	uchar *pixels = height_img->pixels;
	uint stride = _get_image_stride(height_img->width, 24);
	for (int row = 0; row < height_img->height; row++) {
		for (int col = 0; col < stride; col++) {
			if (col + 3 > stride)
				continue;
			int i = row * stride + col;
			uchar r = pixels[i];
			uchar g = pixels[i + 1];
			uchar b = pixels[i + 2];
			int hg = _combine_rgb(r, g, b);

			//above.
			int ha = 0;
			if (row == 0) {
				ha = hg;
			}
			else {
				i = (row - 1) * stride + col;
				uchar r = pixels[i];
				uchar g = pixels[i + 1];
				uchar b = pixels[i + 2];
				ha = _combine_rgb(r, g, b);
			}
			//right.
			int hr = 0;
			if (col + 6 > stride) {
				hr = hg;
			}
			else {
				i = row * stride + col + 1;
				uchar r = pixels[i];
				uchar g = pixels[i + 1];
				uchar b = pixels[i + 2];
				hr = _combine_rgb(r, g, b);
			}

			int diff_ga = hg - ha;
			int diff_gr = hg - hr;
			float vl = sqrt(diff_ga * diff_ga + diff_gr * diff_gr + 1);
			s_normal_temp.push_back(diff_ga / vl);
			s_normal_temp.push_back(diff_gr / vl);
			s_normal_temp.push_back(1 / vl);
		}
	}
}

static void _convert_bgr(image_data *height_img) {
	uchar *pixels = height_img->pixels;
	uint stride = _get_image_stride(height_img->width, 24);
	for (int row = 0; row < height_img->height; row++) {
		for (int col = 0; col < stride; col++) {
			if (col + 3 > stride)
				continue;
			int i = row * stride + col;
			uchar b = pixels[i];
			uchar g = pixels[i + 1];
			uchar r = pixels[i + 2];
			int hg = _combine_rgb(b, g, r);

			//above.
			int ha = 0;
			if (row == 0) {
				ha = hg;
			}
			else {
				i = (row - 1) * stride + col;
				uchar b = pixels[i];
				uchar g = pixels[i + 1];
				uchar r = pixels[i + 2];
				ha = _combine_rgb(b, g, r);
			}
			//right.
			int hr = 0;
			if (col + 6 > stride) {
				hr = hg;
			}
			else {
				i = row * stride + col + 1;
				uchar b = pixels[i];
				uchar g = pixels[i + 1];
				uchar r = pixels[i + 2];
				hr = _combine_rgb(b, g, r);
			}

			int diff_ga = hg - ha;
			int diff_gr = hg - hr;
			float vl = sqrt(diff_ga * diff_ga + diff_gr * diff_gr + 1);
			s_normal_temp.push_back(1 / vl);
			s_normal_temp.push_back(diff_gr / vl);
			s_normal_temp.push_back(diff_ga / vl);
		}
	}
}

static void _convert_rgba(image_data *height_img) {
	//TODO...
}

static void _convert_bgra(image_data *height_img) {
	//TODO...
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

	switch (height_img->format) {
	case IMG_FORMAT_RGB:
		_convert_rgb(height_img);
		break;
	case IMG_FORMAT_BGR:
		_convert_bgr(height_img);
		break;
	case IMG_FORMAT_RGBA:
		//TODO...
		assert(false);
		break;
	case IMG_FORMAT_BGRA:
		//TODO...
		assert(false);
		break;
	default:
		assert(false);
		break;
	}

	uchar *pixels = normal_img->pixels;
	for (int row = 0; row < normal_img->height; row++) {
		for (int col = 0; col < stride; col++) {
			int i = row * stride + col;
			pixels[i] = (s_normal_temp[i] + 1) * 125;
			pixels[i + 1] = (s_normal_temp[i + 1] + 1) * 125;
			pixels[i + 2] = (s_normal_temp[i + 2] + 1) * 125;
		}
	}

	return normal_img;
}