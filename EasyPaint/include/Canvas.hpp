#pragma once

#include "BaseWindow.hpp"

class Canvas : public BaseWindow <Canvas> {
private:
	ID2D1Factory*				pFactory;
	ID2D1HwndRenderTarget*		pRenderTarget;
	ID2D1BitmapRenderTarget*	pbRenderTarget;
	ID2D1SolidColorBrush*		pBgBrush;

	int mouseX = -1;
	int mouseY = -1;

	/*
	* @brief Safely release an interface pointer
	**/
	template <class T> 
	void SafeRelease(T** ppT) {
		if (*ppT) {
			(*ppT)->Release();
			*ppT = nullptr;
		}
	}

	/*
	* @brief initialize device dependent resources (called mutliple times to esnure the presence of the resources)
	* @return {HRESULT} S_OK on success
	*/
	HRESULT CreateGraphicsResources();

	/*
	* @brief Handles the paint operation upon receiving WM_PAINT
	*/
	void PaintCanvas();

	/*
	* @brief Paints the bitmap onto the canvas
	*/
	void PaintBitmap();

	/*
	* Safely Release all device dependent and independent resources
	*/
	void DiscardGraphicsResources();

	/*
	* @brief converts coordinates on the canvas window to coordinates on the bitmap lying inside
	* @param {D2D1_POINT_2F}	coordinates on the window
	* @return {D2D1_POINT_2F}	coordinates mapped onto the bitmap
	*/
	D2D1_POINT_2F MapCoordinate(D2D1_POINT_2F &pos);

protected:
	LPCWSTR ClassName() const {
		return L"Canvas";
	}

public:
	D2D1_SIZE_F bitmapSize;

	/*
	* @param {HINSTANCE} application hInstance
	* @param {UINT} width of the bitmap
	* @param {UINT} height o the bitmap
	*/
	Canvas(HINSTANCE hInst, unsigned int width, unsigned int height): BaseWindow(hInst) {
		Register();

		pFactory = nullptr;
		pRenderTarget = nullptr;
		pbRenderTarget = nullptr;
		pBgBrush = nullptr;

		bitmapSize = D2D1::SizeF(width, height);
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual ~Canvas() {
		DiscardGraphicsResources();
	}
};