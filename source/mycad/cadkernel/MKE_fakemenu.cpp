#include "MKE_fakemenu.h"
#include "MKE_resource.h"

#include "fault.h"
#include "log.h"

#include <commctrl.h>
#include <windowsx.h>

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) });

using namespace mycad;

int Fakemenu::s_nSpacing = 30;

Fakemenu::Fakemenu()
	: WindowWin32(),
	m_ptMouseDown({ 0,0 }),
	m_nPosition(0)
{
	log("Log::Fakemenu::Fakemenu()");
}
Fakemenu::~Fakemenu()
{
	log("Log::Fakemenu::~Fakemenu()");
}

int Fakemenu::wm_activate(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	m_nActive = LOWORD(wParam);

	switch (LOWORD(wParam)) {
		case WA_ACTIVE:
		case WA_CLICKACTIVE: {
			log("Log::Fakemenu::wm_activate() WA_ACTIVE");
			break;
		}
		case WA_INACTIVE: {
			log("Log::Fakemenu::wm_activate() WA_INACTIVE");
			break;
		default:
			break;
		}
	}

	return 0;
}
int Fakemenu::wm_char(WPARAM wParam, LPARAM lParam)//Contains functions for editing the command text buffer.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_char()");

	return 0;
}
int Fakemenu::wm_close()
{
	log("Log::Fakemenu::wm_close()");

	::DestroyWindow(m_hWnd);

	return 0;
}
int Fakemenu::wm_command(WPARAM wParam, LPARAM lParam)
{
	log("Log::Fakemenu::wm_command()");

	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	int nSource = HIWORD(wParam);

	switch (nSource) {
		case BN_CLICKED: {
			log("Log::Fakemenu::wm_command() BN_CLICKED");
			switch (nID) {
				case IDC_STATUSBAR_COORDINATES: {
					log("Log::Fakemenu::wm_command() BN_CLICKED IDC_STATUSBAR_COORDINATES");
					break;
				}
				case IDC_STATUSBAR_MODELSPACE: {
					log("Log::Fakemenu::wm_command() BN_CLICKED IDC_STATUSBAR_MODELSPACE");
					break;
				}
				case IDC_STATUSBAR_GRID: {
					log("Log::Fakemenu::wm_command() BN_CLICKED IDC_STATUSBAR_GRID");
					break;
				}
				case IDC_STATUSBAR_SNAPMODE: {
					log("Log::Fakemenu::wm_command() BN_CLICKED IDC_STATUSBAR_SNAPMODE");
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		default: {
			log("Log::Application::wm_command() default");
		}
	}

	return 0;
}
int Fakemenu::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_create()");

	//HRESULT hResult = S_OK;
	//
	//CREATESTRUCT* lpCreate = (CREATESTRUCT*)lParam;
	//RECT rc = { 5, 0, lpCreate->cx - 10, 31 };
	//
	//char szText[MAX_LOADSTRING]{ 0 };
	//::LoadString(s_hInstance, ID_SHORTCUT_CUSTOMIZATION_COORDINATES, szText, sizeof(szText) / sizeof(char));
	//
	//HWND hWnd = ::CreateWindow(
	//	WC_BUTTON,
	//	szText,
	//	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
	//	rc.left,
	//	rc.top,
	//	rc.right,
	//	rc.bottom,
	//	m_hWnd,
	//	(HMENU)IDC_STATUSBAR_COORDINATES,
	//	s_hInstance,
	//	(LPVOID)0
	//);
	//
	//if (hWnd == NULL) {
	//	hResult = ErrorHandler();
	//}
	//
	//if (SUCCEEDED(hResult)) {
	//	setFont(hWnd);
	//}

	m_hCheckbox[0] = createCheckbox(ID_SHORTCUT_CUSTOMIZATION_COORDINATES, IDC_STATUSBAR_COORDINATES);
	//createCheckbox(ID_SHORTCUT_CUSTOMIZATION_MODELSPACE, IDC_STATUSBAR_MODELSPACE);
	//createCheckbox(ID_SHORTCUT_CUSTOMIZATION_GRID, IDC_STATUSBAR_GRID);
	//createCheckbox(ID_SHORTCUT_CUSTOMIZATION_SNAPMODE, IDC_STATUSBAR_SNAPMODE);

	return 0;
}
int Fakemenu::wm_destroy()
{
	log("Log::Fakemenu::wm_destroy()");
	return 0;
}
int Fakemenu::wm_keydown(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_keydown()");

	return 0;
}
int Fakemenu::wm_killfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	log("Log::Fakemenu::wm_killfocus()");

	return 0;
}
int Fakemenu::wm_lbuttondblclk(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_lbuttondblclk()");

	return 0;
}
int Fakemenu::wm_lbuttondown(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_lbuttondown()");

	m_ptMouseDown = MAKEPOINT(lParam);

	RECT rc;
	::GetWindowRect(m_hWnd, &rc);
	if (::PtInRect(&rc, m_ptMouseDown)) {

	}

	//::SendMessage(m_hCheckbox[0], BM_CLICK, 0, 0);

	return 0;
}
int Fakemenu::wm_lbuttonup(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_lbuttonup()");

	return 0;
}
int Fakemenu::wm_mouseleave()
{
	log("Log::Fakemenu::wm_mouseleave()");
	return 0;
}
int Fakemenu::wm_mousemove(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_mousemove()");

	return 0;
}
int Fakemenu::wm_mousewheel(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_mousewheel()");

	return 0;
}
int Fakemenu::wm_notify(WPARAM wParam, LPARAM lParam)
{ 
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_notify()");

	return 0;
}
int Fakemenu::wm_paint()
{
	log("Log::Fakemenu::wm_paint()");

	//PAINTSTRUCT ps;
	//HDC hDC = BeginPaint(m_hWnd, &ps);
	//::FillRect(hDC, &ps.rcPaint, (HBRUSH)(COLOR_MENU + 1));
	//int nPosition = s_nSpacing-2;
	//for (int i = 0; i <= 4; i++) {
	//	MoveToEx(hDC, 0, nPosition, NULL);
	//	LineTo(hDC, m_cx, nPosition);
	//	nPosition += s_nSpacing;
	//}
	//::EndPaint(m_hWnd, &ps);
	//::ReleaseDC(m_hWnd, hDC);
	return 0;
}
int Fakemenu::wm_setcursor(WPARAM wParam, LPARAM lParam)//Child window; set cursor and return FALSE for default processing.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Fakemenu::wm_setcursor()");

	return 0;
}
int Fakemenu::wm_setfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	log("Log::Fakemenu::wm_setfocus()");

	return 0;
}
int Fakemenu::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	log("Log::Fakemenu::wm_size()");

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	return 0;
}

HWND Fakemenu::createCheckbox(UINT textid, UINT controlid)
{
	HRESULT hResult = S_OK;

	char szText[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, textid, szText, sizeof(szText) / sizeof(char));

	HWND hWnd = ::CreateWindow(
		WC_BUTTON,
		szText,
		//WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		5,
		m_nPosition,
		m_cx - 10,
		27,
		m_hWnd,
		(HMENU)(UINT_PTR)controlid,
		s_hInstance,
		(LPVOID)0
	);

	if (hWnd == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
		setFont(hWnd);
	}

	m_nPosition += s_nSpacing;

	return hWnd;
}
void Fakemenu::createSeperator()
{

}
