#include "SYS_systemwin32.h"

#include "EDL_dialog_drawingunits.h"
#include "EDL_resource.h"

#include "resources.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>
#include <commctrl.h>
#include <intsafe.h>//Required for IntPtrToInt
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.

using namespace mycad;

dlg_DrawingUnits::dlg_DrawingUnits(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CommandDialog(command, commandwindows, mode),
	m_bClockwise(false),
	m_hcboLengthType(0),
	m_hcboLengthPrecision(0),
	m_hcboAngleType(0),
	m_hcboAnglePrecision(0),
	m_hbtnClockwise(0),
	m_hcboInsertionscale(0),
	m_htxtSampleOutputLength(0),
	m_htxtSampleOutputAngle(0),
	m_hcboLightingUnits(0),
	m_hOK(0),
	m_hCancel(0),
	m_hDirection(0),
	m_hHelp(0)
{

}
dlg_DrawingUnits::dlg_DrawingUnits(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CommandDialog(command, commandwindows, text, mode),
	m_bClockwise(false),
	m_hcboLengthType(0),
	m_hcboLengthPrecision(0),
	m_hcboAngleType(0),
	m_hcboAnglePrecision(0),
	m_hbtnClockwise(0),
	m_hcboInsertionscale(0),
	m_htxtSampleOutputLength(0),
	m_htxtSampleOutputAngle(0),
	m_hcboLightingUnits(0),
	m_hOK(0),
	m_hCancel(0),
	m_hDirection(0),
	m_hHelp(0)
{

}
dlg_DrawingUnits::~dlg_DrawingUnits()
{

}

int dlg_DrawingUnits::wm_command(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	BOOL nResult = FALSE;
	switch (LOWORD(wParam)) {
		case IDC_FORMAT_DRAWINGUNITS_ANGLE_CHECKBOX_CLOCKWISE: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
			}
			break;
		}
		case IDOK: {
			RECT rcWindow = { 0,0,0,0 };
			::GetWindowRect(m_hWnd, &rcWindow);
			setWindowState(WINDOWSTATE_POSITION, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DrawingUnits");

			nResult = ::EndDialog(m_hWnd, wParam);

			break;
		}
		case IDCANCEL: {
			nResult = ::EndDialog(m_hWnd, wParam);
			break;
		}
		case IDC_DIRECTION: {

			break;
		}
		case IDHELP: {

			break;
		}
	}
	return (int)nResult;
}
int dlg_DrawingUnits::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) {
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
int dlg_DrawingUnits::wm_destroy()
{
	return FALSE;
}
int dlg_DrawingUnits::wm_erasebkgnd(WPARAM wParam) {
	UNREFERENCED_PARAMETER(wParam);
	return FALSE;
}
int dlg_DrawingUnits::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	setIcon(m_hWnd, MAKEINTRESOURCE(IDI_MYCAD));

	m_hcboLengthType = ::GetDlgItem(m_hWnd, IDC_FORMAT_DRAWINGUNITS_COMBOBOX_LENGTH_TYPE);
	m_hcboLengthPrecision = ::GetDlgItem(m_hWnd, IDC_FORMAT_DRAWINGUNITS_COMBOBOX_LENGTH_PRECISION);
	m_hcboAngleType = ::GetDlgItem(m_hWnd, IDC_FORMAT_DRAWINGUNITS_COMBOBOX_ANGLE_TYPE);
	m_hcboAnglePrecision = ::GetDlgItem(m_hWnd, IDC_FORMAT_DRAWINGUNITS_COMBOBOX_ANGLE_PRECISION);
	m_hbtnClockwise = ::GetDlgItem(m_hWnd, IDC_FORMAT_DRAWINGUNITS_ANGLE_CHECKBOX_CLOCKWISE);

	if (m_bClockwise) {
		::SendMessage(m_hbtnClockwise, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	}
	else {
		::SendMessage(m_hbtnClockwise, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	}

	m_hcboInsertionscale = ::GetDlgItem(m_hWnd, IDC_FORMAT_DRAWINGUNITS_COMBOBOX_INSERTIONSCALE);

	m_htxtSampleOutputLength = ::GetDlgItem(m_hWnd, IDC_FORMAT_DRAWINGUNITS_SAMPLEOUTPUT_LTEXT_LENGTH);
	m_htxtSampleOutputAngle = ::GetDlgItem(m_hWnd, IDC_FORMAT_DRAWINGUNITS_SAMPLEOUTPUT_LTEXT_ANGLE);

	m_hcboLightingUnits = ::GetDlgItem(m_hWnd, IDC_FORMAT_DRAWINGUNITS_COMBOBOX_LIGHTING);

	m_hOK = ::GetDlgItem(m_hWnd, IDOK);
	m_hCancel = ::GetDlgItem(m_hWnd, IDCANCEL);
	m_hDirection = ::GetDlgItem(m_hWnd, IDC_DIRECTION);
	m_hHelp = ::GetDlgItem(m_hWnd, IDHELP);

	RECT rcWindow = { 0,0,0,0 };
	getWindowState(WINDOWSTATE_POSITION, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DrawingUnits");

	::SetWindowPos(m_hWnd,
		HWND_TOP,
		rcWindow.left,
		rcWindow.top,
		364,
		429,
		SWP_SHOWWINDOW | SWP_NOSIZE
	);

	return FALSE;
}
int dlg_DrawingUnits::wm_notify(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	BOOL nResult = FALSE;

//	LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);
//	if ((nmhdr->code == TRBN_THUMBPOSCHANGING) && (nmhdr->hwndFrom == m_htrbSize)) {
//		log("Log::dlg_DrawingUnits::wm_notify() TRBN_THUMBPOSCHANGING ");
//		NMTRBTHUMBPOSCHANGING* nmtrb = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(lParam);
//		switch (nmtrb->nReason) {
//			case TB_THUMBTRACK:
//			case TB_PAGEUP:
//			case TB_PAGEDOWN:
//			case TB_THUMBPOSITION:
//			case TB_TOP:
//			case TB_BOTTOM:
//			case TB_ENDTRACK: {
//				
//				break;
//			}
//		}
//	}
//
//#define lpnm (LPNMHDR(lParam))

	return (int)nResult;
}
int dlg_DrawingUnits::wm_paint() {
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);

	::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);

	return TRUE;
}
int dlg_DrawingUnits::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::dlg_DrawingUnits::wm_syscommand() SC_CLOSE");
			break;
		}
		default: {
			break;
		}
	}

	return FALSE;
}