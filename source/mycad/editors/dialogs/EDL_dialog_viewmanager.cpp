#include "SYS_systemwin32.h"

#include "EDL_dialog_viewmanager.h"
#include "EDL_resource.h"

#include "DRW_view.h"
#include "DRW_viewmanager.h"

#include "resources.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>
#include <commctrl.h>
#include <cassert>
#include <intsafe.h>//Required for IntPtrToInt
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.

using namespace mycad;

HTREEITEM hParent;
HTREEITEM hInsertBefore;
HTREEITEM hRoot;

dlg_ViewManager::dlg_ViewManager(COMMANDINFO* command, DynamicInputWindow* commandwindows[], ViewManager* viewmanager, SIZE viewport, CommandMode mode)
	: CommandDialog(command, commandwindows, mode),
	m_lpViewManager(viewmanager),
	m_nWidth(viewport.cx),
	m_nHeight(viewport.cy),
	m_hViews(0),
	m_fDivider(0.5f),
	m_lpSelectedView(0),
	m_bSelectedView(false),
	m_hCurrentView(0),
	m_hTrvw(0), m_hLstvw(0),
	m_hOK(0), m_hCancel(0), m_hApply(0), m_hHelp(0),
	m_hSetCurrent(0), m_hNew(0), m_hUpdateLayers(0), m_hEditBoundaries(0), m_hDelete(0)
{

}
dlg_ViewManager::dlg_ViewManager(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, ViewManager* viewmanager, SIZE viewport, CommandMode mode)
	: CommandDialog(command, commandwindows, text, mode),
	m_lpViewManager(viewmanager),
	m_nWidth(viewport.cx),
	m_nHeight(viewport.cy),
	m_hViews(0),
	m_fDivider(0.5f),
	m_lpSelectedView(0),
	m_bSelectedView(false),
	m_hCurrentView(0),
	m_hTrvw(0), m_hLstvw(0),
	m_hOK(0), m_hCancel(0), m_hApply(0), m_hHelp(0),
	m_hSetCurrent(0), m_hNew(0), m_hUpdateLayers(0), m_hEditBoundaries(0), m_hDelete(0)
{

}
dlg_ViewManager::~dlg_ViewManager()
{
	
}

