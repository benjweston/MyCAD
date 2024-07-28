#include "SYS_systemwin32.h"

#include "EPL_palette_layerpropertiesmanager.h"
#include "EPL_resource.h"

#include "resources.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.

using namespace mycad;

struct LAYERINFO {
	TCHAR szStatus[10];
	TCHAR szName[50];
	TCHAR szOn[10];
	TCHAR szFreeze[10];
};

LAYERINFO rgLayerInfo[] = {
    {"Yes", "Layer1", "Yes", "On"},
    {"No", "Layer2", "Yes", "On"},
    {"No", "Layer3", "Yes", "On"},
    {"No", "Layer4", "Yes", "On"},
    {"No", "Layer5", "Yes", "On"},
};

BOOL plt_LayerPropertiesManager::s_nVisible{ FALSE };
BOOL plt_LayerPropertiesManager::s_nDocked{ FALSE };
POINT plt_LayerPropertiesManager::s_ptPosition{ 100,50 };
SIZE plt_LayerPropertiesManager::s_szSize{ 1200,300 };

plt_LayerPropertiesManager::plt_LayerPropertiesManager()
	: WindowWin32(),
	m_hTrvw(NULL),
	m_hLstvw(NULL),
	m_nTreeviewWidth(250),
	m_nListviewAWidth(250),
	m_nToolbarHeight(31),
	m_nStatusbarHeight(42)
{
	//m_x = position.left;
	//m_y = position.top;
	//m_cx = position.right;
	//m_cy = position.bottom;
}
plt_LayerPropertiesManager::~plt_LayerPropertiesManager()
{

}

BOOL plt_LayerPropertiesManager::InitListViewColumns()
{
	TCHAR szText[MAX_LOADSTRING]{ 0 };
	LVCOLUMN lvc{ 0 };
	int iColWidth[11]{ 50, 150, 30, 50, 40, 50, 80, 80, 80, 90, 240 };
	int iCol{ 0 };
	// Initialize the LVCOLUMN structure.
	// The mask specifies that the format, width, text,
	// and subitem members of the structure are valid.
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	// Add the columns.
	for (iCol = 0; iCol < ID_LSTVW_LAYERS_COLUMNS; iCol++) {
		lvc.iSubItem = iCol;
		lvc.pszText = szText;
		lvc.cx = iColWidth[iCol];
		lvc.fmt = LVCFMT_LEFT;
		::LoadString(s_hInstance, ID_LAYERS_STATUS + iCol * 10, szText, sizeof(szText) / sizeof(TCHAR));
		if (ListView_InsertColumn(m_hLstvw, iCol, &lvc) == -1) {
			return 0;
		}
	}

	return 1;
}
BOOL plt_LayerPropertiesManager::InsertListViewItems(int cItems)
{
	LVITEM lvI{ 0 };

	// Initialize LVITEM members that are common to all items.
	lvI.pszText = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;

	// Initialize LVITEM members that are different for each item.
	for (int index = 0; index < cItems; index++) {
		lvI.iItem = index;
		lvI.iImage = index;
		if (ListView_InsertItem(m_hLstvw, &lvI) == -1) {
			return 0;
		}
	}

	return 1;
}

