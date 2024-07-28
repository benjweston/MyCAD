#include "SYS_systemwin32.h"

#include "DRW_viewport.h"//DRW_viewport.h needs to be declared before EDL_dialog_viewports.h so that the dialog can use the ViewportManager::VIEWPORTDATA struct.
#include "DRW_viewportmanager.h"

#include "EDL_dialog_viewports.h"
#include "EDL_resource.h"

#include "resources.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>
#include <commctrl.h>
#include <intsafe.h>//Required for IntPtrToInt
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.

using namespace mycad;

typedef struct tagViewportConfigurationData {
	tagViewportConfigurationData() {}
	int nID{ 0 };
	ViewportManager::Configuration enConfiguration{ ViewportManager::Configuration::ActiveModelConfiguration };
	char szName[256]{ 0 };
} VIEWPORTCONFIGURATIONDATA, *LPVIEWPORTCONFIGURATIONDATA;

typedef struct tagViewportContextData {
	tagViewportContextData() {}
	int nID{ 0 };
	ViewportManager::Context enContext{ ViewportManager::Context::Display };
	char szName[256]{ 0 };
} VIEWPORTCONTEXTDATA, *LPVIEWPORTCONTEXTDATA;

typedef struct tagViewportSetupData {
	tagViewportSetupData() {}
	int nID{ 0 };
	ViewportManager::Setup enSetup{ ViewportManager::Setup::_2D };
	char szName[256]{ 0 };
} VIEWPORTSETUPDATA, *LPVIEWPORTSETUPDATA;

typedef struct tagViewportViewData {
	tagViewportViewData() {}
	int nID{ 0 };
	View::PresetView enView{ View::PresetView::Top };
	char szName[256]{ 0 };
} VIEWPORTVIEWDATA, *LPVIEWPORTVIEWDATA;

typedef struct tagViewportVisualStyleData {
	tagViewportVisualStyleData() {}
	int nID{ 10 };
	View::VisualStyle enVisualStyle{ View::VisualStyle::Wireframe2D };
	char szName[256]{ 0 };
} VIEWPORTVISUALSTYLEDATA, *LPVIEWPORTVIEWPORTVISUALSTYLEDATADATA;

ViewportManager::VIEWPORTDATA dlg_Viewports::m_VprtConfigData;

typedef struct tag_dlghdr {
	HWND m_hTabControl;//Tabcontrol handle.
	HWND hDisplay;//Current child dialog box handle.
	RECT m_rcDisplay;//Display rectangle for the tab control.
	DLGTEMPLATEEX* lpRes[2];//Pointer array to child dialog id.
} DLGHDR;

dlg_Viewports::dlg_Viewports(COMMANDINFO* command, DynamicInputWindow* commandwindows[], ViewportManager* viewportmanager, int tabindex, CommandMode mode)
	: CommandDialog(command, commandwindows, mode),
	m_lpViewportManager(viewportmanager),
	m_nCurSel(tabindex),
	m_hOK(0),
	m_hCancel(0),
	m_hHelp(0)
{

}
dlg_Viewports::dlg_Viewports(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, ViewportManager* viewportmanager, int tabindex, CommandMode mode)
	: CommandDialog(command, commandwindows, text, mode),
	m_lpViewportManager(viewportmanager),
	m_nCurSel(tabindex),
	m_hOK(0),
	m_hCancel(0),
	m_hHelp(0)
{

}
dlg_Viewports::~dlg_Viewports()
{

}

ViewportManager::LPVIEWPORTDATA dlg_Viewports::getViewportData() { return &m_VprtConfigData; }

