#include "SYS_systemwin32.h"

#include "EDL_dialog_ucs.h"
#include "EDL_resource.h"

#include "resources.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>
#include <commctrl.h>
#include <intsafe.h>//Required for IntPtrToInt
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.

using namespace mycad;

typedef struct tag_dlghdr {
	HWND m_hTabControl;//Tabcontrol handle.
	HWND hDisplay;//Current child dialog box handle.
	RECT m_rcDisplay;//Display rectangle for the tab control.
	DLGTEMPLATEEX* lpRes[3];//Pointer array to child dialog id.
} DLGHDR;

dlg_UCS::dlg_UCS(COMMANDINFO* command, DynamicInputWindow* commandwindows[], int tabindex, CommandMode mode)
	: CommandDialog(command, commandwindows, mode),
	m_nCurSel(tabindex),
	m_hOK(0),
	m_hCancel(0),
	m_hHelp(0)
{

}
dlg_UCS::dlg_UCS(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, int tabindex, CommandMode mode)
	: CommandDialog(command, commandwindows, text, mode),
	m_nCurSel(tabindex),
	m_hOK(0),
	m_hCancel(0),
	m_hHelp(0)
{

}
dlg_UCS::~dlg_UCS()
{

}

VOID WINAPI dlg_UCS::s_OnChildDialogInit(HWND hDlg)
{
	HWND hParent = GetParent(hDlg);
	DLGHDR *pHdr = (DLGHDR*)::GetWindowLongPtr(hParent, GWLP_USERDATA);
	::SetWindowPos(hDlg,
		NULL,
		pHdr->m_rcDisplay.left + 11,
		pHdr->m_rcDisplay.top + 5,
		pHdr->m_rcDisplay.right - pHdr->m_rcDisplay.left - 11,
		pHdr->m_rcDisplay.bottom - pHdr->m_rcDisplay.top - 5,
		SWP_SHOWWINDOW
	);
	return;
}
INT_PTR CALLBACK dlg_UCS::s_nfnChildDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_INITDIALOG: {
			//log("Log::dlg_UCS::s_nfnChildDialogProc() WM_INITDIALOG");
			s_OnChildDialogInit(hDlg);



			//HWND hEdittextNewName = ::GetDlgItem(hDlg, IDC_VIEW_VIEWPORTS_EDITTEXT_NEW_NAME);
			//::SetFocus(hEdittextNewName);

			return 1;
		}
		case WM_PAINT: {
			//log("Log::dlg_UCS::s_nfnChildDialogProc() WM_PAINT");
			PAINTSTRUCT ps;

			HDC hDC = ::BeginPaint(hDlg, &ps);
			::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
			::EndPaint(hDlg, &ps);

			::ReleaseDC(hDlg, hDC);

			return 0;
		}
		case WM_CTLCOLORSTATIC: {
			//log("Log::dlg_UCS::s_nfnChildDialogProc() WM_CTLCOLORSTATIC");
			UNREFERENCED_PARAMETER(lParam);

			HDC hdcStatic = (HDC)wParam;
			::SetTextColor(hdcStatic, RGB(0, 0, 0));
			::SetBkColor(hdcStatic, RGB(255, 255, 255));

			return (INT_PTR)GetStockObject(WHITE_BRUSH);
		}
		case WM_COMMAND: {
			//log("Log::dlg_UCS::s_nfnChildDialogProc() WM_COMMAND");
			switch (LOWORD(wParam)) {
				//case IDC_VIEW_VIEWPORTS_EDITTEXT_NEW_NAME: {
				//	log("Log::dlg_UCS::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_EDITTEXT_NEW_NAME");
				//	break;
				//}
				case IDC_VIEW_VIEWPORTS_LISTBOX_NEWVIEWPORTS: {
					//log("Log::dlg_UCS::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_LISTBOX_NEWVIEWPORTS");
					switch (HIWORD(wParam)) {
						case LBN_DBLCLK: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_DBLCLK");
							return 1;
						}
						case LBN_KILLFOCUS: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_KILLFOCUS");
							return 1;
						}
						case LBN_SELCANCEL: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_SELCANCEL");
							return 1;
						}
						case LBN_SELCHANGE: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_SELCHANGE");

							


							return 1;
						}
						case LBN_SETFOCUS: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_SETFOCUS");
							return 1;
						}
						default:
							break;
					}
					break;
				}
				case IDC_VIEW_VIEWPORTS_COMBOBOX_CONTEXT: {
					//log("Log::dlg_UCS::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_COMBOBOX_CONTEXT");
					switch (HIWORD(wParam)) {
						case CBN_CLOSEUP: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_CLOSEUP");
							return 1;
						}
						case CBN_DBLCLK: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_DBLCLK");
							return 1;
						}
						case CBN_DROPDOWN: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_DROPDOWN");
							return 1;
						}
						case CBN_EDITCHANGE: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_EDITCHANGE");
							return 1;
						}
						case CBN_EDITUPDATE: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_EDITUPDATE");
							return 1;
						}
						case CBN_KILLFOCUS: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_KILLFOCUS");
							return 1;
						}
						case CBN_SELCHANGE: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_SELCHANGE");



							return 1;
						}
						case CBN_SELENDCANCEL: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_SELENDCANCEL");
							return 1;
						}
						case CBN_SELENDOK: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_SELENDOK");
							return 1;
						}
						case CBN_SETFOCUS: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_SETFOCUS");
							return 1;
						}
						default:
							break;
					}
					break;
				}
				case IDC_VIEW_VIEWPORTS_COMBOBOX_SETUP: {
					//log("Log::dlg_UCS::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_COMBOBOX_SETUP");
					switch (HIWORD(wParam)) {
						case CBN_CLOSEUP: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_CLOSEUP");
							return 1;
						}
						case CBN_DBLCLK: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_DBLCLK");
							return 1;
						}
						case CBN_DROPDOWN: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_DROPDOWN");
							return 1;
						}
						case CBN_EDITCHANGE: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_EDITCHANGE");
							return 1;
						}
						case CBN_EDITUPDATE: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_EDITUPDATE");
							return 1;
						}
						case CBN_KILLFOCUS: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_KILLFOCUS");
							return 1;
						}
						case CBN_SELCHANGE: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_SELCHANGE");


							return 1;
						}
						case CBN_SELENDCANCEL: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_SELENDCANCEL");
							return 1;
						}
						case CBN_SELENDOK: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_SELENDOK");
							return 1;
						}
						case CBN_SETFOCUS: {
							//log("Log::dlg_UCS::s_nfnChildDialogProc() CBN_SETFOCUS");
							return 1;
						}
						default:
							break;
					}
					break;
				}
				//case IDC_VIEW_VIEWPORTS_COMBOBOX_VIEW: {
				//	log("Log::dlg_UCS::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_COMBOBOX_VIEW");
				//	break;
				//}
				//case IDC_VIEW_VIEWPORTS_COMBOBOX_VISUALSTYLE: {
				//	log("Log::dlg_UCS::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_COMBOBOX_VISUALSTYLE");
				//	break;
				//}
				//case IDC_VIEW_VIEWPORTS_LISTBOX_NAMEDVIEWPORTS: {
				//	log("Log::dlg_UCS::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_LISTBOX_NAMEDVIEWPORTS");
				//	switch (HIWORD(wParam)) {
				//		case LBN_DBLCLK: {
				//			log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_DBLCLK");
				//			return 1;
				//		}
				//		case LBN_KILLFOCUS: {
				//			log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_KILLFOCUS");
				//			return 1;
				//		}
				//		case LBN_SELCANCEL: {
				//			log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_SELCANCEL");
				//			return 1;
				//		}
				//		case LBN_SELCHANGE: {
				//			log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_SELCHANGE");
				//			return 1;
				//		}
				//		case LBN_SETFOCUS: {
				//			log("Log::dlg_UCS::s_nfnChildDialogProc() LBN_SETFOCUS");
				//			return 1;
				//		}
				//		default:
				//			break;
				//	}
				//	break;
				//}
				default:
					break;
			}
			return 1;
		}
		default:
			break;
	}
	return 0;
}

