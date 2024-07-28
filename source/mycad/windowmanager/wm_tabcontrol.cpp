#include "wm_tabcontrol.h"
#include "wm_tooltip.h"

#include "fault.h"
#include "log.h"

using namespace mycad;

Tabcontrol::Tabcontrol(DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hwndParent,
	UINT uId,
	HINSTANCE hInstance)
	: Window(hInstance),
	m_uIdSubclass(uId)
{
	log("Log::Tabcontrol::Tabcontrol()");

	HRESULT hResult = S_OK;

	m_hWnd = ::CreateWindow(
		WC_TABCONTROL,
		NULL,
		dwStyle,
		x,
		y,
		nWidth,
		nHeight,
		hwndParent,
		(HMENU)(UINT_PTR)uId,
		hInstance,
		(LPVOID)0
	);

	if (m_hWnd == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		setFont(m_hWnd);

#pragma warning( pop )
	}
}
Tabcontrol::Tabcontrol(DWORD dwExStyle,
	DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hwndParent,
	UINT uId,
	HINSTANCE hInstance)
	: Window(hInstance),
	m_uIdSubclass(uId)
{
	log("Log::Tabcontrol::Tabcontrol()");

	HRESULT hResult = S_OK;

	m_hWnd = ::CreateWindowEx(
		dwExStyle,
		WC_TABCONTROL,
		NULL,
		dwStyle,
		x,
		y,
		nWidth,
		nHeight,
		hwndParent,
		(HMENU)(UINT_PTR)uId,
		hInstance,
		(LPVOID)0
	);

	if (m_hWnd == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		::SetWindowSubclass(m_hWnd, s_lpfnTabcontrolSubclassProc, m_uIdSubclass, (DWORD_PTR)this);

		setFont(m_hWnd);

#pragma warning( pop )
	}
}
Tabcontrol::~Tabcontrol()
{
	log("Log::Tabcontrol::~Tabcontrol()");
	::RemoveWindowSubclass(m_hWnd, s_lpfnTabcontrolSubclassProc, m_uIdSubclass);
	::DestroyWindow(m_hWnd);
}

int Tabcontrol::wm_windowposchanged(LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);


	return 0;
}
int Tabcontrol::wm_windowposchanging(LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);


	return 0;
}

LRESULT CALLBACK Tabcontrol::s_lpfnTabcontrolSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);

	Tabcontrol* window = reinterpret_cast<Tabcontrol*>(dwRefData);

	switch (uMsg) {
		case WM_WINDOWPOSCHANGED: {
			window->wm_windowposchanged(lParam);
			break;
		}
		case WM_WINDOWPOSCHANGING: {
			window->wm_windowposchanging(lParam);
			break;
		}
		default: {
			break;
		}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
