#include "SYS_systemwin32.h"

#include "wm_tooltip.h"

#include "EDL_dialog_draftingsettings.h"
#include "EDL_resource.h"

#include "EDL_dialog_options.h"
#include "EDL_resource.h"

#include "CMD_commandmanager.h"

#include "resources.h"

#include "xml_util.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>
#include <commctrl.h>
#include <intsafe.h>//Required for IntPtrToInt
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.

using namespace mycad;
using namespace tinyxml2;

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }
#endif

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

SnapAndGridTab::SnapAndGridTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
SnapAndGridTab::~SnapAndGridTab()
{

}



PolarTrackingTab::PolarTrackingTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
PolarTrackingTab::~PolarTrackingTab()
{

}



ObjectSnapTab::ObjectSnapTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
ObjectSnapTab::~ObjectSnapTab()
{

}



ObjectSnap3DTab::ObjectSnap3DTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
ObjectSnap3DTab::~ObjectSnap3DTab()
{

}



DynamicInputTab::DynamicInputTab(HWND htabcontrol)
	: dlg_Tab(htabcontrol),
	m_hbtnEnablePointerInput(0),
	m_hbtnEnableDimensionInput(0),
	m_hgrbPointerInput(0),
	m_hbtnPointerInputSettings(0),
	m_hgrbDimensionInput(0),
	m_hbtnDimensionInputSettings(0),
	m_hgrbDynamicPrompts(0),
	m_hbtnDraftingTooltipAppearance(0),
	m_hbtnShowInputNearCrosshairs(0),
	m_hbtnShowAdditionalTooltips(0),
	m_nDynmode{ 3,3 },
	m_nDynprompt{ 1,1 },
	m_nDyninfotips{ 1,1 },
	m_lpTooltip{ 0,0,0,0 }
{
	log("Log::DynamicInputTab::DynamicInputTab");
}
DynamicInputTab::~DynamicInputTab()
{
	log("Log::DynamicInputTab::DynamicInputTab");
}

int DynamicInputTab::getDynmode(int index) { return m_nDynmode[index]; }
int DynamicInputTab::getDynprompt(int index) { return m_nDynprompt[index]; }
int DynamicInputTab::getDyninfotips(int index) { return m_nDyninfotips[index]; }