RECT dlg_UCS::getMaxRect(DLGTEMPLATEEX rcRects[], int size)
{
	RECT rcTab = { 0,0,rcRects[0].cx,rcRects[0].cy };
	for (int i = 1; i < size; i++) {
		if (rcRects[i].cx > rcTab.right) {
			rcTab.right = rcRects[i].cx;
		}
		if (rcRects[i].cy > rcTab.bottom) {
			rcTab.bottom = rcRects[i].cy;
		}
	}
	return rcTab;
}
HRESULT dlg_UCS::setCurSel(HWND htabcontrol)
{
	HRESULT hResult = S_OK;
	int nItemCount = (int)::SendMessage(htabcontrol, TCM_GETITEMCOUNT, 0, 0);
	if ((m_nCurSel >= 0) && (m_nCurSel < nItemCount)) {
		int nIndex = (int)::SendMessage(htabcontrol, TCM_SETCURSEL, (WPARAM)m_nCurSel, 0);
		if (nIndex == -1) {
			hResult = ErrorHandler();
		}
	}
	else {
		hResult = E_INVALIDARG;
	}
	return hResult;
}

HRESULT dlg_UCS::wm_initdialog_tabcontrol(HWND hDlg)
{ 
	DLGHDR* pHdr = (DLGHDR*)LocalAlloc(LPTR, sizeof(DLGHDR));//Allocate memory for the DLGHDR structure.
	if (pHdr == 0) {
		ErrorHandler();
	}
	::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)pHdr);//Save a pointer to the DLGHDR structure in the window data of the dialog box. 
	LocalFree(pHdr);//Free memory for the DLGHDR structure.

	pHdr->m_hTabControl = ::CreateWindow(WC_TABCONTROL,
		0,
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		0,
		0,
		100,
		100,
		hDlg,
		(HMENU)0,
		s_hInstance,
		(LPVOID)0
	);

	if (pHdr->m_hTabControl == NULL) {
		return (HRESULT)ErrorHandler();
	}

	NONCLIENTMETRICS nclim;
	nclim.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nclim, 0);
	CreateFontIndirect(&nclim.lfCaptionFont);

	::SendMessage(pHdr->m_hTabControl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)TRUE);

	//Add a tab for each of the child dialog boxes... 
	TCITEMA tie = { TCIF_TEXT | TCIF_IMAGE,0,0,0,MAX_LOADSTRING,-1,0 };

	tie.pszText = (LPSTR)"Named UCSs";
	int nIndex = (int)::SendMessage(pHdr->m_hTabControl, TCM_INSERTITEM, 0, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"Orthographic UCSs";
	nIndex = (int)::SendMessage(pHdr->m_hTabControl, TCM_INSERTITEM, (WPARAM)1, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"Settings";
	nIndex = (int)::SendMessage(pHdr->m_hTabControl, TCM_INSERTITEM, (WPARAM)2, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	HRESULT hResult = setCurSel(pHdr->m_hTabControl);//Set current tab.
	if (hResult != S_OK) {
		return ErrorHandler();
	}
 
	pHdr->lpRes[0] = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_UCS_NAMEDUCS));
	pHdr->lpRes[1] = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_UCS_ORTHOGRAPHICUCS));
	pHdr->lpRes[2] = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_UCS_SETTINGS));

	int nSize = (int)std::size(pHdr->lpRes);
	RECT rcTab = getMaxRect(*pHdr->lpRes, nSize);//Determine a bounding rectangle that is large enough to contain the largest child dialog box. 

	::MapDialogRect(hDlg, &rcTab);//Map the rectangle from dialog box units to pixels.

	//Calculate how large to make the tab control so the display area can accommodate all the child dialog boxes.
	DWORD dwDlgBase = ::GetDialogBaseUnits();
	int cxMargin = /*LOWORD(dwDlgBase) / 4*/LOWORD(dwDlgBase) >> 2;
	int cyMargin = /*HIWORD(dwDlgBase) / 8*/HIWORD(dwDlgBase) >> 3;

	TabCtrl_AdjustRect(pHdr->m_hTabControl, TRUE, &rcTab);
	::OffsetRect(&rcTab, cxMargin - rcTab.left, cyMargin - rcTab.top);

	::CopyRect(&pHdr->m_rcDisplay, &rcTab);
	TabCtrl_AdjustRect(pHdr->m_hTabControl, FALSE, &pHdr->m_rcDisplay);//Calculate the display rectangle. 

	::SetWindowPos(pHdr->m_hTabControl,
		NULL,
		rcTab.left + 9,
		rcTab.top + 3,
		rcTab.right - rcTab.left - 5,
		rcTab.bottom - rcTab.top - 2,
		SWP_NOZORDER
	);

	wm_notify_tcn_selchange(hDlg);

	return S_OK;
}

