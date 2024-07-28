#include "wm_tooltip.h"

#include "SYS_systemwin32.h"

#include "fault.h"
#include "log.h"

using namespace mycad;

Tooltip::Tooltip()
	: Window(),
	m_ToolInfo({ 0 }),
	m_szTooltipTitle{ 0 },
	m_szTooltipText{ 0 }
{}
Tooltip::Tooltip(DWORD dwStyle, HWND parent, HINSTANCE hInstance, UINT uFlags, UINT_PTR uId, UINT uMaxWidth)
	: Window(hInstance),
	m_ToolInfo({ 0 }),
	m_szTooltipTitle{ 0 },
	m_szTooltipText{ 0 }
{
	//log("Log::Tooltip::Tooltip()");

	HRESULT hResult = S_OK;
	BOOL nResult = FALSE;

	m_hWnd = ::CreateWindowEx(
		WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		dwStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		parent,
		NULL,
		hInstance,
		(LPVOID)0
	);

	if (m_hWnd == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
		setFont(m_hWnd);

		if (uMaxWidth > 0) {
			::SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, (LPARAM)uMaxWidth);
		}

		m_ToolInfo.cbSize = sizeof(TOOLINFO);
		m_ToolInfo.hinst = s_hInstance;
		m_ToolInfo.hwnd = parent;
		m_ToolInfo.uFlags = uFlags;
		m_ToolInfo.uId = uId;

		nResult = (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&m_ToolInfo);
		if (nResult == FALSE) {
			ErrorHandler();
		}
	}
}
Tooltip::Tooltip(DWORD dwStyle, HWND parent, HINSTANCE hInstance, UINT uFlags, UINT_PTR uId, const char* lpszText, UINT uMaxWidth)
	: Window(hInstance),
	m_ToolInfo({ 0 }),
	m_szTooltipTitle{ 0 },
	m_szTooltipText{ 0 }
{
	//log("Log::Tooltip::Tooltip()");

	HRESULT hResult = S_OK;
	BOOL nResult = FALSE;

	m_hWnd = ::CreateWindowEx(
		WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		dwStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		parent,
		NULL,
		hInstance,
		(LPVOID)0
	);

	if (m_hWnd == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
		setFont(m_hWnd);

		::strcpy_s(m_szTooltipText, lpszText);

		if (uMaxWidth > 0) {
			::SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, (LPARAM)uMaxWidth);
		}

		m_ToolInfo.cbSize = sizeof(TOOLINFO);
		m_ToolInfo.hinst = s_hInstance;
		m_ToolInfo.hwnd = parent;
		m_ToolInfo.lpszText = (LPSTR)&m_szTooltipText;
		m_ToolInfo.uFlags = uFlags;
		m_ToolInfo.uId = uId;

		nResult = (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&m_ToolInfo);
		if (nResult == FALSE) {
			ErrorHandler();
		}
	}
}
Tooltip::Tooltip(DWORD dwStyle, HWND parent, HINSTANCE hInstance, UINT uFlags, UINT_PTR uId, const char* lpszTitle, const char* lpszText, UINT uMaxWidth)
	: Window(hInstance),
	m_ToolInfo({ 0 }),
	m_szTooltipTitle{ 0 },
	m_szTooltipText{ 0 }
{
	//log("Log::Tooltip::Tooltip()");

	HRESULT hResult = S_OK;
	BOOL nResult = FALSE;

	m_hWnd = ::CreateWindowEx(
		WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		dwStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		parent,
		NULL,
		hInstance,
		(LPVOID)0
	);

	if (m_hWnd == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
		setFont(m_hWnd);

		::strcpy_s(m_szTooltipTitle, lpszTitle);
		::strcpy_s(m_szTooltipText, lpszText);

		if (uMaxWidth > 0) {
			SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, (LPARAM)uMaxWidth);
		}

		nResult = (BOOL)::SendMessage(m_hWnd, TTM_SETTITLE, (WPARAM)TTI_NONE, (LPARAM)&m_szTooltipTitle);
		if (nResult == FALSE) {
			ErrorHandler();
		}

		m_ToolInfo.cbSize = sizeof(TOOLINFO);
		m_ToolInfo.hinst = s_hInstance;
		m_ToolInfo.hwnd = parent;
		m_ToolInfo.lpszText = (LPSTR)&m_szTooltipText;
		m_ToolInfo.uFlags = uFlags;
		m_ToolInfo.uId = uId;

		nResult = (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&m_ToolInfo);
		if (nResult == FALSE) {
			ErrorHandler();
		}
	}
}
Tooltip::~Tooltip()
{
	//log("Log::Tooltip::~Tooltip()");

	::DestroyWindow(m_hWnd);
}

BOOL Tooltip::setPosition(LPRECT lprc)
{
	BOOL nResult = 0;

	::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM)&m_ToolInfo);

	m_ToolInfo.rect = *lprc;
	nResult = (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&m_ToolInfo);

	return nResult;
}
BOOL Tooltip::setPosition(long left, long top, long right, long bottom)
{
	BOOL nResult = 0;

	m_ToolInfo.rect = { left,top,right,bottom };
	::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM)&m_ToolInfo);
	nResult = (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&m_ToolInfo);

	return nResult;
}

BOOL Tooltip::setText(const char* lpszText)//Returns TRUE if successful, FALSE if not.
{
	::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM)&m_ToolInfo);

	::strcpy_s(m_szTooltipText, lpszText);
	m_ToolInfo.lpszText = (LPSTR)m_szTooltipText;
	return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&m_ToolInfo);
}

BOOL Tooltip::setTitle(const char* lpszTitle, INT nIcon)//Returns TRUE if successful, FALSE if not.
{
	::strcpy_s(m_szTooltipTitle, lpszTitle);
	return (BOOL)::SendMessage(m_hWnd, TTM_SETTITLE, (WPARAM)nIcon, (LPARAM)m_szTooltipTitle);
}

INT Tooltip::setMaxTipWidth(int nWidth) { return (INT)SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, (LPARAM)nWidth); }//Returns the previous maximum tooltip width.

void Tooltip::trackActivate(BOOL bState) { SendMessage(m_hWnd, TTM_TRACKACTIVATE, (WPARAM)bState, (LPARAM)&m_ToolInfo); }
void Tooltip::trackPosition(POINT ptPosition) { SendMessage(m_hWnd, TTM_TRACKPOSITION, 0, MAKELPARAM(ptPosition.x, ptPosition.y)); }

LPTOOLINFO Tooltip::getToolInfo() { return &m_ToolInfo; }
