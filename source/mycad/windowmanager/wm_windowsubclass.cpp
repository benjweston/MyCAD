#include "wm_windowsubclass.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }); 

using namespace mycad;

WindowSubclass::WindowSubclass(HWND hWnd, UINT_PTR uIdSubclass)
	: Window(),
	m_uIdSubclass(uIdSubclass)
{
	m_hWnd = hWnd;
}
WindowSubclass::WindowSubclass(HWND hWnd, HINSTANCE hInstance, UINT_PTR uIdSubclass)
	: Window(hInstance),
	m_uIdSubclass(uIdSubclass)
{
	m_hWnd = hWnd;
}
WindowSubclass::~WindowSubclass()
{

}
