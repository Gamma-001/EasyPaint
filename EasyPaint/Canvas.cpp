#include "Canvas.hpp"

// inherited methods

LRESULT Canvas::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		// initiaize all device independent resources inside WM_CREATE
	case WM_CREATE:
	{
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
		if (FAILED(hr)) {
			return -1;
		}
		break;
	}
	case WM_PAINT:
		PaintCanvas();
		return 0;
	
	// user inputs

	case WM_LBUTTONDOWN:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		return 0;

	// WPARAM : indicates whether virtual keys are down
	// LPARAM : (xpos, ypos)
	case WM_MOUSEMOVE:
	{
		int newX = GET_X_LPARAM(lParam);
		int newY = GET_Y_LPARAM(lParam);

		// draw a line from last mouse position to current
		if ((wParam & MK_LBUTTON) && mouseX != -1) {
			SetCapture(hWnd);

			// note: handle operation failures later
			HRESULT hr = S_OK;

			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			pbRenderTarget->BeginDraw();

			D2D1_POINT_2F point1 = D2D1::Point2F(mouseX, mouseY);
			D2D1_POINT_2F point2 = D2D1::Point2F(newX, newY);

			pbRenderTarget->DrawLine(MapCoordinate(point1), MapCoordinate(point2), pBgBrush, 3.0f);

			hr = pbRenderTarget->EndDraw();
			EndPaint(hWnd, &ps);

			if (SUCCEEDED(hr)) {
				pRenderTarget->BeginDraw();

				PaintBitmap();

				hr = pRenderTarget->EndDraw();
			}

			mouseX = newX;
			mouseY = newY;
		}

		return 0;
	}

	// release the mouse capture and mark the mouse position as uninitialized
	// this will prevent line jumps (line between positions that aren't connected)
	case WM_LBUTTONUP:
		mouseX = -1;
		mouseY = -1;
		ReleaseCapture();
		return 0;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


// non inherited class methods


HRESULT Canvas::CreateGraphicsResources() {
	HRESULT hr = S_OK;

	// no need to initialize resources if they aren't destroyed or unitialized
	if (pRenderTarget != nullptr) return hr;

	// Create the main Hwnd render target
	D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);
	hr = pFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, size),
		&pRenderTarget
	);

	if FAILED(hr) return hr;

	// create a bitmap render target compatible with the hWnd render target
	hr = pRenderTarget->CreateCompatibleRenderTarget(bitmapSize, &pbRenderTarget);

	if (FAILED(hr)) return hr;

	pbRenderTarget->BeginDraw();

	// the bitmap starts of with a white background
	pbRenderTarget->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));

	pbRenderTarget->EndDraw();

	// create other device dependent resources
	const D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f);
	hr = pRenderTarget->CreateSolidColorBrush(color, &pBgBrush);

	if (FAILED(hr)) return hr;

	// if (SUCCEEDED(hr)) OutputDebugString(L"All good");
	return hr;
}

void Canvas::PaintCanvas() {
	HRESULT hr = CreateGraphicsResources();
	if (FAILED(hr)) return;

	// this most likely won't be used
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

	pbRenderTarget->BeginDraw();

	hr = pbRenderTarget->EndDraw();
	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
		OutputDebugString(L"Something went wrong while drawing to the bitmap");
		DiscardGraphicsResources();
	}

	pRenderTarget->BeginDraw();
	pRenderTarget->Clear(D2D1::ColorF(bgColor.r * 0.8f, bgColor.g * 0.8f, bgColor.b * 0.8f, 1.0f));

	PaintBitmap();

	hr = pRenderTarget->EndDraw();
	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
		OutputDebugString(L"Something went wrong while drawing the canvas");
		DiscardGraphicsResources();
	}

	EndPaint(hWnd, &ps);
}

void Canvas::PaintBitmap() {
	ID2D1Bitmap* bitmap;
	pbRenderTarget->GetBitmap(&bitmap);

	RECT rc;
	GetClientRect(hWnd, &rc);

	D2D1_RECT_F sourceRect = D2D1::RectF(0.0f, 0.0f, bitmapSize.width, bitmapSize.height);
	// stores the dimensions of the bitmap
	D2D1_RECT_F targetRect = sourceRect;

	// scale the bitmap to fit the canvas window correctly
	if (bitmap != nullptr) {
		float scaleX = rc.right / targetRect.right;
		float scaleY = rc.bottom / targetRect.bottom;

		float scale = scaleX <= scaleY ? scaleX : scaleY;

		// scale the bitmap on both axis by the smallest axial scale
		targetRect.right *= scale;
		targetRect.bottom *= scale;

		// pad the canvas with empty space so the bitmap appears centerd
		float emptySpaceX = (rc.right - targetRect.right) / 2.0f;
		float emptySpaceY = (rc.bottom - targetRect.bottom) / 2.0f;

		targetRect.left += emptySpaceX;
		targetRect.right += emptySpaceX;

		targetRect.bottom += emptySpaceY;
		targetRect.top += emptySpaceY;

		// draw the bitmap
		pRenderTarget->DrawBitmap(bitmap, targetRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, sourceRect);
	}
}

void Canvas::DiscardGraphicsResources() {
	SafeRelease(&pBgBrush);
	SafeRelease(&pbRenderTarget);
	SafeRelease(&pRenderTarget);
	SafeRelease(&pFactory);
}


// utility functions


D2D1_POINT_2F Canvas::MapCoordinate(D2D1_POINT_2F &pos) {
	float scaleX = rect.right / bitmapSize.width;
	float scaleY = rect.bottom / bitmapSize.height;
	float scale = scaleX <= scaleY ? scaleX : scaleY;

	float emptySpaceX = (rect.right - bitmapSize.width * scale) / 2.0f;
	float emptySpaceY = (rect.bottom - bitmapSize.height * scale) / 2.0f;

	D2D1_POINT_2F res = pos;

	// shift the mouse position towards right and up by the amount of empty space
	// subtract the total empty space from the width of the canvas window
	// this would simulate the bitmap fitting the canvas perfectly
	res.x = (res.x - emptySpaceX) * (bitmapSize.width / (rect.right - emptySpaceX * 2));
	res.y = (res.y - emptySpaceY) * (bitmapSize.height / (rect.bottom - emptySpaceY * 2));

	return res;
}