void dlg_Viewports::s_OnListboxNewViewportsInit(HWND hDlg)
{
	VIEWPORTCONFIGURATIONDATA VprtConfigurationData[13];

	for (int i = 0; i < ARRAYSIZE(VprtConfigurationData); i++) {
		VprtConfigurationData[i].nID = i;
	}

	VprtConfigurationData[0].enConfiguration = ViewportManager::Configuration::ActiveModelConfiguration;
	VprtConfigurationData[1].enConfiguration = ViewportManager::Configuration::One;
	VprtConfigurationData[2].enConfiguration = ViewportManager::Configuration::TwoVertical;
	VprtConfigurationData[3].enConfiguration = ViewportManager::Configuration::TwoHorizontal;
	VprtConfigurationData[4].enConfiguration = ViewportManager::Configuration::ThreeRight;
	VprtConfigurationData[5].enConfiguration = ViewportManager::Configuration::ThreeLeft;
	VprtConfigurationData[6].enConfiguration = ViewportManager::Configuration::ThreeAbove;
	VprtConfigurationData[7].enConfiguration = ViewportManager::Configuration::ThreeBelow;
	VprtConfigurationData[8].enConfiguration = ViewportManager::Configuration::ThreeVertical;
	VprtConfigurationData[9].enConfiguration = ViewportManager::Configuration::ThreeHorizontal;
	VprtConfigurationData[10].enConfiguration = ViewportManager::Configuration::FourEqual;
	VprtConfigurationData[11].enConfiguration = ViewportManager::Configuration::FourRight;
	VprtConfigurationData[12].enConfiguration = ViewportManager::Configuration::FourLeft;

	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_ACTIVE_MODEL_CONFIGURATION, VprtConfigurationData[0].szName, sizeof(VprtConfigurationData[0].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_SINGLE, VprtConfigurationData[1].szName, sizeof(VprtConfigurationData[1].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_2_VERTICAL, VprtConfigurationData[2].szName, sizeof(VprtConfigurationData[2].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_2_HORIZONTAL, VprtConfigurationData[3].szName, sizeof(VprtConfigurationData[3].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_3_RIGHT, VprtConfigurationData[4].szName, sizeof(VprtConfigurationData[4].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_3_LEFT, VprtConfigurationData[5].szName, sizeof(VprtConfigurationData[5].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_3_ABOVE, VprtConfigurationData[6].szName, sizeof(VprtConfigurationData[6].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_3_BELOW, VprtConfigurationData[7].szName, sizeof(VprtConfigurationData[7].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_3_VERTICAL, VprtConfigurationData[8].szName, sizeof(VprtConfigurationData[8].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_3_HORIZONTAL, VprtConfigurationData[9].szName, sizeof(VprtConfigurationData[9].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_4_EQUAL, VprtConfigurationData[10].szName, sizeof(VprtConfigurationData[10].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_4_RIGHT, VprtConfigurationData[11].szName, sizeof(VprtConfigurationData[11].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_4_LEFT, VprtConfigurationData[12].szName, sizeof(VprtConfigurationData[12].szName) / sizeof(char));

	HWND hViewports_List = ::GetDlgItem(hDlg, IDC_VIEW_VIEWPORTS_LISTBOX_NEWVIEWPORTS);
	for (int i = 0; i < ARRAYSIZE(VprtConfigurationData); i++) {
		int nPos = (int)::SendMessage(hViewports_List, LB_ADDSTRING, 0, (LPARAM)VprtConfigurationData[i].szName);
		::SendMessage(hViewports_List, LB_SETITEMDATA, (WPARAM)nPos, (LPARAM)i);
	}
}
void dlg_Viewports::s_OnComboboxContextInit(HWND hDlg)
{
	VIEWPORTCONTEXTDATA VprtContextData[2];

	VprtContextData[0].nID = 0;
	VprtContextData[0].enContext = ViewportManager::Context::Display;
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_CONTEXT_DISPLAY, VprtContextData[0].szName, sizeof(VprtContextData[0].szName) / sizeof(char));

	VprtContextData[1].nID = 1;
	VprtContextData[1].enContext = ViewportManager::Context::CurrentViewport;
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_CONTEXT_CURRENTVIEWPORT, VprtContextData[1].szName, sizeof(VprtContextData[1].szName) / sizeof(char));

	HWND hComboboxContext = ::GetDlgItem(hDlg, IDC_VIEW_VIEWPORTS_COMBOBOX_CONTEXT);
	for (int i = 0; i < ARRAYSIZE(VprtContextData); i++) {
		int nPos = (int)::SendMessage(hComboboxContext, CB_ADDSTRING, 0, (LPARAM)VprtContextData[i].szName);
		::SendMessage(hComboboxContext, CB_SETITEMDATA, (WPARAM)nPos, (LPARAM)i);
	}
	::SendMessage(hComboboxContext, CB_SETCURSEL, 0, 0);

	EnableWindow(hComboboxContext, FALSE);
}
void dlg_Viewports::s_OnComboboxSetupInit(HWND hDlg)
{
	VIEWPORTSETUPDATA VprtSetupData[2];

	VprtSetupData[0].nID = 0;
	VprtSetupData[0].enSetup = ViewportManager::Setup::_2D;
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_SETUP_2D, VprtSetupData[0].szName, sizeof(VprtSetupData[0].szName) / sizeof(char));

	VprtSetupData[1].nID = 1;
	VprtSetupData[1].enSetup = ViewportManager::Setup::_3D;
	::LoadString(s_hInstance, ID_VIEW_VIEWPORTS_NEW_VIEWPORTS_SETUP_3D, VprtSetupData[1].szName, sizeof(VprtSetupData[1].szName) / sizeof(char));

	HWND hComboboxSetup = ::GetDlgItem(hDlg, IDC_VIEW_VIEWPORTS_COMBOBOX_SETUP);
	for (int i = 0; i < ARRAYSIZE(VprtSetupData); i++) {
		int nPos = (int)::SendMessage(hComboboxSetup, CB_ADDSTRING, 0, (LPARAM)VprtSetupData[i].szName);
		::SendMessage(hComboboxSetup, CB_SETITEMDATA, (WPARAM)nPos, (LPARAM)i);
	}
	::SendMessage(hComboboxSetup, CB_SETCURSEL, 0, 0);
}
void dlg_Viewports::s_OnComboboxViewInit(HWND hDlg)
{
	VIEWPORTVIEWDATA VprtViewData[11];

	for (int i = 0; i < ARRAYSIZE(VprtViewData); i++) {
		VprtViewData[i].nID = i;
	}

	VprtViewData[0].enView = View::PresetView::Current;
	VprtViewData[1].enView = View::PresetView::Top;
	VprtViewData[2].enView = View::PresetView::Bottom;
	VprtViewData[3].enView = View::PresetView::Left;
	VprtViewData[4].enView = View::PresetView::Right;
	VprtViewData[5].enView = View::PresetView::Front;
	VprtViewData[6].enView = View::PresetView::Back;
	VprtViewData[7].enView = View::PresetView::SEIsometric;
	VprtViewData[8].enView = View::PresetView::SWIsometric;
	VprtViewData[9].enView = View::PresetView::NWIsometric;
	VprtViewData[10].enView = View::PresetView::NEIsometric;

	//const char *szCurrent = L"*Current*";

	//wcsncpy_s(VprtViewData[0].szName, szCurrent, MAX_LOADSTRING - 1);
	::LoadString(s_hInstance, ID_VIEW_CURRENT, VprtViewData[0].szName, sizeof(VprtViewData[0].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_TOP, VprtViewData[1].szName, sizeof(VprtViewData[1].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_BOTTOM, VprtViewData[2].szName, sizeof(VprtViewData[2].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_LEFT, VprtViewData[3].szName, sizeof(VprtViewData[3].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_RIGHT, VprtViewData[4].szName, sizeof(VprtViewData[4].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_FRONT, VprtViewData[5].szName, sizeof(VprtViewData[5].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_BACK, VprtViewData[6].szName, sizeof(VprtViewData[6].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_SWISOMETRIC, VprtViewData[7].szName, sizeof(VprtViewData[7].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_SEISOMETRIC, VprtViewData[8].szName, sizeof(VprtViewData[8].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_NEISOMETRIC, VprtViewData[9].szName, sizeof(VprtViewData[9].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_NWISOMETRIC, VprtViewData[10].szName, sizeof(VprtViewData[10].szName) / sizeof(char));

	HWND hComboboxView = ::GetDlgItem(hDlg, IDC_VIEW_VIEWPORTS_COMBOBOX_VIEW);
	for (int i = 0; i < ARRAYSIZE(VprtViewData); i++) {
		int nPos = (int)::SendMessage(hComboboxView, CB_ADDSTRING, 0, (LPARAM)VprtViewData[i].szName);
		::SendMessage(hComboboxView, CB_SETITEMDATA, (WPARAM)nPos, (LPARAM)i);
	}
	::SendMessage(hComboboxView, CB_SETCURSEL, 0, 0);
}
void dlg_Viewports::s_OnComboboxVisualStyleInit(HWND hDlg)
{
	VIEWPORTVISUALSTYLEDATA VprtVisualStyleData[12];

	for (int i = 0; i < ARRAYSIZE(VprtVisualStyleData); i++) {
		VprtVisualStyleData[i].nID = i;
	}

	VprtVisualStyleData[0].enVisualStyle = View::VisualStyle::Current;
	VprtVisualStyleData[1].enVisualStyle = View::VisualStyle::Wireframe2D;
	VprtVisualStyleData[2].enVisualStyle = View::VisualStyle::Wireframe3D;
	VprtVisualStyleData[3].enVisualStyle = View::VisualStyle::Hidden2D;
	VprtVisualStyleData[4].enVisualStyle = View::VisualStyle::Hidden3D;
	VprtVisualStyleData[5].enVisualStyle = View::VisualStyle::Conceptual;
	VprtVisualStyleData[6].enVisualStyle = View::VisualStyle::Realistic;
	VprtVisualStyleData[7].enVisualStyle = View::VisualStyle::Shaded;
	VprtVisualStyleData[8].enVisualStyle = View::VisualStyle::ShadedWithEdges;
	VprtVisualStyleData[9].enVisualStyle = View::VisualStyle::ShadesOfGrey;
	VprtVisualStyleData[10].enVisualStyle = View::VisualStyle::Sketchy;
	VprtVisualStyleData[11].enVisualStyle = View::VisualStyle::XRay;

	//wcsncpy_s(VprtVisualStyleData[0].szName, szCurrent, MAX_LOADSTRING - 1);
	::LoadString(s_hInstance, ID_CURRENT, VprtVisualStyleData[0].szName, sizeof(VprtVisualStyleData[0].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_2DWIREFRAME, VprtVisualStyleData[1].szName, sizeof(VprtVisualStyleData[1].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_3DWIREFRAME, VprtVisualStyleData[2].szName, sizeof(VprtVisualStyleData[2].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_HIDDEN, VprtVisualStyleData[3].szName, sizeof(VprtVisualStyleData[3].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_3DHIDDEN, VprtVisualStyleData[4].szName, sizeof(VprtVisualStyleData[4].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_CONCEPTUAL, VprtVisualStyleData[5].szName, sizeof(VprtVisualStyleData[5].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_REALISTIC, VprtVisualStyleData[6].szName, sizeof(VprtVisualStyleData[6].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_SHADED, VprtVisualStyleData[7].szName, sizeof(VprtVisualStyleData[7].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_SHADEDWITHEDGES, VprtVisualStyleData[8].szName, sizeof(VprtVisualStyleData[8].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_SHADESOFGREY, VprtVisualStyleData[9].szName, sizeof(VprtVisualStyleData[9].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_SKETCHY, VprtVisualStyleData[10].szName, sizeof(VprtVisualStyleData[10].szName) / sizeof(char));
	::LoadString(s_hInstance, ID_WIREFRAME, VprtVisualStyleData[11].szName, sizeof(VprtVisualStyleData[11].szName) / sizeof(char));

	HWND hComboboxVisualStyle = ::GetDlgItem(hDlg, IDC_VIEW_VIEWPORTS_COMBOBOX_VISUALSTYLE);
	for (int i = 0; i < ARRAYSIZE(VprtVisualStyleData); i++) {
		int nPos = (int)::SendMessage(hComboboxVisualStyle, CB_ADDSTRING, 0, (LPARAM)VprtVisualStyleData[i].szName);
		::SendMessage(hComboboxVisualStyle, CB_SETITEMDATA, (WPARAM)nPos, (LPARAM)i);
	}
	::SendMessage(hComboboxVisualStyle, CB_SETCURSEL, 0, 0);
}

RECT dlg_Viewports::getMaxRect(DLGTEMPLATEEX rcRects[], int size)
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
HRESULT dlg_Viewports::setCurSel(HWND htabcontrol)
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

VOID WINAPI dlg_Viewports::s_OnChildDialogInit(HWND hDlg)
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
INT_PTR CALLBACK dlg_Viewports::s_nfnChildDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_INITDIALOG: {
			//log("Log::dlg_Viewports::s_nfnChildDialogProc() WM_INITDIALOG");
			s_OnChildDialogInit(hDlg);

			//ViewportManager::VIEWPORTDATA *lpVprtConfigData = (ViewportManager::LPVIEWPORTDATA)::GetWindowLongPtr(GetParent(hDlg), 30);//Get initial value from cbWndExtra of parent dialog

			s_OnListboxNewViewportsInit(hDlg);
			s_OnComboboxContextInit(hDlg);
			s_OnComboboxSetupInit(hDlg);
			s_OnComboboxViewInit(hDlg);
			s_OnComboboxVisualStyleInit(hDlg);

			//HWND hEdittextNewName = ::GetDlgItem(hDlg, IDC_VIEW_VIEWPORTS_EDITTEXT_NEW_NAME);
			//::SetFocus(hEdittextNewName);

			return 1;
		}
		case WM_PAINT: {
			//log("Log::dlg_Viewports::s_nfnChildDialogProc() WM_PAINT");
			PAINTSTRUCT ps;

			HDC hDC = ::BeginPaint(hDlg, &ps);
			::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
			::EndPaint(hDlg, &ps);

			::ReleaseDC(hDlg, hDC);

			return 0;
		}
		case WM_CTLCOLORSTATIC: {
			//log("Log::dlg_Viewports::s_nfnChildDialogProc() WM_CTLCOLORSTATIC");
			UNREFERENCED_PARAMETER(lParam);

			HDC hdcStatic = (HDC)wParam;
			::SetTextColor(hdcStatic, RGB(0, 0, 0));
			::SetBkColor(hdcStatic, RGB(255, 255, 255));

			return (INT_PTR)GetStockObject(WHITE_BRUSH);
		}
		case WM_COMMAND: {
			//log("Log::dlg_Viewports::s_nfnChildDialogProc() WM_COMMAND");
			switch (LOWORD(wParam)) {
				//case IDC_VIEW_VIEWPORTS_EDITTEXT_NEW_NAME: {
				//	log("Log::dlg_Viewports::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_EDITTEXT_NEW_NAME");
				//	break;
				//}
				case IDC_VIEW_VIEWPORTS_LISTBOX_NEWVIEWPORTS: {
					//log("Log::dlg_Viewports::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_LISTBOX_NEWVIEWPORTS");
					switch (HIWORD(wParam)) {
						case LBN_DBLCLK: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_DBLCLK");
							return 1;
						}
						case LBN_KILLFOCUS: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_KILLFOCUS");
							return 1;
						}
						case LBN_SELCANCEL: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_SELCANCEL");
							return 1;
						}
						case LBN_SELCHANGE: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_SELCHANGE");

							HWND hListboxNewViewports = (HWND)lParam;
							int itemIndex = (int)::SendMessage(hListboxNewViewports, LB_GETCURSEL, 0, 0);
							
							//ViewportManager::VIEWPORTDATA *lpVprtConfigData = (ViewportManager::LPVIEWPORTDATA)::GetWindowLongPtr(GetParent(hDlg), 30);//Get value from cbWndExtra of parent dialog
							//lpVprtConfigData->enConfiguration = static_cast<ViewportManager::Configuration>(itemIndex);
							//SetWindowLongPtr(GetParent(hDlg), 30, (LONG_PTR)lpVprtConfigData);//Save value in cbWndExtra of parent dialog
							
							m_VprtConfigData.enConfiguration = static_cast<ViewportManager::Configuration>(itemIndex);

							HWND hComboboxContext = ::GetDlgItem(hDlg, IDC_VIEW_VIEWPORTS_COMBOBOX_CONTEXT);
							if (m_VprtConfigData.enConfiguration == ViewportManager::Configuration::ActiveModelConfiguration) {
								EnableWindow(hComboboxContext, FALSE);
							}
							else {
								EnableWindow(hComboboxContext, TRUE);
							}

							return 1;
						}
						case LBN_SETFOCUS: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_SETFOCUS");
							return 1;
						}
						default:
							break;
					}
					break;
				}
				case IDC_VIEW_VIEWPORTS_COMBOBOX_CONTEXT: {
					//log("Log::dlg_Viewports::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_COMBOBOX_CONTEXT");
					switch (HIWORD(wParam)) {
						case CBN_CLOSEUP: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_CLOSEUP");
							return 1;
						}
						case CBN_DBLCLK: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_DBLCLK");
							return 1;
						}
						case CBN_DROPDOWN: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_DROPDOWN");
							return 1;
						}
						case CBN_EDITCHANGE: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_EDITCHANGE");
							return 1;
						}
						case CBN_EDITUPDATE: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_EDITUPDATE");
							return 1;
						}
						case CBN_KILLFOCUS: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_KILLFOCUS");
							return 1;
						}
						case CBN_SELCHANGE: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_SELCHANGE");

							HWND hComboboxContext = (HWND)lParam;
							int itemIndex = (int)::SendMessage(hComboboxContext, CB_GETCURSEL, 0, 0);

							//ViewportManager::VIEWPORTDATA *lpVprtConfigData = (ViewportManager::LPVIEWPORTDATA)::GetWindowLongPtr(GetParent(hDlg), 30);//Get value from cbWndExtra of parent dialog
							//lpVprtConfigData->enContext = static_cast<ViewportManager::Context>(itemIndex);
							//SetWindowLongPtr(GetParent(hDlg), 30, (LONG_PTR)lpVprtConfigData);//Save value in cbWndExtra of parent dialog

							m_VprtConfigData.enContext = static_cast<ViewportManager::Context>(itemIndex);

							return 1;
						}
						case CBN_SELENDCANCEL: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_SELENDCANCEL");
							return 1;
						}
						case CBN_SELENDOK: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_SELENDOK");
							return 1;
						}
						case CBN_SETFOCUS: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_SETFOCUS");
							return 1;
						}
						default:
							break;
					}
					break;
				}
				case IDC_VIEW_VIEWPORTS_COMBOBOX_SETUP: {
					//log("Log::dlg_Viewports::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_COMBOBOX_SETUP");
					switch (HIWORD(wParam)) {
						case CBN_CLOSEUP: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_CLOSEUP");
							return 1;
						}
						case CBN_DBLCLK: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_DBLCLK");
							return 1;
						}
						case CBN_DROPDOWN: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_DROPDOWN");
							return 1;
						}
						case CBN_EDITCHANGE: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_EDITCHANGE");
							return 1;
						}
						case CBN_EDITUPDATE: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_EDITUPDATE");
							return 1;
						}
						case CBN_KILLFOCUS: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_KILLFOCUS");
							return 1;
						}
						case CBN_SELCHANGE: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_SELCHANGE");

							HWND hComboboxSetup = (HWND)lParam;
							int itemIndex = (int)::SendMessage(hComboboxSetup, CB_GETCURSEL, 0, 0);

							//ViewportManager::VIEWPORTDATA *lpVprtConfigData = (ViewportManager::LPVIEWPORTDATA)::GetWindowLongPtr(GetParent(hDlg), 30);//Get value from cbWndExtra of parent dialog
							//lpVprtConfigData->enContext = static_cast<ViewportManager::Context>(itemIndex);
							//SetWindowLongPtr(GetParent(hDlg), 30, (LONG_PTR)lpVprtConfigData);//Save value in cbWndExtra of parent dialog

							m_VprtConfigData.enSetup = static_cast<ViewportManager::Setup>(itemIndex);

							return 1;
						}
						case CBN_SELENDCANCEL: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_SELENDCANCEL");
							return 1;
						}
						case CBN_SELENDOK: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_SELENDOK");
							return 1;
						}
						case CBN_SETFOCUS: {
							//log("Log::dlg_Viewports::s_nfnChildDialogProc() CBN_SETFOCUS");
							return 1;
						}
						default:
							break;
					}
					break;
				}
				//case IDC_VIEW_VIEWPORTS_COMBOBOX_VIEW: {
				//	log("Log::dlg_Viewports::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_COMBOBOX_VIEW");
				//	break;
				//}
				//case IDC_VIEW_VIEWPORTS_COMBOBOX_VISUALSTYLE: {
				//	log("Log::dlg_Viewports::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_COMBOBOX_VISUALSTYLE");
				//	break;
				//}
				//case IDC_VIEW_VIEWPORTS_LISTBOX_NAMEDVIEWPORTS: {
				//	log("Log::dlg_Viewports::s_nfnChildDialogProc() IDC_VIEW_VIEWPORTS_LISTBOX_NAMEDVIEWPORTS");
				//	switch (HIWORD(wParam)) {
				//		case LBN_DBLCLK: {
				//			log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_DBLCLK");
				//			return 1;
				//		}
				//		case LBN_KILLFOCUS: {
				//			log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_KILLFOCUS");
				//			return 1;
				//		}
				//		case LBN_SELCANCEL: {
				//			log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_SELCANCEL");
				//			return 1;
				//		}
				//		case LBN_SELCHANGE: {
				//			log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_SELCHANGE");
				//			return 1;
				//		}
				//		case LBN_SETFOCUS: {
				//			log("Log::dlg_Viewports::s_nfnChildDialogProc() LBN_SETFOCUS");
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

HRESULT dlg_Viewports::wm_initdialog_tabcontrol(HWND hDlg)
{
	DLGHDR* pHdr = (DLGHDR*)LocalAlloc(LPTR, sizeof(DLGHDR));//Allocate memory for the DLGHDR structure.

	if (pHdr == NULL) {
		return ErrorHandler();
	}

	::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)pHdr);//Save a pointer to the DLGHDR structure in the window data of the dialog box. 

	pHdr->m_hTabControl = ::CreateWindow(
		WC_TABCONTROL,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		12,
		26,
		580 - 23,
		470 - 64,
		hDlg,
		(HMENU)0,
		s_hInstance,
		(LPVOID)0
	);
	
	if (pHdr->m_hTabControl == NULL) {
		return ErrorHandler();
	}

	NONCLIENTMETRICS nclim;
	nclim.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nclim, 0);
	CreateFontIndirect(&nclim.lfCaptionFont);

	::SendMessage(pHdr->m_hTabControl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)TRUE);

	//Add a tab for each of the child dialog boxes... 
	TCITEMA tie = { TCIF_TEXT | TCIF_IMAGE,0,0,0,MAX_LOADSTRING,-1,0 };

	tie.pszText = (LPSTR)"New Viewports";
	int nIndex = (int)::SendMessage(pHdr->m_hTabControl, TCM_INSERTITEM, 0, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"Named Viewports";
	nIndex = (int)::SendMessage(pHdr->m_hTabControl, TCM_INSERTITEM, (WPARAM)1, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	HRESULT hResult = setCurSel(pHdr->m_hTabControl);//Set current tab.
	if (hResult != S_OK) {
		return ErrorHandler();
	}
 
	pHdr->lpRes[0] = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_VIEWPORTS_NEW_VIEWPORTS));
	pHdr->lpRes[1] = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_VIEWPORTS_NAMED_VIEWPORTS));

	int nSize = (int)std::size(pHdr->lpRes);
	RECT rcTab = getMaxRect(*pHdr->lpRes, nSize);//Determine a bounding rectangle that is large enough to contain the largest child dialog box. 

	::MapDialogRect(hDlg, &rcTab);//Map the rectangle from dialog box units to pixels.

	//Calculate the tab control size so that the display area can accommodate all the child dialog boxes.
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

	LocalFree(pHdr);//Free memory for the DLGHDR structure.

	return S_OK;
}