DLGTEMPLATEEX *dlg_UCS::wm_notify_tcn_selchange_LockDlgRes(LPCTSTR lpszResName)
{
	HRSRC hrsrc = FindResource(NULL, lpszResName, RT_DIALOG);
	if (hrsrc == NULL) {
		return 0;
	}

	HGLOBAL hglb = LoadResource(s_hInstance, hrsrc);
	if (hglb == NULL) {
		return 0;
	}

	return (DLGTEMPLATEEX*)LockResource(hglb);
}
void dlg_UCS::wm_notify_tcn_selchange(HWND hDlg)
{
	DLGHDR* pHdr = (DLGHDR*)::GetWindowLongPtr(hDlg, GWLP_USERDATA);
	int nCurSel = (int)::SendMessage(pHdr->m_hTabControl, TCM_GETCURSEL, 0, 0);
	if (pHdr->hDisplay != NULL) {
		::DestroyWindow(pHdr->hDisplay);
	}
	pHdr->hDisplay = CreateDialogIndirect(s_hInstance, (DLGTEMPLATE*)pHdr->lpRes[nCurSel], hDlg, s_nfnChildDialogProc);
}

void dlg_UCS::wm_size()
{
	::SetWindowPos(m_hOK, NULL, m_cx - 102 - 86 - 86, m_cy - 34, 76, 24, SWP_SHOWWINDOW);
	::SetWindowPos(m_hCancel, NULL, m_cx - 102 - 86, m_cy - 34, 76, 24, SWP_SHOWWINDOW);
	::SetWindowPos(m_hHelp, NULL, m_cx - 102, m_cy - 34, 76, 24, SWP_SHOWWINDOW);
}

