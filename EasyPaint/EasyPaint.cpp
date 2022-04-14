#include "EasyPaint.h"
#include "resource.h"
#include "OverlappedWindow.hpp"
#include "TabWindow.hpp"
#include "Canvas.hpp"

#include <cassert>

HIMAGELIST hTBImageList;
HFONT hfSegoe;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdLine, int nCmdShow) {
	const INITCOMMONCONTROLSEX commAttrib = {
		sizeof(commAttrib),
		ICC_TAB_CLASSES
	};

	if (!InitCommonControlsEx(&commAttrib)) {
		OutputDebugString(L"Failed to load classes");
	}

	hfSegoe = CreateFont(15, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_MODERN, L"Segoe UI");

	HMENU hMainMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));

	// Main window for the application
	OverlappedWindow MainWindow(hInstance);
	MainWindow.Create(L"Easy Paint", WS_OVERLAPPEDWINDOW, NULL, NULL, NULL, 0, 0, 1280, 720);

	if (MainWindow.hWnd == NULL) {
		OutputDebugString(L"Could not create the window");
		return -1;
	}

	Canvas cv(hInstance, 1280, 720);

	cv.Create(L"", WS_CHILD | WS_VISIBLE, NULL, NULL, MainWindow.hWnd, 0, 0, 1280, 720);

	ShowWindow(MainWindow.hWnd, nCmdShow);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (hfSegoe) DeleteObject(hfSegoe);
	if (hMainMenu) DestroyMenu(hMainMenu);
	if (hTBImageList) ImageList_Destroy(hTBImageList);

	return 0;
}