DLGTEMPLATEEX *dlg_Viewports::wm_notify_tcn_selchange_LockDlgRes(LPCTSTR lpszResName)
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
void dlg_Viewports::wm_notify_tcn_selchange(HWND hDlg)
{
	DLGHDR* pHdr = (DLGHDR*)::GetWindowLongPtr(hDlg, GWLP_USERDATA);
	int nCurSel = (int)::SendMessage(pHdr->m_hTabControl, TCM_GETCURSEL, 0, 0);
	if (pHdr->hDisplay != NULL) {
		::DestroyWindow(pHdr->hDisplay);
	}
	pHdr->hDisplay = CreateDialogIndirect(s_hInstance, (DLGTEMPLATE*)pHdr->lpRes[nCurSel], hDlg, s_nfnChildDialogProc);
}

int dlg_Viewports::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL nResult = FALSE;
	switch (LOWORD(wParam)) {
		case IDOK: {
			RECT rcWindow = { 0,0,0,0 };
			GetWindowRect(m_hWnd, &rcWindow);
			setWindowState(WINDOWSTATE_POSITION, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\Viewports");

			nResult = ::EndDialog(m_hWnd, wParam);

			SendCommandParam(GetParent(m_hWnd), IDD_VIEWPORTS, IDC__VPORTS, (LPARAM)&m_VprtConfigData);

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
int dlg_Viewports::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam)
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
int dlg_Viewports::wm_destroy()
{
	return FALSE;
}
int dlg_Viewports::wm_erasebkgnd(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);
	return FALSE;
}
int dlg_Viewports::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	setIcon(m_hWnd, MAKEINTRESOURCE(IDI_MYCAD));

	m_hOK = ::GetDlgItem(m_hWnd, IDOK);
	m_hCancel = ::GetDlgItem(m_hWnd, IDCANCEL);
	m_hHelp = ::GetDlgItem(m_hWnd, IDHELP);

	RECT rcWindow = { 0,0,0,0 };
	getWindowState(WINDOWSTATE_POSITION, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\Viewports");

	::SetWindowPos(m_hWnd,
		HWND_TOP,
		rcWindow.left,
		rcWindow.top,
		580,
		470,
		SWP_SHOWWINDOW | SWP_NOSIZE
	);

	wm_initdialog_tabcontrol(m_hWnd);

	return FALSE;
}
int dlg_Viewports::wm_notify(WPARAM wParam, LPARAM lParam)
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
int dlg_Viewports::wm_paint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);

	::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);

	return TRUE;
}
int dlg_Viewports::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::dlg_Viewports::wm_syscommand() SC_CLOSE");
			break;
		}
		default: {
			break;
		}
	}

	return FALSE;
}
