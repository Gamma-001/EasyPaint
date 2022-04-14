#include "TabWindow.hpp"

std::wstring TabWindow::GetTabName(int i) {
	if (i >= 0 && i < tabNames.size()) {
		return tabNames[i];
	}

	return L"";
}

// extend or decrease the width of the tab window based on the parent window
void TabWindow::Resize(RECT* rc) {

	SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, rect.right + rc->right, rect.bottom, SWP_NOZORDER | SWP_NOACTIVATE);

	rect.right += rc->right;
}

void TabWindow::AddTab(std::wstring str) {
	TCITEM tcItem = { 0 };

	LPWSTR temp = new WCHAR[256];
	wcscpy_s(temp, str.size() + 1, str.c_str());

	tcItem.mask = TCIF_TEXT | TCIF_IMAGE;
	tcItem.iImage = -1;
	tcItem.pszText = temp;

	TabCtrl_InsertItem(hWnd, nTabs, &tcItem);

	nTabs += 1;
	tabNames.push_back(str);

	delete[] temp;
}