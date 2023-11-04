#include <Windows.h>

#include <cairo-win32.h>

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

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) == TRUE)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

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
