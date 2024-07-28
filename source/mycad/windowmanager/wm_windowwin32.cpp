#include "wm_windowwin32.h"

#include "SYS_systemwin32.h"

#include "resources.h"

#include "reg_util.h"

#include "fault.h"
#include "log.h"

#include <intsafe.h>//Required for LongPtrToInt
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.
#include <windowsx.h>

using namespace mycad;

WindowWin32::WindowWin32()
	: Window(),
	m_szClassName("\0"),
	//m_szWindowName("\0"),
	m_x(0),
	m_y(0),
	m_cx(0),
	m_cy(0)
{
	log("Log::WindowWin32::WindowWin32()");
}
WindowWin32::WindowWin32(HINSTANCE hInstance)
	: Window(hInstance),
	m_szClassName("\0"),
	//m_szWindowName("\0"),
	m_x(0),
	m_y(0),
	m_cx(0),
	m_cy(0)
{
	log("Log::WindowWin32::WindowWin32()");
}
WindowWin32::~WindowWin32()
{
	log("Log::WindowWin32::~WindowWin32()");
	if (m_hWnd) {
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

DWORD WindowWin32::getExStyle() { return (DWORD)GetWindowLongPtr(m_hWnd, GWL_EXSTYLE); }

void WindowWin32::setClassName(const char* lpszClassName) { ::strcpy_s(m_szClassName, lpszClassName); }
char* WindowWin32::getClassName() { return m_szClassName; }

//void WindowWin32::setWindowName(const char* lpszWindowName) { ::strcpy_s(m_szWindowName, lpszWindowName); }
//char* WindowWin32::getWindowName() { return m_szWindowName; }

LONG WindowWin32::getStyle() { return (LONG)GetWindowLongPtr(m_hWnd, GWL_STYLE); }

void WindowWin32::setX(int x) { m_x = x; }
int WindowWin32::getX() const { return m_x; }

void WindowWin32::setY(int y) { m_y = y; }
int WindowWin32::getY() const { return m_y; }

void WindowWin32::setCX(int cx) { m_cx = cx; }
int WindowWin32::getCX() const { return m_cx; }

void WindowWin32::setCY(int cy) { m_cy = cy; }
int WindowWin32::getCY() const { return m_cy; }

HWND WindowWin32::getParentHWND() const { return (HWND)GetWindowLongPtr(m_hWnd, GWLP_HWNDPARENT); }

int WindowWin32::getID()
{
	HRESULT hResult = S_OK;
	INT nResult = 0;

	LONG_PTR npResult = ::GetWindowLongPtr(m_hWnd, GWLP_ID);
	hResult = ::LongPtrToInt(npResult, &nResult);

	if (hResult == S_OK) {
		return (int)npResult;
	}

	return 0;
}

void WindowWin32::setIcon(HWND hWnd, LPCSTR lpszResName)
{
	HRESULT hResult = S_OK;

	HICON hIcon = (HICON)::LoadImage(
		s_hInstance,
		lpszResName,
		IMAGE_ICON,
		//::GetSystemMetrics(SM_CXSMICON),
		//::GetSystemMetrics(SM_CYSMICON),
		0,
		0,
		LR_SHARED | LR_DEFAULTCOLOR | LR_DEFAULTSIZE
	);

	if (hIcon == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
//#pragma warning( push )
//#pragma warning( disable : 6387)

		::SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);

//#pragma warning( pop )
	}
}

int WindowWin32::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	//log("Log::Window::wm_size(WPARAM wParam, LPARAM lParam)");

	if ((int)wParam == SIZE_MINIMIZED) return 0;

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	wm_size();

	return 0;
}

void WindowWin32::wm_size() {}

LRESULT WindowWin32::SendCommandText(HWND hWnd, UINT_PTR idFrom, const char* text)
{
	if (hWnd == NULL) {
		return FALSE;
	}

	if (IsWindow(hWnd) == FALSE) {
		return FALSE;
	}

	LRESULT lResult = ::SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(WM_SETTEXT, idFrom), (LPARAM)text);

	return lResult;
}
LRESULT WindowWin32::SendCommandParam(HWND hWnd, UINT_PTR idFrom, int code, LPARAM lParam)												
{
	if (hWnd == NULL) {
		return FALSE;
	}

	if (IsWindow(hWnd) == FALSE) {
		return FALSE;
	}

	LRESULT lResult = ::SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(code, idFrom), lParam);

	return lResult;
}