int DynamicInputTab::wm_command(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	BOOL nResult = FALSE;

	switch (LOWORD(wParam)) {
		case IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_ENABLEPOINTERINPUT: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					setDynmode();
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_ENABLEDIMENSIONINPUT: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					setDynmode();
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_SHOWINPUTNEARCROSSHAIRS: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					setDynprompt();
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_SHOWADDITIONALTOOLTIPS: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					setDyninfotips();
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		default: {
			break;
		}
	}

	return (int)nResult;
}
int DynamicInputTab::wm_destroy()
{
	log("Log::DynamicInputTab::wm_destroy");

	return 1;
}
int DynamicInputTab::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	m_hbtnEnablePointerInput = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_ENABLEPOINTERINPUT);

	char chPointerInput[MAX_LOADSTRING]{ 0 };
	::LoadString(NULL, ID_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_ENABLEPOINTERINPUT_TOOLTIP, chPointerInput, sizeof(chPointerInput) / sizeof(TCHAR));
	m_lpTooltip[0] = new Tooltip(
		WS_POPUP | TTS_ALWAYSTIP,
		m_hbtnEnablePointerInput,
		s_hInstance,
		TTF_IDISHWND | TTF_SUBCLASS,
		(UINT_PTR)m_hbtnEnablePointerInput,
		chPointerInput,
		400
	);


	m_hbtnEnableDimensionInput = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_ENABLEDIMENSIONINPUT);

	char chDimensionInput[MAX_LOADSTRING]{ 0 };
	::LoadString(NULL, ID_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_ENABLEDIMENSIONINPUT_TOOLTIP, chDimensionInput, sizeof(chDimensionInput) / sizeof(TCHAR));
	m_lpTooltip[1] = new Tooltip(
		WS_POPUP | TTS_ALWAYSTIP,
		m_hbtnEnableDimensionInput,
		s_hInstance,
		TTF_IDISHWND | TTF_SUBCLASS,
		(UINT_PTR)m_hbtnEnableDimensionInput,
		chDimensionInput,
		400
	);

	m_hgrbPointerInput = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_GROUPBOX_POINTERINPUT);
	m_hbtnPointerInputSettings = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_POINTERINPUT_SETTINGS);

	m_hgrbDimensionInput = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_GROUPBOX_DIMENSIONINPUT);
	m_hbtnDimensionInputSettings = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_DIMENSIONINPUT_SETTINGS);

	m_hgrbDynamicPrompts = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_GROUPBOX_DYNAMICPROMPTS);
	m_hbtnDraftingTooltipAppearance = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_DRAFTINGTOOLTIPAPPEARANCE);

	m_hbtnShowInputNearCrosshairs = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_SHOWINPUTNEARCROSSHAIRS);

	char chInputNearCrosshairs[MAX_LOADSTRING]{ 0 };
	::LoadString(NULL, ID_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_SHOWINPUTNEARCROSSHAIRS_TOOLTIP, chInputNearCrosshairs, sizeof(chInputNearCrosshairs) / sizeof(TCHAR));
	m_lpTooltip[2] = new Tooltip(
		WS_POPUP | TTS_ALWAYSTIP,
		m_hbtnShowInputNearCrosshairs,
		s_hInstance,
		TTF_IDISHWND | TTF_SUBCLASS,
		(UINT_PTR)m_hbtnShowInputNearCrosshairs,
		chInputNearCrosshairs,
		400
	);

	m_hbtnShowAdditionalTooltips = ::GetDlgItem(m_hWnd, IDC_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_SHOWADDITIONALTOOLTIPS);

	char chAdditionalTooltips[MAX_LOADSTRING]{ 0 };
	::LoadString(NULL, ID_TOOLS_DRAFTINGSETTINGS_DYNAMICINPUT_SHOWADDITIONALTOOLTIPS_TOOLTIP, chAdditionalTooltips, sizeof(chAdditionalTooltips) / sizeof(TCHAR));
	m_lpTooltip[3] = new Tooltip(
		WS_POPUP | TTS_ALWAYSTIP,
		m_hbtnShowAdditionalTooltips,
		s_hInstance,
		TTF_IDISHWND | TTF_SUBCLASS,
		(UINT_PTR)m_hbtnShowAdditionalTooltips,
		chAdditionalTooltips,
		400
	);

	wm_initdialog_getDynmode();
	wm_initdialog_getDynprompt();
	wm_initdialog_getDyninfotips();

	getXMLData();
	
	return 1;
}
int DynamicInputTab::wm_notify(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

#define lpnm (LPNMHDR(lParam))

	//switch (lpnm->code) {
	//	case TCN_SELCHANGING: {//Tabcontrol selection changing
	//	return 1;
	//	}
	//}
	return 0;
}
int DynamicInputTab::wm_size(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	::SetWindowPos(m_hbtnEnablePointerInput,
		NULL,
		5,
		19,
		200,
		14,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hgrbPointerInput,
		NULL,
		5,
		40,
		m_cx / 2 - 8,
		133,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnPointerInputSettings,
		NULL,
		m_cx / 4-58,
		141,
		120,
		23,
		SWP_SHOWWINDOW
	);




	::SetWindowPos(m_hbtnEnableDimensionInput,
		NULL,
		m_cx / 2 + 5,
		19,
		200,
		14,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hgrbDimensionInput,
		NULL,
		m_cx / 2 + 8,
		40,
		m_cx / 2 - 8,
		133,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnDimensionInputSettings,
		NULL,
		3 * m_cx / 4-56,
		141,
		120,
		23,
		SWP_SHOWWINDOW
	);



	::SetWindowPos(m_hgrbDynamicPrompts,
		NULL,
		5,
		178,
		m_cx - 5,
		107,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnShowInputNearCrosshairs,
		NULL,
		m_cx / 2 + 5,
		195,
		210,
		30,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnShowAdditionalTooltips,
		NULL,
		m_cx / 2 + 5,
		235,
		210,
		30,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnDraftingTooltipAppearance,
		NULL,
		m_cx / 2 - 80,
		300,
		160,
		23,
		SWP_SHOWWINDOW
	);

	return 0;
}

