#pragma once

#include "BaseWindow.hpp"

class TabWindow : public BaseWindow <TabWindow> {
private:
	int nTabs;
	std::vector<std::wstring> tabNames;

protected:
	LPCWSTR ClassName() const {
		return WC_TABCONTROL;
	}

public:
	TabWindow(HINSTANCE hInst) : BaseWindow(hInst) {
		nTabs = 0;
	}

	// would not be used because this is a control class
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) { }

	/*
	* @brief Returs the inner text of a tab at the given index
	* @param {int} index of the tab
	* @return {std::wstring} inner text of the tab 
	*/
	std::wstring GetTabName(int i);

	/*
	* @brief resize the tab window in response to parent window size change
	* @param {RECT} stores difference of previous and current parent window size
	**/
	void Resize(RECT* rc);

	/*
	* @brief Adds a new tab to the tab control
	**/
	void AddTab(std::wstring str);
};