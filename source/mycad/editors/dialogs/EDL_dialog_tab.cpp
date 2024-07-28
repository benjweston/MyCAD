#include "EDL_dialog_tab.h"

#include "reg_util.h"
#include "xml_util.h"

#include "fault.h"
#include "log.h"

#include <intsafe.h>//Required for IntPtrToInt

using namespace mycad;
using namespace tinyxml2;

dlg_Tab::dlg_Tab()
	: m_hTabControl(0)
{
	m_cx = 704;
	m_cy = 544;
}
dlg_Tab::dlg_Tab(HWND htabcontrol)
	: m_hTabControl(htabcontrol)
{
	m_cx = 704;
	m_cy = 544;
}
dlg_Tab::~dlg_Tab()
{

}

void dlg_Tab::setRegistryData() {}
void dlg_Tab::getRegistryData() {}

int dlg_Tab::setXMLData() { return static_cast<int>(XML_SUCCESS); }
int dlg_Tab::getXMLData() { return static_cast<int>(XML_SUCCESS); }

int dlg_Tab::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HRESULT hResult = S_OK;
	INT nResult = 0;

	INT_PTR npBrush = (INT_PTR)::CreateSolidBrush(RGB(255, 255, 255));

	hResult = ::IntPtrToInt(npBrush, &nResult);
	if (hResult == S_OK) {
		return nResult;
	}

	return FALSE;
}
int dlg_Tab::wm_erasebkgnd(WPARAM wParam) {
	UNREFERENCED_PARAMETER(wParam);
	return FALSE;
}
int dlg_Tab::wm_paint() {
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);

	::FillRect(hDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);

	return TRUE;
}