void DynamicInputTab::setDynmode()
{
	LRESULT lResult = BST_UNCHECKED;

	int nEnablePointerInput = 0;
	int nEnableDimensionInput = 0;

	lResult = ::SendMessage(m_hbtnEnablePointerInput, BM_GETCHECK, 0, 0);
	if (lResult == BST_CHECKED) {
		nEnablePointerInput = 1;
	}

	lResult = ::SendMessage(m_hbtnEnableDimensionInput, BM_GETCHECK, 0, 0);
	if (lResult == BST_CHECKED) {
		nEnableDimensionInput = 1;
	}

	if ((nEnablePointerInput == 0) && (nEnableDimensionInput == 0)) {
		m_nDynmode[0] = 0;
	}
	if ((nEnablePointerInput == 1) && (nEnableDimensionInput == 0)) {
		m_nDynmode[0] = 1;
	}
	if ((nEnablePointerInput == 0) && (nEnableDimensionInput == 1)) {
		m_nDynmode[0] = 2;
	}
	if ((nEnablePointerInput == 1) && (nEnableDimensionInput == 1)) {
		m_nDynmode[0] = 3;
	}
}
void DynamicInputTab::setDynprompt()
{
	LRESULT lResult = BST_UNCHECKED;

	lResult = ::SendMessage(m_hbtnShowInputNearCrosshairs, BM_GETCHECK, 0, 0);
	if (lResult == BST_CHECKED) {
		m_nDynprompt[0] = 1;
	}
	else {
		m_nDynprompt[0] = 0;
	}

}
void DynamicInputTab::setDyninfotips()
{
	LRESULT lResult = BST_UNCHECKED;

	lResult = ::SendMessage(m_hbtnShowAdditionalTooltips, BM_GETCHECK, 0, 0);
	if (lResult == BST_CHECKED) {
		m_nDyninfotips[0] = 1;
	}
	else {
		m_nDyninfotips[0] = 0;
	}
}