int dlg_UCS::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	BOOL nResult = FALSE;

	switch (LOWORD(wParam)) {
		case IDOK: {
			//ViewportManager::LPVIEWPORTDATA lpVprtConfigData = (ViewportManager::LPVIEWPORTDATA)::GetWindowLongPtr(hDlg, 30);//Get current value from cbWndExtra of dialog
			//SetWindowLongPtr(s_Application->getHWND(), 4, (LONG_PTR)lpVprtConfigData);//Save value in cbWndExtra of application window

			RECT rcWindow = { 0,0,0,0 };
			GetWindowRect(m_hWnd, &rcWindow);
			setWindowState(WINDOWSTATE_POSITION, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\UCS");
			setWindowState(WINDOWSTATE_SIZE, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\UCS");

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
		default:
			break;
	}

	return (int)nResult;
}
int dlg_UCS::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam)
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
int dlg_UCS::wm_destroy()
{
	return FALSE;
}
int dlg_UCS::wm_erasebkgnd(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);
	return FALSE;
}
int dlg_UCS::wm_getminmaxinfo(LPARAM lParam)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

	lpMMI->ptMinTrackSize.x = 424;
	lpMMI->ptMinTrackSize.y = 361;

	return TRUE;
}
int dlg_UCS::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	setIcon(m_hWnd, MAKEINTRESOURCE(IDI_MYCAD));

	RECT rcWindow = { 0,0,0,0 };
	getWindowState(WINDOWSTATE_POSITION, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\UCS");
	getWindowState(WINDOWSTATE_SIZE, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\UCS");

	::SetWindowPos(m_hWnd,
		HWND_TOP,
		rcWindow.left,
		rcWindow.top,
		rcWindow.right,
		rcWindow.bottom,
		SWP_SHOWWINDOW
	);

	wm_initdialog_tabcontrol(m_hWnd);

	m_hOK = ::GetDlgItem(m_hWnd, IDOK);
	m_hCancel = ::GetDlgItem(m_hWnd, IDCANCEL);
	m_hHelp = ::GetDlgItem(m_hWnd, IDHELP);

	return FALSE;
}
int dlg_UCS::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	BOOL nResult = FALSE;

#define lpnm (LPNMHDR(lParam))

	switch (lpnm->code) {
		case TCN_SELCHANGE: {
			wm_notify_tcn_selchange(m_hWnd);
			nResult = TRUE;
			break;
		}
		case TCN_SELCHANGING: {

			break;
		}
		default: {
			break;
		}
	}

	return (int)nResult;
}
int dlg_UCS::wm_paint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);

	::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);

	return TRUE;
}
int dlg_UCS::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	wm_size();

	return TRUE;
}
int dlg_UCS::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::dlg_UCS::wm_syscommand() SC_CLOSE");
			break;
		}
		default: {
			break;
		}
	}

	return FALSE;
}
