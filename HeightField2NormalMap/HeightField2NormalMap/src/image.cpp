#include <windows.h>
#include <wincodec.h>
#include <assert.h>
#include <stdio.h>
#include <atlbase.h>

#include "image.h"

#define CHECK(S) assert((S) == S_OK)

typedef struct image_handler {
	IWICImagingFactory *factory;
	IWICBitmapDecoder *decoder;
	IWICBitmapFrameDecode *frameDecode;
	IWICBitmapEncoder *encoder;
	IWICBitmapFrameEncode *frameEncode;
} image_handler;

static image_handler *s_handler = nullptr;

static wchar_t* _char2wchar_t(const char* str) {
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if (len == 0){
		return NULL;
	}
	wchar_t* buf = (wchar_t*)malloc(sizeof(wchar_t)*(len + 1));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, len);

	return buf;
}

void img_lib_init() {
	::CoInitialize(NULL);
	if (s_handler == nullptr) {
		s_handler = new image_handler();
		CHECK(CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&s_handler->factory)
			));
	}
}

void img_lib_dispose() {
	if (s_handler != nullptr) {
		if (s_handler->factory != nullptr)
			s_handler->factory->Release();
		if (s_handler->decoder != nullptr)
			s_handler->decoder->Release();
		if (s_handler->frameDecode != nullptr)
			s_handler->frameDecode->Release();
		delete s_handler;
		s_handler = nullptr;
	}
	::CoUninitialize();
}

static uint _get_image_stride(uint width, uint bpp) {
	assert(bpp % 8 == 0);
	uint byteCount = bpp / 8;
	uint stride = (width * byteCount + 3) & ~3;
	assert(stride % sizeof(DWORD) == 0);
	return stride;
}

void img_load_image(const char *path, image_data *data) {
	if (s_handler == nullptr) {
		img_lib_init();
	}

	wchar_t *wpath = _char2wchar_t(path);
	CHECK(s_handler->factory->CreateDecoderFromFilename(
		wpath,
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&s_handler->decoder
		));

	if (s_handler->frameDecode != nullptr)
		s_handler->frameDecode->Release();

	CHECK(s_handler->decoder->GetFrame(0, &s_handler->frameDecode));

	s_handler->frameDecode->GetSize(&data->width, &data->height);
	GUID pixel_format = { 0 };
	s_handler->frameDecode->GetPixelFormat(&pixel_format);

	uint stride = 0;
	if (IsEqualGUID(pixel_format, GUID_WICPixelFormat32bppRGBA)) {
		data->format = IMG_FORMAT_RGBA;
		stride = _get_image_stride(data->width, 32);
	}
	else if (IsEqualGUID(pixel_format, GUID_WICPixelFormat32bppBGRA)) {
		data->format = IMG_FORMAT_BGRA;
		stride = _get_image_stride(data->width, 32);
	}
	else if (IsEqualGUID(pixel_format, GUID_WICPixelFormat32bppBGR)) {
		data->format = IMG_FORMAT_BGR;
		stride = _get_image_stride(data->width, 32);
	}
	else if (IsEqualGUID(pixel_format, GUID_WICPixelFormat24bppRGB)) {
		data->format = IMG_FORMAT_RGB;
		stride = _get_image_stride(data->width, 24);
	}
	else if (IsEqualGUID(pixel_format, GUID_WICPixelFormat24bppBGR)) {
		data->format = IMG_FORMAT_BGR;
		stride = _get_image_stride(data->width, 24);
	}
	else {
		data->format = IMG_FORMAT_NULL;
		printf("WIC Unsupported image format! the filename is:[%s]", path);
		return;
	}
	
	uint size = stride * data->height;
	if (data->pixels != nullptr)
		delete[] data->pixels;
	data->pixels = new uchar[size];
	CHECK(s_handler->frameDecode->CopyPixels(
			nullptr,
			stride,
			size,
			data->pixels
		));
}

void img_save_image(const char *path, image_data *data) {
	if (s_handler == nullptr) {
		printf("need load image first......");
		return;
	}

	wchar_t *wpath = _char2wchar_t(path);
	CComPtr<IWICStream> stream;
	CHECK(s_handler->factory->CreateStream(&stream));
	CHECK(stream->InitializeFromFilename(wpath, GENERIC_WRITE));
	CHECK(s_handler->factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &s_handler->encoder));
	CHECK(s_handler->encoder->Initialize(stream, WICBitmapEncoderNoCache));
	CHECK(s_handler->encoder->CreateNewFrame(&s_handler->frameEncode, nullptr));
	CHECK(s_handler->frameEncode->Initialize(nullptr));
	CHECK(s_handler->frameEncode->SetSize(data->width, data->height));
	WICPixelFormatGUID pf = GUID_WICPixelFormat24bppRGB;
	CHECK(s_handler->frameEncode->SetPixelFormat(&pf));

	uint stride = _get_image_stride(data->width, 24);
	uint size   = stride * data->height;
	CHECK(s_handler->frameEncode->WritePixels(data->height, stride, size, data->pixels));
	CHECK(s_handler->frameEncode->Commit());
	CHECK(s_handler->encoder->Commit());
}