void dlg_ViewManager::wm_initdialog_treeview_imagelist()
{
	HINSTANCE hModule = ::LoadLibrary("mycaddata.dll");
	if (hModule != NULL) {
		HIMAGELIST hImageList{ 0 };
		hImageList = ::ImageList_Create(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 | ILC_MASK, 14, 0);

		HICON hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_VIEWMANAGER_CURRENT));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_VIEWMANAGER_MODELVIEWS));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_VIEWMANAGER_LAYOUTVIEWS));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_VIEWMANAGER_PRESETVIEWS));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_TOP));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_BOTTOM));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_LEFT));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_RIGHT));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_FRONT));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_BACK));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_SWISOMETRIC));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_SEISOMETRIC));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_NEISOMETRIC));
		::ImageList_AddIcon(hImageList, hIcon);

		hIcon = ::LoadIcon(s_hInstance, MAKEINTRESOURCE(IDI_VIEW_3DVIEWS_NWISOMETRIC));
		::ImageList_AddIcon(hImageList, hIcon);

		//FreeLibrary(s_hInstance);
		::DestroyIcon(hIcon);

		::SendMessage(m_hTrvw, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)hImageList);

		::FreeLibrary(hModule);
	}
}
void dlg_ViewManager::wm_initdialog_treeview()
{
	TVINSERTSTRUCT tvins;
	tvins.hParent = NULL;
	tvins.hInsertAfter = TVI_ROOT;

	tvins.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvins.item.pszText = (LPSTR)"Current";
	tvins.item.iImage = 0;
	tvins.item.iSelectedImage = 0;
	::SendMessage(m_hTrvw, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	tvins.item.pszText = (LPSTR)"Model Views";
	tvins.item.iImage = 1;
	tvins.item.iSelectedImage = 1;
	::SendMessage(m_hTrvw, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	tvins.item.pszText = (LPSTR)"Layout Views";
	tvins.item.iImage = 2;
	tvins.item.iSelectedImage = 2;
	::SendMessage(m_hTrvw, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	tvins.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE;
	tvins.item.pszText = (LPSTR)"Preset Views";
	tvins.item.iImage = 3;
	tvins.item.iSelectedImage = 3;
	tvins.item.stateMask = TVIS_EXPANDED;
	tvins.item.state = TVIS_EXPANDED;
	hParent = (HTREEITEM)::SendMessage(m_hTrvw, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	tvins.hParent = hParent;
	tvins.hInsertAfter = TVI_LAST;

	tvins.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	for (int i = 0; i < m_lpViewManager->getSize(); i++) {
		tvins.item.pszText = m_lpViewManager->getObjects()[i]->getName();
		tvins.item.iImage = 4 + i;
		tvins.item.iSelectedImage = 4 + i;
		tvins.item.lParam = (LPARAM)m_lpViewManager->getObjects()[i];
		::SendMessage(m_hTrvw, TVM_INSERTITEM, 0, (LPARAM)&tvins);
	}
}

void dlg_ViewManager::wm_size()
{
	::SetWindowPos(m_hViews,NULL,11,28,m_cx - 22,m_cy - 80,SWP_SHOWWINDOW);
	::SetWindowPos(m_hTrvw,NULL,24,46,static_cast<int>(m_cx * m_fDivider) - 120,m_cy - 109,SWP_SHOWWINDOW);
	::SetWindowPos(m_hLstvw,NULL,24 + static_cast<int>(m_cx * m_fDivider) - 120 + 3,49,m_cx - static_cast<int>(m_cx * m_fDivider) - 75,m_cy - 112,SWP_SHOWWINDOW);
	::SetWindowPos(m_hOK,NULL,m_cx - 102 - 86 - 86 - 86,m_cy - 34,76,24,SWP_SHOWWINDOW);
	::SetWindowPos(m_hCancel,NULL,m_cx - 102 - 86 - 86,m_cy - 34,76,24,SWP_SHOWWINDOW);
	::SetWindowPos(m_hApply,NULL,m_cx - 102 - 86,m_cy - 34,76,24,SWP_SHOWWINDOW);
	::SetWindowPos(m_hHelp,NULL,m_cx - 102,m_cy - 34,76,24,SWP_SHOWWINDOW);
	::SetWindowPos(m_hSetCurrent,NULL,m_cx - 147,49,120,24,SWP_SHOWWINDOW);
	::SetWindowPos(m_hNew,NULL,m_cx - 147,79,120,24,SWP_SHOWWINDOW);
	::SetWindowPos(m_hUpdateLayers,NULL,m_cx - 147,109,120,24,SWP_SHOWWINDOW);
	::SetWindowPos(m_hEditBoundaries,NULL,m_cx - 147,139,120,24,SWP_SHOWWINDOW);
	::SetWindowPos(m_hDelete,NULL,m_cx - 147,169,120,24,SWP_SHOWWINDOW);
}

int dlg_ViewManager::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	BOOL nResult = FALSE;

	switch (LOWORD(wParam)) {
		case IDOK: {
			RECT rcWindow = { 0,0,0,0 };
			GetWindowRect(m_hWnd, &rcWindow);
			setWindowState(WINDOWSTATE_POSITION, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\ViewManager");
			setWindowState(WINDOWSTATE_SIZE, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\ViewManager");

			nResult = ::EndDialog(m_hWnd, wParam);

			break;
		}
		case IDCANCEL: {
			nResult = ::EndDialog(m_hWnd, wParam);
			break;
		}
		case IDC_VIEW_VIEWMANAGER_BUTTON_APPLY: {
			SendCommandParam(GetParent(m_hWnd), IDD_VIEWMANAGER, IDC_VIEW_VIEWMANAGER_BUTTON_APPLY, MAKELPARAM(0, static_cast<int>(m_lpSelectedView->getPresetView())));
			break;
		}
		case IDHELP: {

			break;
		}
		case IDC_VIEW_VIEWMANAGER_BUTTON_SETCURRENT: {
			if (m_lpSelectedView == nullptr) {
				break;
			}
			
			m_bSelectedView = true;
			m_lpSelectedView->size(m_nWidth, m_nHeight);

			nResult = EnableWindow(m_hApply, TRUE);
			UpdateWindow(m_hApply);

			break;
		}
		case IDC_VIEW_VIEWMANAGER_BUTTON_NEW: {

			break;
		}
		case IDC_VIEW_VIEWMANAGER_BUTTON_UPDATELAYERS: {

			break;
		}
		case IDC_VIEW_VIEWMANAGER_BUTTON_EDITBOUNDARIES: {

			break;
		}
		case IDC_VIEW_VIEWMANAGER_BUTTON_DELETE: {

			break;
		}
	}

	return (int)nResult;
}
int dlg_ViewManager::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam)
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
int dlg_ViewManager::wm_destroy()
{
	DestroyWindow(m_hTrvw);
	DestroyWindow(m_hLstvw);
	return FALSE;
}
int dlg_ViewManager::wm_erasebkgnd(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);
	return FALSE;
}
int dlg_ViewManager::wm_getminmaxinfo(LPARAM lParam)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

	lpMMI->ptMinTrackSize.x = 601;
	lpMMI->ptMinTrackSize.y = 395;

	return TRUE;
}
int dlg_ViewManager::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	setIcon(m_hWnd, MAKEINTRESOURCE(IDI_MYCAD));

	m_hCurrentView = ::GetDlgItem(m_hWnd, IDC_VIEW_VIEWMANAGER_LTEXT_CURRENTVIEW);
	m_hViews = ::GetDlgItem(m_hWnd, IDC_VIEW_VIEWMANAGER_GROUPBOX_VIEWS);

	m_hTrvw = ::CreateWindow(
		WC_TREEVIEW,
		NULL,
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS,
		24,
		46,
		static_cast<int>(m_cx * m_fDivider) - 120,
		m_cy - 109,
		m_hWnd,
		(HMENU)IDC_VIEW_VIEWMANAGER_TREEVIEW_VIEWS,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hTrvw == NULL) {
		ErrorHandler();
		return FALSE;
	}

	setFont(m_hTrvw);

	m_hLstvw = ::CreateWindow(
		WC_LISTVIEW,
		NULL,
		WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
		24 + static_cast<int>(m_cx * m_fDivider) - 120 + 3,
		49,
		m_cx - static_cast<int>(m_cx * m_fDivider) - 75,
		m_cy - 112,
		m_hWnd,
		(HMENU)IDC_VIEW_VIEWMANAGER_EDIT,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hLstvw == NULL) {
		ErrorHandler();
		return FALSE;
	}

	setFont(m_hLstvw);

	wm_initdialog_treeview_imagelist();
	wm_initdialog_treeview();

	m_hOK = ::GetDlgItem(m_hWnd, IDOK);
	m_hCancel = ::GetDlgItem(m_hWnd, IDCANCEL);

	m_hApply = ::GetDlgItem(m_hWnd, IDC_VIEW_VIEWMANAGER_BUTTON_APPLY);
	EnableWindow(m_hApply, FALSE);

	m_hHelp = ::GetDlgItem(m_hWnd, IDHELP);

	m_hSetCurrent = ::GetDlgItem(m_hWnd, IDC_VIEW_VIEWMANAGER_BUTTON_SETCURRENT);
	m_hNew = ::GetDlgItem(m_hWnd, IDC_VIEW_VIEWMANAGER_BUTTON_NEW);
	m_hUpdateLayers = ::GetDlgItem(m_hWnd, IDC_VIEW_VIEWMANAGER_BUTTON_UPDATELAYERS);
	m_hEditBoundaries = ::GetDlgItem(m_hWnd, IDC_VIEW_VIEWMANAGER_BUTTON_EDITBOUNDARIES);
	m_hDelete = ::GetDlgItem(m_hWnd, IDC_VIEW_VIEWMANAGER_BUTTON_DELETE);

	RECT rcWindow = { 0,0,0,0 };
	getWindowState(WINDOWSTATE_POSITION, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\ViewManager");
	getWindowState(WINDOWSTATE_SIZE, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\ViewManager");

	RECT rcWindowRect = { 0,0,0,0 };
	::GetWindowRect(m_hWnd, &rcWindowRect);
	rcWindowRect.right -= rcWindowRect.left;
	rcWindowRect.bottom -= rcWindowRect.top;

	rcWindowRect.left = rcWindow.left;
	rcWindowRect.top = rcWindow.top;

	::SetWindowPos(m_hWnd, HWND_TOP, rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom, SWP_SHOWWINDOW);

	BOOL nResult = EqualRect(&rcWindow, &rcWindowRect);
	if (nResult == TRUE) {
		RECT rcClient = { 0,0,0,0 };
		GetClientRect(m_hWnd, &rcClient);
		m_cx = rcClient.right;
		m_cy = rcClient.bottom;
		wm_size();
	}

	return FALSE;
}
int dlg_ViewManager::wm_move(LPARAM lParam)
{
	m_x = GET_X_LPARAM(lParam);
	m_y = GET_Y_LPARAM(lParam);

	return TRUE;
}
int dlg_ViewManager::wm_notify(WPARAM wParam, LPARAM lParam)
{
	BOOL nResult = FALSE;
	int nId = LOWORD(wParam);
	//int nCode = HIWORD(wParam);

#define lpnm (LPNMHDR(lParam))

	switch (nId) {
		case IDC_VIEW_VIEWMANAGER_TREEVIEW_VIEWS: {
			switch (lpnm->code) {
				case NM_CLICK: {
					log("Log::dlg_ViewManager::wm_notify() NM_CLICK");
					HTREEITEM hSelected{ 0 };
					TV_ITEM tvi{ 0 };
					char Text[255] = "";
					memset(&tvi, 0, sizeof(tvi));
					hSelected = (HTREEITEM)::SendMessage(m_hTrvw, TVM_GETNEXTITEM, (WPARAM)TVGN_CARET, (LPARAM)hSelected);

					if (hSelected == NULL)
					{
						break;
					}
					else {
						EnableWindow(m_hApply, TRUE);
					}

					TreeView_EnsureVisible(m_hTrvw, hSelected);
					::SendMessage(m_hTrvw, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)hSelected);
					//bool flagSelected = true;
					tvi.mask = TVIF_TEXT;
					tvi.pszText = Text;
					tvi.cchTextMax = 256;
					tvi.hItem = hSelected;

					nResult = TRUE;

					break;
				}
				case NM_DBLCLK: {
					log("Log::dlg_ViewManager::wm_notify() NM_DBLCLK");
					break;
				}
				case NM_RCLICK: {
					log("Log::dlg_ViewManager::wm_notify() NM_RCLICK");
					break;
				}
				case TVN_SELCHANGING: {
					log("Log::dlg_ViewManager::wm_notify() TVN_SELCHANGING");
					break;
				}
				case TVN_SELCHANGED: {
					log("Log::dlg_ViewManager::wm_notify() TVN_SELCHANGED");

					LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
					TV_ITEM itmNew = pnmtv->itemNew;
					m_lpSelectedView = (View*)itmNew.lParam;

					nResult = TRUE;

					break;
				}
				case TVN_ITEMEXPANDING: {
					log("Log::dlg_ViewManager::wm_notify() TVN_ITEMEXPANDING");
					break;
				}
				case TVN_ITEMEXPANDED: {
					log("Log::dlg_ViewManager::wm_notify() TVN_ITEMEXPANDED");
					break;
				}
				default: {
					break;
				}
			}
		}
		default: {
			break;
		}
	}

	return (int)nResult;
}
int dlg_ViewManager::wm_paint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);

	::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);

	return TRUE;
}
int dlg_ViewManager::wm_showwindow(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return FALSE;
}
int dlg_ViewManager::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	wm_size();

	return TRUE;
}
int dlg_ViewManager::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::dlg_ViewManager::wm_syscommand() SC_CLOSE");
			//if (::SendMessage(m_hWnd, WM_DESTROY, 0, 0) == 0) {
			//	
			//}
			break;
		}
		default: {
			break;
		}
	}

	return FALSE;
}