void DynamicInputTab::wm_initdialog_getDynmode()
{
	m_nDynmode[0] = CommandWindow::getSystemVariable(IDC_DYNMODE);
	m_nDynmode[1] = m_nDynmode[0];

	if ((m_nDynmode[0] == 1) || (m_nDynmode[0] == 3)) {
		::SendMessage(m_hbtnEnablePointerInput, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	}
	else {
		::SendMessage(m_hbtnEnablePointerInput, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	}

	if ((m_nDynmode[0] == 2) || (m_nDynmode[0] == 3)) {
		::SendMessage(m_hbtnEnableDimensionInput, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	}
	else {
		::SendMessage(m_hbtnEnableDimensionInput, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	}
}
void DynamicInputTab::wm_initdialog_getDynprompt()
{
	m_nDynprompt[0] = CommandWindow::getSystemVariable(IDC_DYNPROMPT);
	m_nDynprompt[1] = m_nDynprompt[0];

	if (m_nDynprompt[0] == 0) {
		::SendMessage(m_hbtnShowInputNearCrosshairs, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	}
	else if (m_nDynprompt[0] == 1) {
		::SendMessage(m_hbtnShowInputNearCrosshairs, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	}
}
void DynamicInputTab::wm_initdialog_getDyninfotips()
{
	m_nDyninfotips[0] = CommandWindow::getSystemVariable(IDC_DYNINFOTIPS);
	m_nDyninfotips[1] = m_nDyninfotips[0];

	if (m_nDyninfotips[0] == 0) {
		::SendMessage(m_hbtnShowAdditionalTooltips, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	}
	else if (m_nDyninfotips[0] == 1) {
		::SendMessage(m_hbtnShowAdditionalTooltips, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	}
}

int DynamicInputTab::getXMLData()
{
	XMLError eResult = XML_SUCCESS;

	const char* pchSolutionDir = EXPAND(SOLDIR);
	char chSolutionDir[MAX_LOADSTRING]{ 0 };
	char chSolutionDirTemp[MAX_LOADSTRING]{ 0 };
	::strcpy_s(chSolutionDirTemp, pchSolutionDir);//Copy const char* into char[].
	char* substr = chSolutionDirTemp + 1;//Remove the quotes at the beginning of the string.
	substr[::strlen(substr) - 2] = '\0';//Remove the quotes and colon at the end of the string.
	::strcpy_s(chSolutionDir, substr);
	char chData[MAX_LOADSTRING]{ "source\\mycad\\editors\\dialogs\\EDL_draftingsettings.xml" };
	::strcat_s(chSolutionDir, chData);

	tinyxml2::XMLDocument xmlDoc;
	eResult = xmlDoc.LoadFile(chSolutionDir);
	XMLCheckResult(eResult);

	XMLNode* lpRoot = xmlDoc.FirstChild();
	if (lpRoot == nullptr) return XML_ERROR_FILE_READ_ERROR;

	XMLElement* lpDynamicInput = lpRoot->FirstChildElement("DynamicInput");
	if (lpDynamicInput == nullptr) return XML_ERROR_PARSING_ELEMENT;

	//XMLElement* lpShowCommandPromptsNearCrosshairs = lpDynamicInput->FirstChildElement("ShowCommandPromptsNearCrosshairs");
	//if (lpShowCommandPromptsNearCrosshairs) {
	//	int nDYNPROMPT = 0;
	//	if (xmlutil::getXMLInt(lpShowCommandPromptsNearCrosshairs, nDYNPROMPT) == XML_SUCCESS) {
	//		log("Log::Application::getXMLData() nDYNPROMPT =  %d", nDYNPROMPT);
	//		s_strDynamicInputData.nDYNPROMPT = nDYNPROMPT;

	//		if (s_strDynamicInputData.nDYNPROMPT == 0) {
	//			::SendMessage(m_hbtnShowInputNearCrosshairs, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	//		}
	//		else if (s_strDynamicInputData.nDYNPROMPT == 1) {
	//			::SendMessage(m_hbtnShowInputNearCrosshairs, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	//		}
	//	}
	//}

	//XMLElement* lpShowAdditionalTooltips = lpDynamicInput->FirstChildElement("ShowAdditionalTooltips");
	//if (lpShowAdditionalTooltips) {
	//	int nDYNINFOTIPS = 0;
	//	if (xmlutil::getXMLInt(lpShowAdditionalTooltips, nDYNINFOTIPS) == XML_SUCCESS) {
	//		log("Log::Application::getXMLData() nDYNINFOTIPS =  %d", nDYNINFOTIPS);
	//		s_strDynamicInputData.nDYNINFOTIPS = nDYNINFOTIPS;

	//		if (s_strDynamicInputData.nDYNINFOTIPS == 0) {
	//			::SendMessage(m_hbtnShowAdditionalTooltips, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	//		}
	//		else if (s_strDynamicInputData.nDYNINFOTIPS == 1) {
	//			::SendMessage(m_hbtnShowAdditionalTooltips, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	//		}
	//	}
	//}

	return static_cast<int>(eResult);
}



QuickPropertiesTab::QuickPropertiesTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
QuickPropertiesTab::~QuickPropertiesTab()
{
	
}



SelectionCyclingTab::SelectionCyclingTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
SelectionCyclingTab::~SelectionCyclingTab()
{

}



dlg_DraftingSettings::dlg_DraftingSettings(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CommandDialog(command, commandwindows, mode),
	m_hTabControl(0),
	m_lpTab(0),
	m_rcDisplay({ 0,0,0,0 }),
	m_lpRes(0),
	m_nCurSel(0),
	m_hOptions(0),
	m_hOK(0),
	m_hCancel(0),
	m_hHelp(0),
	m_lpSnapAndGridTab(nullptr),
	m_lpPolarTrackingTab(nullptr),
	m_lpObjectSnapTab(nullptr),
	m_lpObjectSnap3DTab(nullptr),
	m_lpDynamicInputTab(nullptr),
	m_lpQuickPropertiesTab(nullptr),
	m_lpSelectionCyclingTab(nullptr),
	m_lpOptions(nullptr)
{

}
dlg_DraftingSettings::dlg_DraftingSettings(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CommandDialog(command, commandwindows, text, mode),
	m_hTabControl(0),
	m_lpTab(0),
	m_rcDisplay({ 0,0,0,0 }),
	m_lpRes(0),
	m_nCurSel(0),
	m_hOptions(0),
	m_hOK(0),
	m_hCancel(0),
	m_hHelp(0),
	m_lpSnapAndGridTab(nullptr),
	m_lpPolarTrackingTab(nullptr),
	m_lpObjectSnapTab(nullptr),
	m_lpObjectSnap3DTab(nullptr),
	m_lpDynamicInputTab(nullptr),
	m_lpQuickPropertiesTab(nullptr),
	m_lpSelectionCyclingTab(nullptr),
	m_lpOptions(nullptr)
{

}
dlg_DraftingSettings::~dlg_DraftingSettings()
{

}

INT_PTR CALLBACK dlg_DraftingSettings::s_nfnChildDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;

	if (hDlg) {
		WindowWin32* window = nullptr;
		if (uMsg == WM_INITDIALOG) {
			window = (WindowWin32*)lParam;
			window->setHWND(hDlg);

			//window->setSystem((SystemWin32*)getSystem());

			SetWindowLongPtr(hDlg, DWLP_USER, lParam);
		}
		else {
			window = (WindowWin32*)GetWindowLongPtr(hDlg, DWLP_USER);
		}

		if (window) {
			switch (uMsg) {
				case WM_COMMAND: {
					lResult = window->wm_command(wParam, lParam);
					break;
				}
				case WM_CTLCOLOREDIT: {
					lResult = window->wm_ctlcoloredit(wParam, lParam);
					break;
				}
				case WM_CTLCOLORLISTBOX: {
					lResult = window->wm_ctlcolorlistbox(wParam, lParam);
					break;
				}
				case WM_CTLCOLORSTATIC: {
					return window->wm_ctlcolorstatic(wParam, lParam);
				}
				case WM_ERASEBKGND: {
					lResult = window->wm_erasebkgnd(wParam);
					break;
				}
				case WM_GETMINMAXINFO: {
					lResult = window->wm_getminmaxinfo(lParam);
					break;
				}
				case WM_INITDIALOG: {
					lResult = window->wm_initdialog(wParam, lParam);
					break;
				}
				case WM_NOTIFY: {
					lResult = window->wm_notify(wParam, lParam);
					break;
				}
				case WM_PAINT: {
					window->wm_paint();
					lResult = ::DefWindowProc(hDlg, uMsg, wParam, lParam);
					break;
				}
				case WM_SETCURSOR: {
					lResult = window->wm_setcursor(wParam, lParam);
					break;
				}
				case WM_SIZE: {
					lResult = window->wm_size(wParam, lParam);
					break;
				}
				default: {
					return 0;
				}
			}
		}
		else {
			// Event found for a window before the pointer to the class has been set.
			log("Log::dlg_DraftingSettings::s_nfnChildDialogProc: window event before creation.");
			return 0;
		}
	}
	else {
		// Events without valid hWnd
		log("Log::dlg_DraftingSettings::s_nfnChildDialogProc: event without window.");
	}

	return lResult;
}

void dlg_DraftingSettings::createOptions()
{
	DynamicInputWindow* m_lpCommandWindows[3] = { (DynamicInputWindow*)m_lpTextWindow, (DynamicInputWindow*)m_lpCommandLine, (DynamicInputWindow*)m_lpDynamicInput };
	m_lpOptions = new dlg_Options(&CommandManager::s_mCommandByID.at(IDC_OPTIONS), m_lpCommandWindows, 6, CommandMode::NONE);//Default tab is Drafting(6).
	if (m_system->createDialog(m_lpOptions, IDD_OPTIONS, m_system->getApplication()->getHWND()) == IDOK) {
		log("Log::dlg_DraftingSettings::createOptions() IDOK");
	}

	if (m_lpOptions) {
		delete m_lpOptions;
		m_lpOptions = NULL;
	}
}

HRESULT dlg_DraftingSettings::wm_initdialog_tabcontrol(HWND hDlg)
{
	m_hTabControl = ::CreateWindow(WC_TABCONTROL,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		11,
		5,
		m_cx - 38,
		m_cy - 82,
		hDlg,
		(HMENU)0,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hTabControl == NULL) {
		return ErrorHandler();
	}

	NONCLIENTMETRICS nclim;
	nclim.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nclim, 0);
	CreateFontIndirect(&nclim.lfCaptionFont);

	::SendMessage(m_hTabControl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)TRUE);

	//Add a tab for each of the child dialog boxes... 
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;

	tie.pszText = (LPSTR)"Snap and Grid";
	int nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, 0, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	m_lpSnapAndGridTab = new SnapAndGridTab(m_hTabControl);

	tie.pszText = (LPSTR)"Polar Tracking";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)1, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	m_lpPolarTrackingTab = new PolarTrackingTab(m_hTabControl);

	tie.pszText = (LPSTR)"Object Snap";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)2, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	m_lpObjectSnapTab = new ObjectSnapTab(m_hTabControl);

	tie.pszText = (LPSTR)"3D Object Snap";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)3, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	m_lpObjectSnap3DTab = new ObjectSnap3DTab(m_hTabControl);

	tie.pszText = (LPSTR)"Dynamic Input";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)4, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	m_lpDynamicInputTab = new DynamicInputTab(m_hTabControl);

	tie.pszText = (LPSTR)"Quick Properties";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)5, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	m_lpQuickPropertiesTab = new QuickPropertiesTab(m_hTabControl);

	tie.pszText = (LPSTR)"Selection Cycling";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)6, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	m_lpSelectionCyclingTab = new SelectionCyclingTab(m_hTabControl);

	//Set current tab...
	int nItemCount = (int)::SendMessage(m_hTabControl, TCM_GETITEMCOUNT, 0, 0);
	if ((m_nCurSel >= 0) && (m_nCurSel < nItemCount)) {
		nIndex = (int)::SendMessage(m_hTabControl, TCM_SETCURSEL, (WPARAM)m_nCurSel, 0);
		if (nIndex == -1) {
			return ErrorHandler();
		}
	}
	else {
		return E_INVALIDARG;
	}

	wm_size_tabcontrol();

	wm_notify_tcn_selchange(hDlg);

	return S_OK;
}

DLGTEMPLATEEX* dlg_DraftingSettings::wm_notify_tcn_selchange_LockDlgRes(LPCTSTR lpszResName)
{
	HRSRC hrsrc = FindResource(NULL, lpszResName, RT_DIALOG);
	if (hrsrc != 0) {
		HGLOBAL hglb = LoadResource(s_hInstance, hrsrc);
		if (hglb != 0) {
			return (DLGTEMPLATEEX*)LockResource(hglb);
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}
void dlg_DraftingSettings::wm_notify_tcn_selchange(HWND hDlg)
{
	m_nCurSel = (int)::SendMessage(m_hTabControl, TCM_GETCURSEL, 0, 0);

	if (m_lpTab != nullptr) {
		if (m_lpTab->getValid() == TRUE) {
			DestroyWindow(m_lpTab->getHWND());
		}
	}

	switch (m_nCurSel) {
		case 0: {
			m_lpTab = m_lpSnapAndGridTab;
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_DRAFTINGSETTINGS_SNAPANDGRID));
			break;
		}
		case 1: {
			m_lpTab = m_lpPolarTrackingTab;
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_DRAFTINGSETTINGS_POLARTRACKING));
			break;
		}
		case 2: {
			m_lpTab = m_lpObjectSnapTab;
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_DRAFTINGSETTINGS_OBJECTSNAP));
			break;
		}
		case 3: {
			m_lpTab = m_lpObjectSnap3DTab;
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_DRAFTINGSETTINGS_3DOBJECTSNAP));
			break;
		}
		case 4: {
			m_lpTab = m_lpDynamicInputTab;
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_DRAFTINGSETTINGS_DYNAMICINPUT));
			break;
		}
		case 5: {
			m_lpTab = m_lpQuickPropertiesTab;
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_DRAFTINGSETTINGS_QUICKPROPERTIES));
			break;
		}
		case 6: {
			m_lpTab = m_lpSelectionCyclingTab;
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_DRAFTINGSETTINGS_SELECTIONCYCLING));
			break;
		}
		default: {
			m_lpTab = m_lpObjectSnapTab;
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_DRAFTINGSETTINGS_OBJECTSNAP));
		}
	}

	m_lpTab->setSystem((SystemWin32*)getSystem());
	HWND hTab = CreateDialogIndirectParam(s_hInstance, (DLGTEMPLATE*)m_lpRes, hDlg, s_nfnChildDialogProc, (LPARAM)m_lpTab);
	if (hTab == NULL) {
		ErrorHandler();
	}

	wm_size_tabcontrol();
}

