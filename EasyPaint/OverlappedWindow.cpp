#include "OverlappedWindow.hpp"
#include "TabWindow.hpp"

LRESULT OverlappedWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

		// called whenever the client area updates
		// WPARAM, LPARAM : Not used
	case WM_PAINT:
	{
		// retrieve the paint struct for the current window and pain the client region
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);

		FillRect(hDC, &ps.rcPaint, CreateSolidBrush(RGB(255 * bgColor.r, 255 * bgColor.g, 255 * bgColor.b)));

		EndPaint(hWnd, &ps);

	}
	return 0;

	// Send after the window has changed its size
	// WPARAM : type of resizing request (see docs)
	// LPARAM : (LOW: width, HIGH: height)
	case WM_SIZE:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);

		EnumChildWindows(hWnd, EnumChildProc, reinterpret_cast <LPARAM> (&rc));

		rect.right = rc.right;
		rect.bottom = rc.bottom;
	}
	return 0;

	// sent when a command item is selected
	// WPARAM: {Menu : (IDM_*, 0), Accelerator : (IDM_*, 1), Control: (identifier, notification code)}
	// LPARAM: {Menu, Accelerator : 0, Control : Handle to the control window}
	case WM_COMMAND:
	{
		std::wstring val = std::to_wstring(LOWORD(wParam));
		val += L"\n";

		// OutputDebugString(val.c_str());
	}
	break;

	// Sent by owner drawn controls
	// WPARAM : identifier of the control, 0 in case of a menu
	// LPARAM : DRAWITEMSTRUCT structure
	case WM_DRAWITEM:
	{
		//DRAWITEMSTRUCT* pDis = reinterpret_cast <DRAWITEMSTRUCT*> (lParam);
		//
		//// itemID will store the index of the tab item
		//int id = pDis->itemID;

		//TabWindow* tab = reinterpret_cast <TabWindow*> (GetWindowLongPtr(pDis->hwndItem, GWLP_USERDATA));

		//OutputDebugString(tab->GetTabName(id).c_str());

		//PAINTSTRUCT ps;
		//BeginPaint(pDis->hwndItem, &ps);

		//FillRect(pDis->hDC, &pDis->rcItem, CreateSolidBrush(RGB(10, 10, 10)));

		//EndPaint(pDis->hwndItem, &ps);
	}
	return TRUE;

	// Sent by common control
	// WPARAM : identifier fo the common control sending the message
	// LPARAM : pointer to NMHDR structure
	case WM_NOTIFY:
	{
		NMHDR* nmhdr = reinterpret_cast <NMHDR*> (lParam);

		switch (nmhdr->code) {
		case TCN_SELCHANGING:
			// retrun false to allow the selection to change
			return FALSE;

		case TCN_SELCHANGE:
		{
			// gets the index of the tab page
			int iPage = TabCtrl_GetCurSel(nmhdr->hwndFrom);

			TabWindow* pTab = reinterpret_cast <TabWindow*> (GetWindowLongPtr(nmhdr->hwndFrom, GWLP_USERDATA));

			// will be used later
		}
		break;
		}
	}
	break;

	// called whenever the application sends a DestroyWindow()
	// WPARAM, LPARAM: not used
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;

		// called when the user attemps to close the window
		// WPARAM, LPARAM: not used
	case WM_CLOSE:
		if (MessageBox(hWnd, L"Close the window?", L"", MB_YESNO) == IDYES) {
			DestroyWindow(hWnd);
		}
		return 0;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}