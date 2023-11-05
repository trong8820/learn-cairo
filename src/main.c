#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <Windows.h>

#include <cairo-win32.h>

#include <png.h>

typedef struct png_io
{
	uint8_t *buffer;
	size_t offset;
} png_io;

cairo_surface_t *icon_surface;

void png_read_fn(png_structp png, png_bytep data, png_size_t length)
{
	png_io *io = (png_io *)png_get_io_ptr(png);
	memcpy(data, io->buffer + io->offset, length);
	io->offset += length;
}

inline int multiply_alpha(int alpha, int color)
{
	int temp = (alpha*color) + 0x80;
	return ((temp + (temp >> 8)) >> 8);
}

void premultiply_data(png_structp png, png_row_infop row_info, png_bytep data)
{
	unsigned int i;
	for (i = 0; i < row_info->rowbytes; i += 4) {
		uint8_t *base = &data[i];
		uint8_t  alpha = base[3];
		uint32_t p;
		if (alpha == 0) {
			p = 0;
		}
		else {
			uint8_t  red = base[0];
			uint8_t  green = base[1];
			uint8_t  blue = base[2];
			if (alpha != 0xff) {
				red = multiply_alpha(alpha, red);
				green = multiply_alpha(alpha, green);
				blue = multiply_alpha(alpha, blue);
			}
			p = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
		}
		memcpy(base, &p, sizeof(uint32_t));
	}
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASSEX wcex =
	{
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = NULL,
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.hbrBackground = GetStockObject(WHITE_BRUSH),
		.lpszMenuName = NULL,
		.lpszClassName = "LEARN_CAIRO_WIN_CLASS",
		.hIconSm = NULL
	};
	if (RegisterClassEx(&wcex) == 0u)
	{
		return FALSE;
	}

	HWND hWnd = CreateWindowEx
	(
		WS_EX_OVERLAPPEDWINDOW,
		"LEARN_CAIRO_WIN_CLASS",
		"Learn Cairo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL
	);
	if (hWnd == NULL)
	{
		UnregisterClass("LEARN_CAIRO_WIN_CLASS", hInstance);
		return FALSE;
	}

	FILE *file = fopen("data/icon.png", "rb");
	long current_position = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t file_length = (size_t)ftell(file);
	fseek(file, current_position, SEEK_SET);
	uint8_t *buffer = (uint8_t *)malloc(file_length*sizeof(uint8_t));
	assert(buffer);
	fread(buffer, 1, file_length, file);
	fclose(file);

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop png_info = png_create_info_struct(png);

	png_io io =
	{
		.buffer = buffer,
		.offset = 0
	};
	png_set_read_fn(png, &io, &png_read_fn);
	png_read_info(png, png_info);
	png_uint_32 icon_width = png_get_image_width(png, png_info);
	png_uint_32 icon_height = png_get_image_height(png, png_info);
	size_t icon_rowbytes = png_get_rowbytes(png, png_info);
	png_bytep *icon_bytes = (png_bytep *)malloc(icon_height*sizeof(png_bytep));
	uint8_t *pixels = (uint8_t *)malloc(icon_width*icon_rowbytes);
	assert(pixels);
	png_bytep *row_pointers = (png_bytep *)malloc(icon_height*sizeof(png_bytep));
	assert(row_pointers);
	for (size_t j = 0; j < icon_height; ++j)
	{
		row_pointers[j] = pixels + j*icon_rowbytes;
	}
	png_set_read_user_transform_fn(png, premultiply_data);
	png_read_image(png, row_pointers);
	png_read_end(png, png_info);

	icon_surface = cairo_image_surface_create_for_data(pixels, CAIRO_FORMAT_ARGB32, icon_width, icon_height, icon_rowbytes);

	free(row_pointers);
	png_destroy_read_struct(&png, &png_info, NULL);
	free(buffer);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) == TRUE)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	cairo_surface_destroy(icon_surface);
	free(pixels);
	DestroyWindow(hWnd);
	UnregisterClass("LEARN_CAIRO_WIN_CLASS", hInstance);
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(hWnd, &ps);
		cairo_surface_t *surface = cairo_win32_surface_create(hdc);
		cairo_t *cairo = cairo_create(surface);
		cairo_rectangle(cairo, 100.0, 100.0, 200.0, 300.0);
		cairo_stroke(cairo);
		cairo_select_font_face(cairo, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(cairo, 24.0);
		cairo_move_to(cairo, 100.0, 50.0);
		cairo_show_text(cairo, "Xin chao");

		cairo_set_source_surface(cairo, icon_surface, 320.0, 20.0);
		cairo_paint(cairo);
		cairo_set_source_surface(cairo, icon_surface, 520.0, 20.0);
		cairo_paint(cairo);

		cairo_destroy(cairo);
		cairo_surface_destroy(surface);
		EndPaint(hWnd, &ps);
	}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