void WindowWin32::setWindowState(int var, LPARAM lparam, const char* path, const char* key)
{
	char chDir[MAX_LOADSTRING]{ 0 };
	char chPath[MAX_LOADSTRING]{ 0 };
	char chCompanyName[MAX_LOADSTRING]{ 0 };
	char chApplicationName[MAX_LOADSTRING]{ 0 };
	char chRelease[MAX_LOADSTRING]{ 0 };

	HINSTANCE hModule = ::LoadLibrary("mycaddata.dll");
	if (hModule != NULL) {
		::LoadString(hModule, ID_COMPANY_NAME, chCompanyName, sizeof(chCompanyName) / sizeof(char));
		::LoadString(hModule, ID_APPLICATION_NAME, chApplicationName, sizeof(chApplicationName) / sizeof(char));
		::LoadString(hModule, ID_APPLICATION_RELEASE, chRelease, sizeof(chRelease) / sizeof(char));
		::FreeLibrary(hModule);
	}

	_tcscpy_s(chDir, STRING_SIZE(chDir), "Software\\");
	_tcscat_s(chDir, STRING_SIZE(chDir), chCompanyName);
	_tcscat_s(chDir, STRING_SIZE(chDir), "\\");
	_tcscat_s(chDir, STRING_SIZE(chDir), chApplicationName);
	_tcscat_s(chDir, STRING_SIZE(chDir), "\\");
	_tcscat_s(chDir, STRING_SIZE(chDir), chRelease);

	::strcpy_s(chPath, chDir);
	_tcscat_s(chPath, STRING_SIZE(chPath), path);

	switch (var) {
		case WINDOWSTATE_RECT: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_RECT");
			RECT* rcRect = (RECT*)lparam;
			rcRect->right -= rcRect->left;
			rcRect->bottom -= rcRect->top;
			SetUserValue(chPath, key, REG_BINARY, rcRect, sizeof(RECT));
			break;
		}
		case WINDOWSTATE_POSITION: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_POSITION");
			RECT* position = (RECT*)lparam;
			SetUserValue(chPath, "X", REG_DWORD, &position->left, sizeof(LONG));
			SetUserValue(chPath, "Y", REG_DWORD, &position->top, sizeof(LONG));
			break;
		}
		case WINDOWSTATE_SIZE: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_SIZE");
			RECT* position = (RECT*)lparam;
			position->right -= position->left;
			position->bottom -= position->top;
			SetUserValue(chPath, "Width", REG_DWORD, &position->right, sizeof(LONG));
			SetUserValue(chPath, "Height", REG_DWORD, &position->bottom, sizeof(LONG));
			break;
		}
		case WINDOWSTATE_ACTIVETAB: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_ACTIVETAB");
			int* nActiveTab = (int*)lparam;
			SetUserValue(chPath, "ActiveTab", REG_DWORD, nActiveTab, sizeof(LONG));
			break;
		}
		case WINDOWSTATE_SCROLLBARS: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_SCROLLBARS");
			BOOL nScrollbars = (BOOL)lparam;
			SetUserValue(chPath, "Scrollbars", REG_DWORD, &nScrollbars, sizeof(BOOL));
			break;
		}
		case WINDOWSTATE_SHOW: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_SCROLLBARS");
			BOOL nShow = (BOOL)lparam;
			SetUserValue(chPath, key, REG_DWORD, &nShow, sizeof(BOOL));
			break;
		}
		default: {
			log("Log::WindowWin32::setWindowState() default");
			break;
		}
	}
}
void WindowWin32::getWindowState(int var, void* pDest, const char* path, const char* key)
{
	char chDir[MAX_LOADSTRING]{ 0 };
	char chPath[MAX_LOADSTRING]{ 0 };
	char chCompanyName[MAX_LOADSTRING]{ 0 };
	char chApplicationName[MAX_LOADSTRING]{ 0 };
	char chRelease[MAX_LOADSTRING]{ 0 };

	HINSTANCE hModule = ::LoadLibrary("mycaddata.dll");
	if (hModule != NULL) {
		::LoadString(hModule, ID_COMPANY_NAME, chCompanyName, sizeof(chCompanyName) / sizeof(char));
		::LoadString(hModule, ID_APPLICATION_NAME, chApplicationName, sizeof(chApplicationName) / sizeof(char));
		::LoadString(hModule, ID_APPLICATION_RELEASE, chRelease, sizeof(chRelease) / sizeof(char));
		::FreeLibrary(hModule);
	}

	_tcscpy_s(chDir, STRING_SIZE(chDir), "Software\\");
	_tcscat_s(chDir, STRING_SIZE(chDir), chCompanyName);
	_tcscat_s(chDir, STRING_SIZE(chDir), "\\");
	_tcscat_s(chDir, STRING_SIZE(chDir), chApplicationName);
	_tcscat_s(chDir, STRING_SIZE(chDir), "\\");
	_tcscat_s(chDir, STRING_SIZE(chDir), chRelease);

	::strcpy_s(chPath, chDir);
	_tcscat_s(chPath, STRING_SIZE(chPath), path);

	switch (var) {
		case WINDOWSTATE_RECT: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_RECT");
			RECT* rcRect = (RECT*)pDest;
			GetUserValue(chPath, key, REG_BINARY, rcRect, sizeof(RECT));
			pDest = rcRect;
			break;
		}
		case WINDOWSTATE_POSITION: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_POSITION");
			RECT* rcPosition = (RECT*)pDest;
			GetUserValue(chPath, "X", REG_DWORD, &rcPosition->left, sizeof(long));
			GetUserValue(chPath, "Y", REG_DWORD, &rcPosition->top, sizeof(long));
			pDest = rcPosition;
			break;
		}
		case WINDOWSTATE_SIZE: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_SIZE");
			RECT* rcSize = (RECT*)pDest;
			GetUserValue(chPath, "Width", REG_DWORD, &rcSize->right, sizeof(long));
			GetUserValue(chPath, "Height", REG_DWORD, &rcSize->bottom, sizeof(long));
			pDest = rcSize;
			break;
		}
		case WINDOWSTATE_ACTIVETAB: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_ACTIVETAB");
			int* nActiveTab = (int*)pDest;
			GetUserValue(chPath, "ActiveTab", REG_DWORD, nActiveTab, sizeof(long));
			pDest = nActiveTab;
			break;
		}
		case WINDOWSTATE_SCROLLBARS: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_SCROLLBARS");
			BOOL* nScrollbars = (BOOL*)pDest;
			GetUserValue(chPath, "Scrollbars", REG_DWORD, nScrollbars, sizeof(BOOL));
			pDest = nScrollbars;
			break;
		}
		case WINDOWSTATE_SHOW: {
			log("Log::WindowWin32::setWindowState() WINDOWSTATE_SCROLLBARS");
			BOOL* nScrollbars = (BOOL*)pDest;
			GetUserValue(chPath, key, REG_DWORD, nScrollbars, sizeof(BOOL));
			pDest = nScrollbars;
			break;
		}
		default: {
			log("Log::WindowWin32::setWindowState() default");
			break;
		}
	}
}
