#include "SYS_systemwin32.h"

#include "EDL_dialog_ucsicon.h"
#include "EDL_resource.h"

#include "resources.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>
#include <commctrl.h>
#include <intsafe.h>//Required for IntPtrToInt
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.

using namespace mycad;

dlg_UCSIcon::dlg_UCSIcon(COMMANDINFO* command, DynamicInputWindow* commandwindows[], int style, unsigned int linewidth, unsigned int size, Colour3<float> modelspaceiconcolour, Colour3<float> layouttabiconcolour, CommandMode mode)
	: CommandDialog(command, commandwindows, mode),
	m_nStyle(style),
	m_nLinewidth(linewidth),
	m_nSize(size),
	m_nRange({ 5,95 }),
	m_ModelspaceIconColour(modelspaceiconcolour),
	m_LayoutTabIconColour(layouttabiconcolour),
	m_hbtnTintForXYZ(0),
	m_bTintForXYZ(true),
	m_hrbtIconStyle2D(0),
	m_hrbtIconStyle3D(0),
	m_hcboLinewidth(0),
	m_htxtSize(0),
	m_htrbUCSIconSize(0),
	m_hcboModelspaceIconColour(0),
	m_hcboLayoutTabIconColour(0),
	m_hOK(0),
	m_hCancel(0),
	m_hHelp(0)
{

}
dlg_UCSIcon::dlg_UCSIcon(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, int style, unsigned int linewidth, unsigned int size, Colour3<float> modelspaceiconcolour, Colour3<float> layouttabiconcolour, CommandMode mode)
	: CommandDialog(command, commandwindows, text, mode),
	m_nStyle(style),
	m_nLinewidth(linewidth),
	m_nSize(size),
	m_nRange({ 5,95 }),
	m_ModelspaceIconColour(modelspaceiconcolour),
	m_LayoutTabIconColour(layouttabiconcolour),
	m_hbtnTintForXYZ(0),
	m_bTintForXYZ(true),
	m_hrbtIconStyle2D(0),
	m_hrbtIconStyle3D(0),
	m_hcboLinewidth(0),
	m_htxtSize(0),
	m_htrbUCSIconSize(0),
	m_hcboModelspaceIconColour(0),
	m_hcboLayoutTabIconColour(0),
	m_hOK(0),
	m_hCancel(0),
	m_hHelp(0)
{

}
dlg_UCSIcon::~dlg_UCSIcon()
{
	log("Log::dlg_UCSIcon::~dlg_UCSIcon()");
}

int dlg_UCSIcon::getStyle() { return m_nStyle; }
unsigned int dlg_UCSIcon::getLinewidth() { return m_nLinewidth; }
unsigned int dlg_UCSIcon::getSize() { return m_nSize; }
Colour3<float> dlg_UCSIcon::getModelspaceIconColour() { return m_ModelspaceIconColour; }
Colour3<float> dlg_UCSIcon::getLayoutTabIconColour() { return m_LayoutTabIconColour; }

void dlg_UCSIcon::selectColour(HWND hcombobox) {
	CHOOSECOLOR cc;
	static COLORREF acrCustClr[16];
	HBRUSH hbrush;
	static DWORD rgbCurrent;

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE) {
		hbrush = ::CreateSolidBrush(cc.rgbResult);
		rgbCurrent = cc.rgbResult;

		Colour3<unsigned int> c3fColour = { 0,0,0 };

		c3fColour.r = GetRValue(cc.rgbResult);
		c3fColour.g = GetGValue(cc.rgbResult);
		c3fColour.b = GetBValue(cc.rgbResult);

		for (unsigned int i = 0; i < 256; i++) {
			if ((dxfColors[i][0] == c3fColour.r) && (dxfColors[i][1] == c3fColour.g) && (dxfColors[i][2] == c3fColour.b)) {
				char szColour[8];
				::sprintf_s(szColour, "%d", i);

				char szColourName[16] = "Colour ";
				::strcat_s(szColourName, 16, szColour);

				::SendMessage(hcombobox, CB_ADDSTRING, 0, (LPARAM)szColourName);
				break;
			}
		}
	}
}

