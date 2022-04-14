#pragma once

#include "BaseWindow.hpp"
#include "TabWindow.hpp"

class OverlappedWindow : public BaseWindow <OverlappedWindow> {
protected:
	LPCWSTR ClassName() const {
		return L"OverlappedWindow";
	}

public:
	OverlappedWindow(HINSTANCE hInst) : BaseWindow(hInst) {
		Register();
	}
	/*
	* @param {UINT} message code
	* @param {WPARAM} depends on message code
	* @param {LPARAM} depends on message code
	**/
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};