void dlg_DraftingSettings::wm_size_tabcontrol()
{
	if (m_lpTab == nullptr) return;

	RECT rcTab = { 12, 11,m_cx - 23,m_cy - 60 };

	::SetWindowPos(m_hTabControl,
		NULL,
		rcTab.left,
		rcTab.top,
		rcTab.right,
		rcTab.bottom,
		SWP_NOZORDER
	);

	::OffsetRect(&rcTab, -2, -2);
	::CopyRect(&m_rcDisplay, &rcTab);

	::SetWindowPos(m_lpTab->getHWND(),
		NULL,
		m_rcDisplay.left + 12,
		m_rcDisplay.top + 5 + 21,
		m_rcDisplay.right - m_rcDisplay.left - 12,
		m_rcDisplay.bottom - m_rcDisplay.top - 5 - 21,
		SWP_SHOWWINDOW
	);
}

int dlg_DraftingSettings::wm_command(WPARAM wParam, LPARAM lParam) 
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL nResult = FALSE;
	switch (LOWORD(wParam)) {
		case IDOK: {
			if (m_lpDynamicInputTab->getDynmode(0) != m_lpDynamicInputTab->getDynmode(1)) {
				SendCommandParam(GetParent(m_hWnd), IDD_DRAFTINGSETTINGS, IDC_SETVAR, MAKELPARAM(IDC_DYNMODE, m_lpDynamicInputTab->getDynmode(0)));
			}

			if (m_lpDynamicInputTab->getDynprompt(0) != m_lpDynamicInputTab->getDynprompt(1)) {
				SendCommandParam(GetParent(m_hWnd), IDD_DRAFTINGSETTINGS, IDC_SETVAR, MAKELPARAM(IDC_DYNPROMPT, m_lpDynamicInputTab->getDynprompt(0)));
			}

			if (m_lpDynamicInputTab->getDyninfotips(0) != m_lpDynamicInputTab->getDyninfotips(1)) {
				SendCommandParam(GetParent(m_hWnd), IDD_DRAFTINGSETTINGS, IDC_SETVAR, MAKELPARAM(IDC_DYNINFOTIPS, m_lpDynamicInputTab->getDyninfotips(0)));
			}

			RECT rcWindow = { 0,0,0,0 };
			::GetWindowRect(m_hWnd, &rcWindow);
			setWindowState(WINDOWSTATE_POSITION, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DraftingSettings");
			setWindowState(WINDOWSTATE_SIZE, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DraftingSettings");
			setWindowState(WINDOWSTATE_ACTIVETAB, (LPARAM)&m_nCurSel, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DraftingSettings");

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
		case ID_TOOLS_DRAFTINGSETTINGS_OPTIONS: {
			createOptions();
			nResult = TRUE;
			break;
		}
		default: {
			break;
		}
	}
	return (int)nResult;
}
int dlg_DraftingSettings::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam)
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
int dlg_DraftingSettings::wm_destroy()
{
	DestroyWindow(m_hTabControl);
	return FALSE;
}
int dlg_DraftingSettings::wm_erasebkgnd(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);
	return FALSE;
}
int dlg_DraftingSettings::wm_getminmaxinfo(LPARAM lParam)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

	lpMMI->ptMinTrackSize.x = 490;
	lpMMI->ptMinTrackSize.y = 471;

	return TRUE;
}
int dlg_DraftingSettings::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	setIcon(m_hWnd, MAKEINTRESOURCE(IDI_MYCAD));

	m_hOptions = ::GetDlgItem(m_hWnd, ID_TOOLS_DRAFTINGSETTINGS_OPTIONS);
	m_hOK = ::GetDlgItem(m_hWnd, IDOK);
	m_hCancel = ::GetDlgItem(m_hWnd, IDCANCEL);
	m_hHelp = ::GetDlgItem(m_hWnd, IDHELP);

	getWindowState(WINDOWSTATE_ACTIVETAB, &m_nCurSel, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DraftingSettings");

	wm_initdialog_tabcontrol(m_hWnd);

	RECT rcWindow = { 0,0,0,0 };
	getWindowState(WINDOWSTATE_POSITION, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DraftingSettings");
	getWindowState(WINDOWSTATE_SIZE, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\DraftingSettings");

	::SetWindowPos(m_hWnd,
		HWND_TOP,
		rcWindow.left,
		rcWindow.top,
		rcWindow.right,
		rcWindow.bottom,
		SWP_SHOWWINDOW
	);

	return FALSE;
}
int dlg_DraftingSettings::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	BOOL nResult = FALSE;

#define lpnm (LPNMHDR(lParam))

	switch (lpnm->code) {
		case TCN_SELCHANGE: {//Tabcontrol selection changed
			wm_notify_tcn_selchange(m_hWnd);
			nResult = TRUE;
			break;
		}
		case TCN_SELCHANGING: {//Tabcontrol selection changing

			break;
		}
		default: {
			break;
		}
	}

	return (int)nResult;
}
int dlg_DraftingSettings::wm_paint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);

	::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);

	return TRUE;
}
int dlg_DraftingSettings::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	wm_size_tabcontrol();

	::SetWindowPos(m_hOptions,
		NULL,
		13,
		m_cy - 34,
		75,
		23,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hOK,
		NULL,
		m_cx - 88 - 81 - 81,
		m_cy - 34,
		75,
		23,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hCancel,
		NULL,
		m_cx - 88 - 81,
		m_cy - 34,
		75,
		23,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hHelp,
		NULL,
		m_cx - 88,
		m_cy - 34,
		75,
		23,
		SWP_SHOWWINDOW
	);

	return TRUE;
}
int dlg_DraftingSettings::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::dlg_DraftingSettings::wm_syscommand() SC_CLOSE");
			break;
		}
		default: {
			break;
		}
	}

	return FALSE;
}