int dlg_UCSIcon::wm_command(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	BOOL nResult = FALSE;
	switch (LOWORD(wParam)) {
		case IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_RADIOBUTTON_2D: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					m_nStyle = 0;
					nResult = TRUE;
					break;
				}
			}
			break;
		}
		case IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_RADIOBUTTON_3D: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					m_nStyle = 1;
					nResult = TRUE;
					break;
				}
			}
			break;
		}
		case IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_COMBOBOX_LINEWIDTH: {
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE: {
					int nItemIndex = (int)::SendMessage(m_hcboLinewidth, CB_GETCURSEL, 0, 0);
					m_nLinewidth = nItemIndex + 1;
					nResult = TRUE;
					break;
				}
			}
			break;
		}
		case IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_TRACKBAR_SIZE: {
			switch (HIWORD(wParam)) {
				case WM_HSCROLL: {

					nResult = TRUE;
					break;
				}
			}
			break;
		}
		case IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_COMBOBOX_MODELSPACEICONCOLOUR: {
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE: {
					int nItemIndex = (int)::SendMessage(m_hcboModelspaceIconColour, CB_GETCURSEL, 0, 0);
					if (nItemIndex == 7) {
						selectColour(m_hcboModelspaceIconColour);
					}
					nResult = TRUE;
					break;
				}
			}
			break;
		}
		case IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_COMBOBOX_LAYOUTTABICONCOLOUR: {
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE: {
					int nItemIndex = (int)::SendMessage(m_hcboLayoutTabIconColour, CB_GETCURSEL, 0, 0);
					if (nItemIndex == 7) {
						selectColour(m_hcboLayoutTabIconColour);
					}
					nResult = TRUE;
					break;
				}
			}
			break;
		}
		case IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_CHECKBOX_SINGLECOLOUR: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
			}
			break;
		}
		case IDOK: {
			long nCurSel = (long)::SendMessage(m_htrbUCSIconSize, TBM_GETPOS, 0, 0);
			if (nCurSel < m_nRange.cx ) {
				m_nSize = m_nRange.cx;
			}
			else if (nCurSel > m_nRange.cy) {
				m_nSize = m_nRange.cy;
			}
			else {
				m_nSize = nCurSel;
			}

			LRESULT lResult = ::SendMessage(m_hbtnTintForXYZ, BM_GETCHECK, 0, 0);
			if (lResult == BST_CHECKED) {
				m_bTintForXYZ = false;
			}
			else if (lResult == BST_UNCHECKED) {
				m_bTintForXYZ = true;
			}

			RECT rcWindow = { 0,0,0,0 };
			GetWindowRect(m_hWnd, &rcWindow);
			setWindowState(WINDOWSTATE_POSITION, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\UCSIcon");

			::EndDialog(m_hWnd, wParam);

			nResult = TRUE;
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
int dlg_UCSIcon::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) {
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
int dlg_UCSIcon::wm_destroy()
{
	DestroyWindow(m_htrbUCSIconSize);
	return FALSE;
}
int dlg_UCSIcon::wm_erasebkgnd(WPARAM wParam) {
	UNREFERENCED_PARAMETER(wParam);
	return FALSE;
}
int dlg_UCSIcon::wm_hscroll(WPARAM wParam, LPARAM lParam)
{
	if ((lParam != 0) && (reinterpret_cast<HWND>(lParam) == m_htrbUCSIconSize)) {
		switch (LOWORD(wParam)) {
			case SB_ENDSCROLL:
			case SB_LEFT:
			case SB_RIGHT:
			case SB_LINELEFT:
			case SB_LINERIGHT:
			case SB_PAGELEFT:
			case SB_PAGERIGHT:
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK: {
				
				break;
			}
		}
	}
	return TRUE;
}
int dlg_UCSIcon::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	setIcon(m_hWnd, MAKEINTRESOURCE(IDI_MYCAD));

	m_hrbtIconStyle2D = ::GetDlgItem(m_hWnd, IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_RADIOBUTTON_2D);
	m_hrbtIconStyle3D = ::GetDlgItem(m_hWnd, IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_RADIOBUTTON_3D);

	if (m_nStyle == 0) {
		::SendMessage(m_hrbtIconStyle2D, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		::SendMessage(m_hrbtIconStyle3D, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
	}
	else if (m_nStyle == 1) {
		::SendMessage(m_hrbtIconStyle2D, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		::SendMessage(m_hrbtIconStyle3D, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
	}

	m_hcboLinewidth = ::GetDlgItem(m_hWnd, IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_COMBOBOX_LINEWIDTH);

	::SendMessage(m_hcboLinewidth, CB_ADDSTRING, 0, (LPARAM)"1");
	::SendMessage(m_hcboLinewidth, CB_ADDSTRING, 0, (LPARAM)"2");
	::SendMessage(m_hcboLinewidth, CB_ADDSTRING, 0, (LPARAM)"3");

	::SendMessage(m_hcboLinewidth, CB_SETCURSEL, (WPARAM)(m_nLinewidth - 1), (LPARAM)0);

	m_htxtSize = ::GetDlgItem(m_hWnd, IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_EDITTEXT_SIZE);

	std::string strSize = std::to_string(m_nSize);
	char const* szSize = strSize.c_str();
	::SendMessage(m_htxtSize, WM_SETTEXT, 0, (LPARAM)szSize);

	m_htrbUCSIconSize = ::CreateWindow(
		TRACKBAR_CLASS,
		NULL,
		WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_BOTH | TBS_NOTICKS | TBS_DOWNISLEFT/* | TBS_TOOLTIPS*/,
		83,
		128,
		150,
		25,
		m_hWnd,
		(HMENU)IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_TRACKBAR_SIZE,
		s_hInstance,
		(LPVOID)0
	);

	if (m_htrbUCSIconSize == NULL) {
		ErrorHandler();
		return FALSE;
	}

#pragma warning( push )
#pragma warning( disable : 6387)
	::SendMessage(m_htrbUCSIconSize, TBM_SETRANGE, (WPARAM)FALSE, (LPARAM)MAKELONG(m_nRange.cx, m_nRange.cy));
	::SendMessage(m_htrbUCSIconSize, PBM_SETSTEP, (WPARAM)1, 0);
	::SendMessage(m_htrbUCSIconSize, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)50);
#pragma warning( pop )

	//setFont(m_htrbUCSIconSize);

	m_hcboModelspaceIconColour = ::GetDlgItem(m_hWnd, IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_COMBOBOX_MODELSPACEICONCOLOUR);

	::SendMessage(m_hcboModelspaceIconColour, CB_ADDSTRING, 0, (LPARAM)"Red");
	::SendMessage(m_hcboModelspaceIconColour, CB_ADDSTRING, 0, (LPARAM)"Yellow");
	::SendMessage(m_hcboModelspaceIconColour, CB_ADDSTRING, 0, (LPARAM)"Green");
	::SendMessage(m_hcboModelspaceIconColour, CB_ADDSTRING, 0, (LPARAM)"Cyan");
	::SendMessage(m_hcboModelspaceIconColour, CB_ADDSTRING, 0, (LPARAM)"Blue");
	::SendMessage(m_hcboModelspaceIconColour, CB_ADDSTRING, 0, (LPARAM)"Magenta");
	::SendMessage(m_hcboModelspaceIconColour, CB_ADDSTRING, 0, (LPARAM)"Black");
	::SendMessage(m_hcboModelspaceIconColour, CB_ADDSTRING, 0, (LPARAM)"Select Colour");

	::SendMessage(m_hcboModelspaceIconColour, CB_SETCURSEL, (WPARAM)(6), (LPARAM)0);

	m_hcboLayoutTabIconColour = ::GetDlgItem(m_hWnd, IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_COMBOBOX_LAYOUTTABICONCOLOUR);

	::SendMessage(m_hcboLayoutTabIconColour, CB_ADDSTRING, 0, (LPARAM)"Red");
	::SendMessage(m_hcboLayoutTabIconColour, CB_ADDSTRING, 0, (LPARAM)"Yellow");
	::SendMessage(m_hcboLayoutTabIconColour, CB_ADDSTRING, 0, (LPARAM)"Green");
	::SendMessage(m_hcboLayoutTabIconColour, CB_ADDSTRING, 0, (LPARAM)"Cyan");
	::SendMessage(m_hcboLayoutTabIconColour, CB_ADDSTRING, 0, (LPARAM)"Blue");
	::SendMessage(m_hcboLayoutTabIconColour, CB_ADDSTRING, 0, (LPARAM)"Magenta");
	::SendMessage(m_hcboLayoutTabIconColour, CB_ADDSTRING, 0, (LPARAM)"Black");
	::SendMessage(m_hcboLayoutTabIconColour, CB_ADDSTRING, 0, (LPARAM)"Select Colour");

	::SendMessage(m_hcboLayoutTabIconColour, CB_SETCURSEL, (WPARAM)(4), (LPARAM)0);

	m_hbtnTintForXYZ = ::GetDlgItem(m_hWnd, IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_CHECKBOX_SINGLECOLOUR);

	if (m_bTintForXYZ) {
		::SendMessage(m_hbtnTintForXYZ, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	}
	else {
		::SendMessage(m_hbtnTintForXYZ, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	}

	m_hOK = ::GetDlgItem(m_hWnd, IDOK);
	m_hCancel = ::GetDlgItem(m_hWnd, IDCANCEL);
	m_hHelp = ::GetDlgItem(m_hWnd, IDHELP);

	RECT rcWindow = { 0,0,0,0 };
	getWindowState(WINDOWSTATE_POSITION, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\UCSIcon");

	::SetWindowPos(m_hWnd,
		HWND_TOP,
		rcWindow.left,
		rcWindow.top,
		463,
		349,
		SWP_SHOWWINDOW | SWP_NOSIZE
	);

	return FALSE;
}
int dlg_UCSIcon::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	BOOL nResult = FALSE;
	//LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);
	//if ((nmhdr->code == TRBN_THUMBPOSCHANGING) && (nmhdr->hwndFrom == m_htrbUCSIconSize)) {
	//	log("Log::dlg_UCSIcon::wm_notify() TRBN_THUMBPOSCHANGING ");
	//	NMTRBTHUMBPOSCHANGING* nmtrb = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(lParam);
	//	switch (nmtrb->nReason) {
	//		case TB_THUMBTRACK:
	//		case TB_PAGEUP:
	//		case TB_PAGEDOWN:
	//		case TB_THUMBPOSITION:
	//		case TB_TOP:
	//		case TB_BOTTOM:
	//		case TB_ENDTRACK: {
	//			
	//			break;
	//		}
	//	}
	//}

#define lpnm (LPNMHDR(lParam))

	if (lpnm->idFrom == IDC_VIEW_DISPLAY_UCSICON_PROPERTIES_TRACKBAR_SIZE) {
		//switch (lpnm->code) {
		//	case SB_ENDSCROLL:
		//	case SB_LEFT:
		//	case SB_RIGHT:
		//	case SB_LINELEFT:
		//	case SB_LINERIGHT:
		//	case SB_PAGELEFT:
		//	case SB_PAGERIGHT:
		//	case SB_THUMBPOSITION:
		//	case SB_THUMBTRACK: {
		//
		//		break;
		//	}
		//}

		long nCurSel = (long)::SendMessage(m_htrbUCSIconSize, TBM_GETPOS, 0, 0);
		if (nCurSel < m_nRange.cx) {
			nCurSel = m_nRange.cx;
		}
		else if (nCurSel > m_nRange.cy) {
			nCurSel = m_nRange.cy;
		}

		std::string strSize = std::to_string(nCurSel);
		char const* szSize = strSize.c_str();
		::SendMessage(m_htxtSize, WM_SETTEXT, 0, (LPARAM)szSize);

		nResult = TRUE;
	}

	return (int)nResult;
}
int dlg_UCSIcon::wm_paint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);

	::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);

	return TRUE;
}
int dlg_UCSIcon::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::dlg_UCSIcon::wm_syscommand() SC_CLOSE");
			break;
		}
		default: {
			break;
		}
	}

	return FALSE;
}
