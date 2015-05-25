#include <stdio.h>
#include <windows.h>
#include <string>
using std::string;

#include "image.h"
#include "converter.h"
#include "utils.h"

#define MAX_PATH 120
static image_data s_image;

bool is_absolute_path(const char *path) {
	char c;
	while (c = *path++) {
		if (c == ':')
			return true;
	}
	return false;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: HeightField2NormalMap height.png normal.png");
		return -1;
	}

	wchar_t wbuffer[MAX_PATH];
	GetModuleFileName(nullptr, wbuffer, MAX_PATH);
	char *buffer = wchar_t2char(wbuffer);
	string::size_type pos = string(buffer).find_last_of("\\");
	string cwd = string(buffer).substr(0, pos);

	string input_path;
	if (is_absolute_path(argv[1]))
		input_path = argv[1];
	else
		input_path = cwd + "\\" + argv[1];

	string output_path;
	if (is_absolute_path(argv[2]))
		output_path = argv[2];
	else
		output_path = cwd + "\\" + argv[2];

	img_lib_init();
	img_load_image(input_path.c_str(), &s_image);
	image_data *normal = convert(&s_image);
	img_save_image(output_path.c_str(), normal);

	img_lib_dispose();

	printf("Done!");
	getchar();
	return 0;
}