#pragma once

#include "EasyPaint.h"

// abstract class for all application defined window classes to derive from
template <class DERIVED_TYPE>
class BaseWindow {
protected:
	virtual LPCWSTR ClassName() const = 0;
	
	bool Register() {
		WNDCLASS wc = { 0 };

		wc.lpfnWndProc = static_cast <WNDPROC> (WndProc);
		wc.lpszClassName = ClassName();
		wc.hInstance = hInstance;

		return RegisterClass(&wc);
	}

public:
	HWND hWnd;
	HINSTANCE hInstance;
	RECT rect;

	D2D1_COLOR_F bgColor;

	BaseWindow(HINSTANCE hInst, D2D1_COLOR_F bg = D2D1::ColorF(0.15f, 0.15f, 0.15f, 1.0f)) {
		hInstance = hInst;
		hWnd = NULL;

		rect = { 0, 0, 0, 0 };
		bgColor = bg;
	}

	/*
	* @brief Creates a window of the class type
	**/
	bool Create(
		LPCWSTR	wndTitle,
		DWORD	dwStyle,
		DWORD	dwExStyle = NULL,
		HMENU	hMenu = NULL,
		HWND	hWndParent = NULL,
		int		x = CW_USEDEFAULT,
		int		y = CW_USEDEFAULT,
		int		width = CW_USEDEFAULT,
		int		height = CW_USEDEFAULT
	) {
		HWND tempHWnd;

		tempHWnd = CreateWindowEx(dwExStyle, ClassName(), wndTitle, dwStyle, x, y, width, height, hWndParent, hMenu, hInstance, static_cast <LPVOID> (this));

		// for user defined class hWnd is set in WM_NCCREATE
		// for default window classes, it will be returned by CreateWindowEx

		if (hWnd == NULL) {
			hWnd = tempHWnd;
			DERIVED_TYPE* pDerived = static_cast <DERIVED_TYPE*> (this);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast <LONG_PTR> (pDerived));
		}

		if (hWnd) {
			GetClientRect(hWnd, &rect);
		}

		return hWnd != NULL;
	}

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		DERIVED_TYPE* pDerived = nullptr;
		BaseWindow* pBase = nullptr;

		// if WM_NCCRETE is being handled here, the hWnd needs to be assigned at this stage
		// not doing so would lead to losing the hwnd

		if (uMsg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast <CREATESTRUCT*> (lParam);
			pDerived = static_cast <DERIVED_TYPE*> (pCreate->lpCreateParams);

			pDerived->hWnd = hWnd;

			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast <LONG_PTR> (pDerived));
		}
		else {
			pDerived = reinterpret_cast <DERIVED_TYPE*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		if (pDerived) pBase = pDerived;

		if (pBase) return pBase->HandleMessage(uMsg, wParam, lParam);
		else return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	/*
	* @brief Child window procedure called by EnumChildWindows
	* @param {HWND} handle to the child window
	* @param {LPARAM} application defined parameter
	**/
	static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam) {
		DERIVED_TYPE* pDerived = reinterpret_cast <DERIVED_TYPE*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (pDerived == nullptr) {
			return FALSE;
		}

		pDerived->Resize(reinterpret_cast <RECT*> (lParam));

		return TRUE;
	}

	/*
	* @brief window procedure sub-routine
	* @param {UINT} Message Code
	* @param {WPARAM} message dependent argument
	* @param {LPARAM} message dependent argument
	**/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	virtual void Resize(RECT *rc) { }

	virtual ~BaseWindow() { }
};