int plt_LayerPropertiesManager::wm_close()
{
	s_nVisible = FALSE;

	RECT rcWindow = { 0,0,0,0 };
	::GetWindowRect(m_hWnd, &rcWindow);
	setWindowState(WINDOWSTATE_POSITION, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\LayerManager");
	setWindowState(WINDOWSTATE_SIZE, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\LayerManager");

	::DestroyWindow(m_hWnd);

	return 0;
}
int plt_LayerPropertiesManager::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HRESULT hResult = S_OK;
	BOOL nResult = FALSE;

	//setIcon(m_hWnd, MAKEINTRESOURCE(IDI_MYCAD));

	RECT rc{0,0,0,0};
	::GetClientRect(m_hWnd, &rc);
	m_hTrvw = ::CreateWindow(
		WC_TREEVIEW,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		0,
		m_nToolbarHeight,
		m_nTreeviewWidth,
		rc.bottom - m_nToolbarHeight - m_nStatusbarHeight,
		m_hWnd,
		(HMENU)IDC_LAYERPROPERTIESMANAGER_TREEVIEW,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hTrvw == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		setFont(m_hTrvw);

#pragma warning( pop )
	}

	m_hLstvw = ::CreateWindow(
		WC_LISTVIEW,
		NULL,
		WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
		0,
		m_nToolbarHeight,
		m_nTreeviewWidth,
		rc.bottom - m_nToolbarHeight - m_nStatusbarHeight,
		m_hWnd,
		(HMENU)IDC_LAYERPROPERTIESMANAGER_LISTVIEW,
		s_hInstance,
		NULL
	);

	if (m_hLstvw == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		setFont(m_hLstvw);

#pragma warning( pop )
	}

	if (SUCCEEDED(hResult)) {
		nResult = InitListViewColumns();
		if (nResult == FALSE) {
			hResult = ErrorHandler();
		}
	}

	if (SUCCEEDED(hResult)) {
		nResult = InsertListViewItems(5);
		if (nResult == FALSE) {
			hResult = ErrorHandler();
		}
	}

	return 0;
}
int plt_LayerPropertiesManager::wm_destroy()
{
	::DestroyWindow(m_hLstvw);

	m_system->getWindowManager().setObjectInactive(this);
	BOOL nResult = m_system->getWindowManager().removeObject(this);
	if (nResult == TRUE) {
		log("Log::plt_LayerPropertiesManager::wm_destroy() m_system->getWindowManager().removeWindow == TRUE");
	}
	else {
		log("Log::plt_LayerPropertiesManager::wm_destroy() m_system->getWindowManager().removeWindow == FALSE");
	}
	return 0;
}
int plt_LayerPropertiesManager::wm_erasebkgnd(WPARAM wParam)
{
	//UNREFERENCED_PARAMETER(wParam);
	//return 0;

	HDC hDC = (HDC)wParam;
	RECT rcClient = { 0,0,0,0 };
	::GetClientRect(m_hWnd, &rcClient);
	::FillRect(hDC, &rcClient, (HBRUSH)(COLOR_MENU + 1));
	return TRUE;
}
int plt_LayerPropertiesManager::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

#define lpnm (LPNMHDR(lParam))

	switch (lpnm->code) {
		case LVN_GETDISPINFO: {
			NMLVDISPINFO *plvdi = (NMLVDISPINFO*)lParam;
			switch (plvdi->item.iSubItem) {
				case 0: {
					plvdi->item.pszText = rgLayerInfo[plvdi->item.iItem].szStatus;
					break;
				}
				case 1: {
					plvdi->item.pszText = rgLayerInfo[plvdi->item.iItem].szName;
					break;
				}
				case 2: {
					plvdi->item.pszText = rgLayerInfo[plvdi->item.iItem].szOn;
					break;
				}
				case 3: {
					plvdi->item.pszText = rgLayerInfo[plvdi->item.iItem].szFreeze;
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
	return 0;
}
int plt_LayerPropertiesManager::wm_paint()
{
	//PAINTSTRUCT ps;
	//HDC hDC = ::BeginPaint(m_hWnd, &ps);
	//::FillRect(hDC, &ps.rcPaint, (HBRUSH)(COLOR_MENU + 1));
	//::EndPaint(m_hWnd, &ps);
	//::ReleaseDC(m_hWnd, hDC);
	return 0;
}
int plt_LayerPropertiesManager::wm_setcursor(WPARAM wParam, LPARAM lParam) { return (int)::DefWindowProc(m_hWnd, WM_SETCURSOR, wParam, lParam); }
int plt_LayerPropertiesManager::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	int cx = GET_X_LPARAM(lParam);
	int cy = GET_Y_LPARAM(lParam);

	::SetWindowPos(m_hTrvw,
		NULL,
		0,
		m_nToolbarHeight,
		m_nTreeviewWidth,
		cy - m_nToolbarHeight - m_nStatusbarHeight,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hLstvw,
		NULL,
		m_nTreeviewWidth,
		m_nToolbarHeight,
		cx - m_nTreeviewWidth,
		cy - m_nToolbarHeight - m_nStatusbarHeight,
		SWP_SHOWWINDOW
	);

	return 0;
}
int plt_LayerPropertiesManager::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);

	switch (nID) {
		case SC_CLOSE: {
			log("Log::plt_LayerPropertiesManager::wm_syscommand() SC_CLOSE");
			break;
		}
		case SC_MAXIMIZE: {

			break;
		}
		case SC_MINIMIZE: {

			break;
		}
		case SC_RESTORE: {

			break;
		}
		default: {
			break;
		}
	}

	return 0;
}
