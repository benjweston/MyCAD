#include "SYS_systemwin32.h"

#include "EDL_dialog_drawingwindowcolours.h"
#include "EDL_resource.h"

//#include "DRW_viewportmanager.h"

#include "resources.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>
#include <commctrl.h>
#include <intsafe.h>//Required for IntPtrToInt
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.

using namespace mycad;

dlg_DrawingWindowColours::dlg_DrawingWindowColours()
	: CommandDialog(),
	m_hContext(0),
	m_hInterfaceElement(0),
	m_hColour(0),
	m_hOK(0),
	m_hCancel(0),
	m_hHelp(0)
{

}
dlg_DrawingWindowColours::~dlg_DrawingWindowColours()
{

}

int dlg_DrawingWindowColours::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL nResult = FALSE;
	switch (LOWORD(wParam)) {
		case IDOK: {
			RECT rcWindow = { 0,0,0,0 };
			GetWindowRect(m_hWnd, &rcWindow);
			setWindowState(WINDOWSTATE_POSITION, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DrawingWindowColours");

			nResult = ::EndDialog(m_hWnd, wParam);

			break;
		}
		case IDCANCEL: {
			nResult = ::EndDialog(m_hWnd, wParam);
			break;
		}
		case IDHELP: {

			break;
		}
	}
	return (int)nResult;
}
int dlg_DrawingWindowColours::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HRESULT hResult = S_OK;
	INT nResult = 0;

	INT_PTR npBrush = (INT_PTR)::CreateSolidBrush(RGB(240, 240, 240));

	hResult = ::IntPtrToInt(npBrush, &nResult);
	if (hResult == S_OK) {
		return nResult;
	}

	return FALSE;
}
int dlg_DrawingWindowColours::wm_destroy()
{
	return FALSE;
}
int dlg_DrawingWindowColours::wm_erasebkgnd(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);
	return FALSE;
}
int dlg_DrawingWindowColours::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	setIcon(m_hWnd, MAKEINTRESOURCE(IDI_MYCAD));

	m_hContext = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAWINGWINDOWCOLOURS_LTEXT_CONTEXT);
	m_hInterfaceElement = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAWINGWINDOWCOLOURS_LTEXT_INTERFACEELEMENT);
	m_hColour = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAWINGWINDOWCOLOURS_LTEXT_COLOUR);

	m_hOK = ::GetDlgItem(m_hWnd, IDOK);
	m_hCancel = ::GetDlgItem(m_hWnd, IDCANCEL);
	m_hHelp = ::GetDlgItem(m_hWnd, IDHELP);

	RECT rcWindow = { 0,0,0,0 };
	getWindowState(WINDOWSTATE_POSITION, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DrawingWindowColours");

	::SetWindowPos(m_hWnd,
		HWND_TOP,
		rcWindow.left,
		rcWindow.top,
		576,
		548,
		SWP_SHOWWINDOW | SWP_NOSIZE
	);

	return FALSE;
}
int dlg_DrawingWindowColours::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	BOOL nResult = FALSE;

	//m_lpFiles.m_notify();

#define lpnm (LPNMHDR(lParam))

	switch (lpnm->code) {
		case TCN_SELCHANGE: {
			//wm_notify_tcn_selchange(m_hWnd);
			nResult = TRUE;
			break;
		}
		case TCN_SELCHANGING: {//Tabcontrol selection changing

			break;
		}
		default: {
			break;
		}
	}

	return (int)nResult;
}
int dlg_DrawingWindowColours::wm_paint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);

	::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);

	return TRUE;
}
int dlg_DrawingWindowColours::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	::SetWindowPos(m_hOK,
		NULL,
		m_cx - 88 - 81 - 81,
		m_cy - 30,
		75,
		23,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hCancel,
		NULL,
		m_cx - 88 - 81,
		m_cy - 30,
		75,
		23,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hHelp,
		NULL,
		m_cx - 88,
		m_cy - 30,
		75,
		23,
		SWP_SHOWWINDOW
	);

	return TRUE;
}
int dlg_DrawingWindowColours::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::dlg_DrawingWindowColours::wm_syscommand() SC_CLOSE");
			break;
		}
		default: {
			break;
		}
	}

	return FALSE;
}
