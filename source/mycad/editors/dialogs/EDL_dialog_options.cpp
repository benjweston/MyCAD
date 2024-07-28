#include "SYS_systemwin32.h"

#include "EDL_dialog_options.h"
#include "EDL_dialog_drawingwindowcolours.h"
#include "EDL_resource.h"

#include "resources.h"

#include "reg_util.h"
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

typedef struct tag_dlghdr {
	HWND m_hTabControl;//Tabcontrol handle.
	HWND hDisplay;//Current child dialog box handle.
	RECT m_rcDisplay;//Display rectangle for the tab control.
	DLGTEMPLATEEX* lpRes[10];//Pointer array to child dialog id.
} DLGHDR;

FilesTab::FilesTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
FilesTab::~FilesTab()
{

}



DisplayTab::DisplayTab(HWND htabcontrol)
	: dlg_Tab(htabcontrol),
	m_htxtColourTheme(0),
	m_hbtnDisplayscrollbarsindrawingwindow(0),
	m_hbtnUselargebuttonsforToolbars(0),
	m_hbtnResizeribboniconstostandardsizes(0),
	m_hbtnShowToolTips(0),
	m_hedtNumberofsecondsbeforedisplay(0),
	m_hbtnShowshortcutkeysinToolTips(0),
	m_hbtnShowextendedToolTips(0)
{

}
DisplayTab::~DisplayTab()
{

}

void DisplayTab::setRegistryData()
{
	char chDir[MAX_LOADSTRING]{ 0 };
	char chPath[MAX_LOADSTRING]{ 0 };
	char chCompanyName[MAX_LOADSTRING]{ 0 };
	char chApplicationName[MAX_LOADSTRING]{ 0 };
	char chRelease[MAX_LOADSTRING]{ 0 };

	HINSTANCE hModule = ::LoadLibrary("mycaddata.dll");
	if (hModule != NULL) {
		::LoadString(hModule, ID_COMPANY_NAME, chCompanyName, sizeof(chCompanyName) / sizeof(char));
		::LoadString(hModule, ID_APPLICATION_NAME, chApplicationName, sizeof(chApplicationName) / sizeof(char));
		::LoadString(hModule, ID_APPLICATION_RELEASE, chRelease, sizeof(chRelease) / sizeof(char));
		::FreeLibrary(hModule);
	}

	::_tcscpy_s(chDir, STRING_SIZE(chDir), "Software\\");
	::_tcscat_s(chDir, STRING_SIZE(chDir), chCompanyName);
	::_tcscat_s(chDir, STRING_SIZE(chDir), "\\");
	::_tcscat_s(chDir, STRING_SIZE(chDir), chApplicationName);
	::_tcscat_s(chDir, STRING_SIZE(chDir), "\\");
	::_tcscat_s(chDir, STRING_SIZE(chDir), chRelease);

	::strcpy_s(chPath, chDir);
	::_tcscat_s(chPath, STRING_SIZE(chPath), "\\Profiles\\<<Unnamed Profile>>\\Drawing Window");

	BOOL nScrollbars = (BOOL)::SendMessage(m_hbtnDisplayscrollbarsindrawingwindow, BM_GETCHECK, 0, 0);//Returns BST_CHECKED (0x0001) or BST_UNCHECKED (0x0000)
	//if (nScrollbars == BST_CHECKED) {
	//	nScrollbars = TRUE;
	//}
	//else if (nScrollbars == BST_UNCHECKED) {
	//	nScrollbars = FALSE;
	//}
	SetUserValue(chPath, "Scrollbars", REG_DWORD, &nScrollbars, sizeof(BOOL));
}
void DisplayTab::getRegistryData()
{
	char chDir[MAX_LOADSTRING]{ 0 };
	char chPath[MAX_LOADSTRING]{ 0 };
	char chCompanyName[MAX_LOADSTRING]{ 0 };
	char chApplicationName[MAX_LOADSTRING]{ 0 };
	char chRelease[MAX_LOADSTRING]{ 0 };

	HINSTANCE hModule = ::LoadLibrary("mycaddata.dll");
	if (hModule != NULL) {
		::LoadString(hModule, ID_COMPANY_NAME, chCompanyName, sizeof(chCompanyName) / sizeof(char));
		::LoadString(hModule, ID_APPLICATION_NAME, chApplicationName, sizeof(chApplicationName) / sizeof(char));
		::LoadString(hModule, ID_APPLICATION_RELEASE, chRelease, sizeof(chRelease) / sizeof(char));
		::FreeLibrary(hModule);
	}

	_tcscpy_s(chDir, STRING_SIZE(chDir), "Software\\");
	_tcscat_s(chDir, STRING_SIZE(chDir), chCompanyName);
	_tcscat_s(chDir, STRING_SIZE(chDir), "\\");
	_tcscat_s(chDir, STRING_SIZE(chDir), chApplicationName);
	_tcscat_s(chDir, STRING_SIZE(chDir), "\\");
	_tcscat_s(chDir, STRING_SIZE(chDir), chRelease);

	::strcpy_s(chPath, chDir);
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\Profiles\\<<Unnamed Profile>>\\Drawing Window");

	BOOL nScrollbars = false;
	GetUserValue(chPath, "Scrollbars", REG_DWORD, &nScrollbars, sizeof(BOOL));
	if (nScrollbars == TRUE) {
		::SendMessage(m_hbtnDisplayscrollbarsindrawingwindow, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	}
	else {
		::SendMessage(m_hbtnDisplayscrollbarsindrawingwindow, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	}
}

int DisplayTab::setXMLData() { return static_cast<int>(XML_SUCCESS); }
int DisplayTab::getXMLData()
{
	XMLError eResult = XML_SUCCESS;

	const char* pchSolutionDir = EXPAND(SOLDIR);
	char chSolutionDir[MAX_LOADSTRING]{ 0 };
	char chSolutionDirTemp[MAX_LOADSTRING]{ 0 };
	::strcpy_s(chSolutionDirTemp, pchSolutionDir);//Copy const char* into char[].
	char* substr = chSolutionDirTemp + 1;//Remove the quotes at the beginning of the string.
	substr[::strlen(substr) - 2] = '\0';//Remove the quotes and colon at the end of the string.
	::strcpy_s(chSolutionDir, substr);
	char chData[MAX_LOADSTRING]{ "source\\mycad\\editors\\dialogs\\EDL_options.xml" };
	::strcat_s(chSolutionDir, chData);

	tinyxml2::XMLDocument xmlDoc;
	eResult = xmlDoc.LoadFile(chSolutionDir);
	XMLCheckResult(eResult);

	XMLNode* lpRoot = xmlDoc.FirstChild();
	if (lpRoot == nullptr) return XML_ERROR_FILE_READ_ERROR;

	XMLElement* lpDisplay = lpRoot->FirstChildElement("Display");
	if (lpDisplay == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpWindowElements = lpDisplay->FirstChildElement("WindowElements");
	if (lpWindowElements == nullptr) return XML_ERROR_PARSING_ELEMENT;

	//XMLElement* lpColourtheme = lpWindowElements->FirstChildElement("Colourtheme");
	//if (lpColourtheme) {
	//	bool bMarker = false;
	//	if (xmlutil::getXMLBool(lpColourtheme, bMarker) == XML_SUCCESS) {
	//		log("Log::Application::getDrafting bMarker =  %d", bMarker);
	//		if (bMarker) {
	//			::SendMessage(m_hbtnMarker, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	//		}
	//		else {
	//			::SendMessage(m_hbtnMarker, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	//		}
	//	}
	//}

	//XMLElement* lpDisplayscrollbarsindrawingwindow = lpWindowElements->FirstChildElement("Displayscrollbarsindrawingwindow");
	//if (lpDisplayscrollbarsindrawingwindow) {
	//	bool bDisplayscrollbarsindrawingwindow = false;
	//	if (xmlutil::getXMLBool(lpDisplayscrollbarsindrawingwindow, bDisplayscrollbarsindrawingwindow) == XML_SUCCESS) {
	//		log("Log::Application::getDisplayscrollbarsindrawingwindow bDisplayscrollbarsindrawingwindow =  %d", bDisplayscrollbarsindrawingwindow);
	//		if (bDisplayscrollbarsindrawingwindow) {
	//			::SendMessage(m_hbtnDisplayscrollbarsindrawingwindow, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
	//		}
	//		else {
	//			::SendMessage(m_hbtnDisplayscrollbarsindrawingwindow, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
	//		}
	//	}
	//}

	XMLElement* lpUselargebuttonsforToolbars = lpWindowElements->FirstChildElement("UselargebuttonsforToolbars");
	if (lpUselargebuttonsforToolbars) {
		bool bUselargebuttonsforToolbars = false;
		if (xmlutil::getXMLBool(lpUselargebuttonsforToolbars, bUselargebuttonsforToolbars) == XML_SUCCESS) {
			log("Log::Application::getUselargebuttonsforToolbars bUselargebuttonsforToolbars =  %d", bUselargebuttonsforToolbars);
			if (bUselargebuttonsforToolbars) {
				::SendMessage(m_hbtnUselargebuttonsforToolbars, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnUselargebuttonsforToolbars, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpResizeribboniconstostandardsizes = lpWindowElements->FirstChildElement("Resizeribboniconstostandardsizes");
	if (lpResizeribboniconstostandardsizes) {
		bool bResizeribboniconstostandardsizes = false;
		if (xmlutil::getXMLBool(lpResizeribboniconstostandardsizes, bResizeribboniconstostandardsizes) == XML_SUCCESS) {
			log("Log::Application::getResizeribboniconstostandardsizes bResizeribboniconstostandardsizes =  %d", bResizeribboniconstostandardsizes);
			if (bResizeribboniconstostandardsizes) {
				::SendMessage(m_hbtnResizeribboniconstostandardsizes, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnResizeribboniconstostandardsizes, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpShowTooltips = lpWindowElements->FirstChildElement("ShowTooltips");
	if (lpShowTooltips) {
		bool bShowTooltips = false;
		if (xmlutil::getXMLBool(lpShowTooltips, bShowTooltips) == XML_SUCCESS) {
			log("Log::Application::getShowTooltips bShowTooltips =  %d", bShowTooltips);
			if (bShowTooltips) {
				::SendMessage(m_hbtnShowToolTips, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnShowToolTips, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpNumberofsecondsbeforedisplay = lpWindowElements->FirstChildElement("Numberofsecondsbeforedisplay");
	if (lpNumberofsecondsbeforedisplay) {
		const char* szNumberofsecondsbeforedisplay = xmlutil::getXMLText(lpNumberofsecondsbeforedisplay);
		::SendMessage(m_hedtNumberofsecondsbeforedisplay, WM_SETTEXT, 0, (LPARAM)szNumberofsecondsbeforedisplay);
	}

	XMLElement* lpShowshortcutkeysinTooltips = lpWindowElements->FirstChildElement("ShowshortcutkeysinTooltips");
	if (lpShowshortcutkeysinTooltips) {
		bool bShowshortcutkeysinTooltips = false;
		if (xmlutil::getXMLBool(lpShowshortcutkeysinTooltips, bShowshortcutkeysinTooltips) == XML_SUCCESS) {
			log("Log::Application::getShowshortcutkeysinTooltips bShowshortcutkeysinTooltips =  %d", bShowshortcutkeysinTooltips);
			if (bShowshortcutkeysinTooltips) {
				::SendMessage(m_hbtnShowshortcutkeysinToolTips, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnShowshortcutkeysinToolTips, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpShowextendedTooltips = lpWindowElements->FirstChildElement("ShowextendedTooltips");
	if (lpShowextendedTooltips) {
		bool bShowextendedTooltips = false;
		if (xmlutil::getXMLBool(lpShowextendedTooltips, bShowextendedTooltips) == XML_SUCCESS) {
			log("Log::Application::getShowextendedTooltips bShowextendedTooltips =  %d", bShowextendedTooltips);
			if (bShowextendedTooltips) {
				::SendMessage(m_hbtnShowextendedToolTips, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnShowextendedToolTips, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	return static_cast<int>(eResult);
}

int DisplayTab::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	BOOL nResult = FALSE;

	switch (LOWORD(wParam)) {
		case IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_DISPLAYSCROLLBARS: {
			switch (HIWORD(wParam)) {
			case BN_CLICKED: {

				break;
			}
			default:
				break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_WINDOWELEMENTS_LARGEBUTTONS: {
			switch (HIWORD(wParam)) {
			case BN_CLICKED: {

				break;
			}
			default:
				break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_WINDOWELEMENTS_RESIZERIBBONICONS: {
			switch (HIWORD(wParam)) {
			case BN_CLICKED: {

				break;
			}
			default:
				break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_WINDOWELEMENTS_SHOWTOOLTIPS: {
			switch (HIWORD(wParam)) {
			case BN_CLICKED: {

				break;
			}
			default:
				break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_SHOWSHORTCUTKEYSINTOOLTIPS: {
			switch (HIWORD(wParam)) {
			case BN_CLICKED: {

				break;
			}
			default:
				break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_SHOWEXTENDEDTOOLTIPS: {
			switch (HIWORD(wParam)) {
			case BN_CLICKED: {

				break;
			}
			default:
				break;
			}
			break;
		}
		default: {
			break;
		}
	}

	return (int)nResult;
}
int DisplayTab::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	m_htxtColourTheme = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DISPLAY_LTEXT_COLOURTHEME);

	m_hbtnDisplayscrollbarsindrawingwindow = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_DISPLAYSCROLLBARS);
	m_hbtnUselargebuttonsforToolbars = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_WINDOWELEMENTS_LARGEBUTTONS);
	m_hbtnResizeribboniconstostandardsizes = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_WINDOWELEMENTS_RESIZERIBBONICONS);
	m_hbtnShowToolTips = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_WINDOWELEMENTS_SHOWTOOLTIPS);

	m_hedtNumberofsecondsbeforedisplay = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DISPLAY_EDITTEXT_WINDOWELEMENTS_NUMBEROFSECONDSBEFOREDISPLAY);
	m_hbtnShowshortcutkeysinToolTips = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_SHOWSHORTCUTKEYSINTOOLTIPS);
	m_hbtnShowextendedToolTips = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DISPLAY_CHECKBOX_SHOWEXTENDEDTOOLTIPS);

	getRegistryData();
	getXMLData();

	return 1;
}
int DisplayTab::wm_notify(WPARAM wParam, LPARAM lParam) {
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
int DisplayTab::wm_size(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);



	return 0;
}



OpenAndSaveTab::OpenAndSaveTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
OpenAndSaveTab::~OpenAndSaveTab()
{

}



PlotAndPublishTab::PlotAndPublishTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
PlotAndPublishTab::~PlotAndPublishTab()
{

}



SystemTab::SystemTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
SystemTab::~SystemTab()
{

}



UserPreferencesTab::UserPreferencesTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
UserPreferencesTab::~UserPreferencesTab()
{

}



DraftingTab::DraftingTab(HWND htabcontrol)
	: dlg_Tab(htabcontrol),
	m_hAutoSnapSettings(0),
	m_hAutoTrackSettings(0),
	m_hAlignmentPointAcquisition(0),
	m_hAutoSnapMarkerSize(0),
	m_hApertureSize(0),
	m_hObjectSnapOptions(0),
	m_hbtnMarker(0),
	m_hbtnMagnet(0),
	m_hbtnDisplayAutoSnaptooltip(0),
	m_hbtnDisplayAutoSnapaperturebox(0),
	m_hbtnColours(0),
	m_hbtnDisplaypolartrackingvector(0),
	m_hbtnDisplayfull_screentrackingvector(0),
	m_hbtnDisplayAutoTrackingtooltip(0),
	m_lpDrawingWindowColours(0),
	m_hbtnAutomaticPointAquisitionAutomatic(0),
	m_hbtnAutomaticPointAquisitionShiftToAcquire(0),
	m_hctlAutoSnapMarkerSize(0),
	m_htrbAutoSnapMarkerSize(0),
	m_hctlApertureSize(0),
	m_htrbApertureSize(0),
	m_hbtnIgnorehatchobjects(0),
	m_hbtnIgnoredimensionextensionlines(0),
	m_hbtnIgnorenegativeZobjectsnapsforDynamicUCS(0),
	m_hbtnReplaceZvaluewithcurrentelevation(0),
	m_hbtnDraftingTooltipSettings(0),
	m_hbtnLightsGlyphSettings(0),
	m_hbtnCamerasGlyphSettings(0)
{

}
DraftingTab::~DraftingTab()
{

}

void DraftingTab::createDrawingWindowColours()
{
	m_lpDrawingWindowColours = new dlg_DrawingWindowColours();
	if (m_system->createDialog(m_lpDrawingWindowColours, IDD_DRAWINGWINDOWCOLOURS, m_system->getApplication()->getHWND()) == IDOK) {
		log("Log::dlg_Options::createDrawinngWindowColours() IDOK");
	}

	if (m_lpDrawingWindowColours) {
		delete m_lpDrawingWindowColours;
		m_lpDrawingWindowColours = NULL;
	}
}

int DraftingTab::getXMLData()
{
	XMLError eResult = XML_SUCCESS;

	const char* pchSolutionDir = EXPAND(SOLDIR);
	char chSolutionDir[MAX_LOADSTRING]{ 0 };
	char chSolutionDirTemp[MAX_LOADSTRING]{ 0 };
	::strcpy_s(chSolutionDirTemp, pchSolutionDir);//Copy const char* into char[].
	char* substr = chSolutionDirTemp + 1;//Remove the quotes at the beginning of the string.
	substr[::strlen(substr) - 2] = '\0';//Remove the quotes and colon at the end of the string.
	::strcpy_s(chSolutionDir, substr);
	char chData[MAX_LOADSTRING]{ "appdata\\options.xml" };
	::strcat_s(chSolutionDir, chData);

	tinyxml2::XMLDocument xmlDoc;
	eResult = xmlDoc.LoadFile(chSolutionDir);
	XMLCheckResult(eResult);

	XMLNode* lpRoot = xmlDoc.FirstChild();
	if (lpRoot == nullptr) return XML_ERROR_FILE_READ_ERROR;

	XMLElement* lpDrafting = lpRoot->FirstChildElement("Drafting");
	if (lpDrafting == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpAutoSnapSettings = lpDrafting->FirstChildElement("AutoSnapSettings");
	if (lpAutoSnapSettings == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpMarker = lpAutoSnapSettings->FirstChildElement("Marker");
	if (lpMarker) {
		bool bMarker = false;
		if (xmlutil::getXMLBool(lpMarker, bMarker) == XML_SUCCESS) {
			log("Log::Application::getDrafting bMarker =  %d", bMarker);
			if (bMarker) {
				::SendMessage(m_hbtnMarker, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnMarker, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpMagnet = lpAutoSnapSettings->FirstChildElement("Magnet");
	if (lpMagnet) {
		bool bMagnet = false;
		if (xmlutil::getXMLBool(lpMagnet, bMagnet) == XML_SUCCESS) {
			log("Log::Application::getDrafting bMagnet =  %d", bMagnet);
			if (bMagnet) {
				::SendMessage(m_hbtnMagnet, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnMagnet, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpDisplayAutoSnaptooltip = lpAutoSnapSettings->FirstChildElement("DisplayAutoSnapTooltip");
	if (lpDisplayAutoSnaptooltip) {
		bool bDisplayAutoSnaptooltip = false;
		if (xmlutil::getXMLBool(lpDisplayAutoSnaptooltip, bDisplayAutoSnaptooltip) == XML_SUCCESS) {
			log("Log::Application::getDrafting bDisplayAutoSnaptooltip =  %d", bDisplayAutoSnaptooltip);
			if (bDisplayAutoSnaptooltip) {
				::SendMessage(m_hbtnDisplayAutoSnaptooltip, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnDisplayAutoSnaptooltip, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpDisplayAutoSnapaperturebox = lpAutoSnapSettings->FirstChildElement("DisplayAutoSnapApertureBox");
	if (lpDisplayAutoSnapaperturebox) {
		bool bDisplayAutoSnapaperturebox = false;
		if (xmlutil::getXMLBool(lpDisplayAutoSnapaperturebox, bDisplayAutoSnapaperturebox) == XML_SUCCESS) {
			log("Log::Application::getDrafting bDisplayAutoSnapaperturebox =  %d", bDisplayAutoSnapaperturebox);
			if (bDisplayAutoSnapaperturebox) {
				::SendMessage(m_hbtnDisplayAutoSnapaperturebox, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnDisplayAutoSnapaperturebox, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpAutoTrackSettings = lpDrafting->FirstChildElement("AutoTrackSettings");
	if (lpAutoTrackSettings == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpDisplaypolartrackingvector = lpAutoTrackSettings->FirstChildElement("PolarTrackingVector");
	if (lpDisplaypolartrackingvector) {
		bool bDisplaypolartrackingvector = false;
		if (xmlutil::getXMLBool(lpDisplaypolartrackingvector, bDisplaypolartrackingvector) == XML_SUCCESS) {
			log("Log::Application::getDrafting bDisplaypolartrackingvector =  %d", bDisplaypolartrackingvector);
			if (bDisplaypolartrackingvector) {
				::SendMessage(m_hbtnDisplaypolartrackingvector, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnDisplaypolartrackingvector, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpDisplayfull_screentrackingvector = lpAutoTrackSettings->FirstChildElement("FullScreenVector");
	if (lpDisplayfull_screentrackingvector) {
		bool bDisplayfull_screentrackingvector = false;
		if (xmlutil::getXMLBool(lpDisplayfull_screentrackingvector, bDisplayfull_screentrackingvector) == XML_SUCCESS) {
			log("Log::Application::getDrafting bDisplayfull_screentrackingvector =  %d", bDisplayfull_screentrackingvector);
			if (bDisplayfull_screentrackingvector) {
				::SendMessage(m_hbtnDisplayfull_screentrackingvector, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnDisplayfull_screentrackingvector, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpDisplayAutoTrackingtooltip = lpAutoTrackSettings->FirstChildElement("AutoTrackTooltip");
	if (lpDisplayAutoTrackingtooltip) {
		bool bDisplayAutoTrackingtooltip = false;
		if (xmlutil::getXMLBool(lpDisplayAutoTrackingtooltip, bDisplayAutoTrackingtooltip) == XML_SUCCESS) {
			log("Log::Application::getDrafting bDisplayAutoTrackingtooltip =  %d", bDisplayAutoTrackingtooltip);
			if (bDisplayAutoTrackingtooltip) {
				::SendMessage(m_hbtnDisplayAutoTrackingtooltip, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnDisplayAutoTrackingtooltip, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpAlignmentPointAcquisition = lpDrafting->FirstChildElement("AlignmentPointAcquisition");
	if (lpAlignmentPointAcquisition == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpAutomaticPointAquisitionAutomatic = lpAlignmentPointAcquisition->FirstChildElement("Automatic");
	if (lpAutomaticPointAquisitionAutomatic) {
		bool bAutomaticPointAquisitionAutomatic = false;
		if (xmlutil::getXMLBool(lpAutomaticPointAquisitionAutomatic, bAutomaticPointAquisitionAutomatic) == XML_SUCCESS) {
			log("Log::Application::getDrafting bAutomaticPointAquisitionAutomatic =  %d", bAutomaticPointAquisitionAutomatic);
			if (bAutomaticPointAquisitionAutomatic) {
				::SendMessage(m_hbtnAutomaticPointAquisitionAutomatic, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
				::SendMessage(m_hbtnAutomaticPointAquisitionShiftToAcquire, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnAutomaticPointAquisitionAutomatic, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
				::SendMessage(m_hbtnAutomaticPointAquisitionShiftToAcquire, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
		}
	}

	XMLElement* lpAutoSnapMarkerSize = lpDrafting->FirstChildElement("AutoSnapMarkerSize");
	if (lpAutoSnapMarkerSize) {
		int nAutoSnapMarkerSize = 0;
		if (xmlutil::getXMLInt(lpAutoSnapMarkerSize, nAutoSnapMarkerSize) == XML_SUCCESS) {
			log("Log::Application::getAutoSnapMarkerSize nAutoSnapMarkerSize =  %d", nAutoSnapMarkerSize);
			//if (bMarker) {
			//	::SendMessage(m_hbtnMarker, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			//}
			//else {
			//	::SendMessage(m_hbtnMarker, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			//}
		}
	}

	XMLElement* lpApertureSize = lpDrafting->FirstChildElement("ApertureSize");
	if (lpApertureSize) {
		int nApertureSize = 0;
		if (xmlutil::getXMLInt(lpApertureSize, nApertureSize) == XML_SUCCESS) {
			log("Log::Application::getApertureSize nApertureSize =  %d", nApertureSize);
			//if (bMarker) {
			//	::SendMessage(m_hbtnMarker, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			//}
			//else {
			//	::SendMessage(m_hbtnMarker, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			//}
		}
	}

	XMLElement* lpObjectSnapOptions = lpDrafting->FirstChildElement("ObjectSnapOptions");
	if (lpObjectSnapOptions == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpIgnorehatchobjects = lpObjectSnapOptions->FirstChildElement("Ignorehatchobjects");
	if (lpIgnorehatchobjects) {
		bool bIgnorehatchobjects = false;
		if (xmlutil::getXMLBool(lpIgnorehatchobjects, bIgnorehatchobjects) == XML_SUCCESS) {
			log("Log::Application::getIgnorehatchobjects bIgnorehatchobjects =  %d", bIgnorehatchobjects);
			if (bIgnorehatchobjects) {
				::SendMessage(m_hbtnIgnorehatchobjects, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnIgnorehatchobjects, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpIgnoredimensionextensionlines = lpObjectSnapOptions->FirstChildElement("Ignoredimensionextensionlines");
	if (lpIgnoredimensionextensionlines) {
		bool bIgnoredimensionextensionlines = false;
		if (xmlutil::getXMLBool(lpIgnoredimensionextensionlines, bIgnoredimensionextensionlines) == XML_SUCCESS) {
			log("Log::Application::getIgnoredimensionextensionlines bIgnoredimensionextensionlines =  %d", bIgnoredimensionextensionlines);
			if (bIgnoredimensionextensionlines) {
				::SendMessage(m_hbtnIgnoredimensionextensionlines, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnIgnoredimensionextensionlines, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpIgnorenegativeZobjectsnapsforDynamicUCS = lpObjectSnapOptions->FirstChildElement("IgnorenegativeZobjectsnapsforDynamicUCS");
	if (lpIgnorenegativeZobjectsnapsforDynamicUCS) {
		bool bIgnorenegativeZobjectsnapsforDynamicUCS = false;
		if (xmlutil::getXMLBool(lpIgnorenegativeZobjectsnapsforDynamicUCS, bIgnorenegativeZobjectsnapsforDynamicUCS) == XML_SUCCESS) {
			log("Log::Application::getIgnorenegativeZobjectsnapsforDynamicUCS bIgnorenegativeZobjectsnapsforDynamicUCS =  %d", bIgnorenegativeZobjectsnapsforDynamicUCS);
			if (bIgnorenegativeZobjectsnapsforDynamicUCS) {
				::SendMessage(m_hbtnIgnorenegativeZobjectsnapsforDynamicUCS, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnIgnorenegativeZobjectsnapsforDynamicUCS, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	XMLElement* lpReplaceZvaluewithcurrentelevation = lpObjectSnapOptions->FirstChildElement("ReplaceZvaluewithcurrentelevation");
	if (lpReplaceZvaluewithcurrentelevation) {
		bool bReplaceZvaluewithcurrentelevation = false;
		if (xmlutil::getXMLBool(lpReplaceZvaluewithcurrentelevation, bReplaceZvaluewithcurrentelevation) == XML_SUCCESS) {
			log("Log::Application::getReplaceZvaluewithcurrentelevation bReplaceZvaluewithcurrentelevation =  %d", bReplaceZvaluewithcurrentelevation);
			if (bReplaceZvaluewithcurrentelevation) {
				::SendMessage(m_hbtnReplaceZvaluewithcurrentelevation, BM_SETCHECK, (WPARAM)(BST_CHECKED), 0);
			}
			else {
				::SendMessage(m_hbtnReplaceZvaluewithcurrentelevation, BM_SETCHECK, (WPARAM)(BST_UNCHECKED), 0);
			}
		}
	}

	return static_cast<int>(eResult);
}

int DraftingTab::wm_command(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	BOOL nResult = FALSE;

	switch (LOWORD(wParam)) {
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOSNAP_SETTINGS_MARKER: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOSNAP_SETTINGS_MAGNET: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOSNAP_SETTINGS_DISPLAYAUTOSNAPTOOLTIP: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOSNAP_SETTINGS_DISPLAYAUTOSNAPAPERTUREBOX: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_BUTTON_AUTOSNAP_SETTINGS_COLOURS: {
			createDrawingWindowColours();
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOTRACK_SETTINGS_DISPLAYPOLARTRACKINGVECTOR: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOTRACK_SETTINGS_DISPLAYFULLSCREENTRACKINGVECTOR: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOTRACK_SETTINGS_DISPLAYAUTOTRACKTOOLTIP: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_RADIOBUTTON_ALIGNMENT_POINTACQUISITION_AUTOMATIC: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_RADIOBUTTON_ALIGNMENT_POINTACQUISITION_SHIFTTOACQUIRE: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_OBJECTSNAPOPTIONS_HATCH: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_OBJECTSNAPOPTIONS_EXTENSION: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_OBJECTSNAPOPTIONS_ZOBJECT: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_OBJECTSNAPOPTIONS_ZVALUE: {
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {

					break;
				}
				default:
					break;
			}
			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_BUTTON_OBJECTSNAPOPTIONS_DRAFTINGTOOLTIPSETTINGS: {

			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_BUTTON_OBJECTSNAPOPTIONS_LIGHTSGLYPHSETTINGS: {

			break;
		}
		case IDC_TOOLS_OPTIONS_DRAFTING_BUTTON_OBJECTSNAPOPTIONS_CAMERASGLYPHSETTINGS: {

			break;
		}
		default:
			break;
	}

	return (int)nResult;
}
int DraftingTab::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	m_hAutoSnapSettings = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_GROUPBOX_AUTOSNAP_SETTINGS);
	m_hAutoTrackSettings = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_GROUPBOX_AUTOTRACK_SETTINGS);
	m_hAlignmentPointAcquisition = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_GROUPBOX_ALIGNMENT_POINTACQUISITION);
	m_hAutoSnapMarkerSize = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_GROUPBOX_AUTOSNAP_MARKER_SIZE);
	m_hApertureSize = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_GROUPBOX_APERTURE_SIZE);
	m_hObjectSnapOptions = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_GROUPBOX_OBJECTSNAPOPTIONS);

	m_hbtnMarker = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOSNAP_SETTINGS_MARKER);
	m_hbtnMagnet = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOSNAP_SETTINGS_MAGNET);
	m_hbtnDisplayAutoSnaptooltip = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOSNAP_SETTINGS_DISPLAYAUTOSNAPTOOLTIP);
	m_hbtnDisplayAutoSnapaperturebox = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOSNAP_SETTINGS_DISPLAYAUTOSNAPAPERTUREBOX);

	m_hbtnColours = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_BUTTON_AUTOSNAP_SETTINGS_COLOURS);

	m_hbtnDisplaypolartrackingvector = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOTRACK_SETTINGS_DISPLAYPOLARTRACKINGVECTOR);
	m_hbtnDisplayfull_screentrackingvector = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOTRACK_SETTINGS_DISPLAYFULLSCREENTRACKINGVECTOR);
	m_hbtnDisplayAutoTrackingtooltip = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_AUTOTRACK_SETTINGS_DISPLAYAUTOTRACKTOOLTIP);

	m_hbtnAutomaticPointAquisitionAutomatic = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_RADIOBUTTON_ALIGNMENT_POINTACQUISITION_AUTOMATIC);
	m_hbtnAutomaticPointAquisitionShiftToAcquire = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_RADIOBUTTON_ALIGNMENT_POINTACQUISITION_SHIFTTOACQUIRE);

	m_hctlAutoSnapMarkerSize = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CONTROL_AUTOSNAP_MARKER_SIZE);

	m_htrbAutoSnapMarkerSize = ::CreateWindow(
		TRACKBAR_CLASS,
		NULL,
		WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_BOTH | TBS_NOTICKS | TBS_DOWNISLEFT/* | TBS_TOOLTIPS*/,
		75,
		227,
		202,
		25,
		m_hWnd,
		(HMENU)IDC_TOOLS_OPTIONS_DRAFTING_TRACKBAR_AUTOSNAP_MARKER_SIZE,
		s_hInstance,
		(LPVOID)0
	);

	if (m_htrbAutoSnapMarkerSize == NULL) {
		ErrorHandler();
		return FALSE;
	}

#pragma warning( push )
#pragma warning( disable : 6387)
	::SendMessage(m_htrbAutoSnapMarkerSize, TBM_SETRANGE, (WPARAM)FALSE, (LPARAM)MAKELONG(5, 95));
	::SendMessage(m_htrbAutoSnapMarkerSize, PBM_SETSTEP, (WPARAM)1, 0);
	::SendMessage(m_htrbAutoSnapMarkerSize, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)50);
#pragma warning( pop )

	//setFont(m_htrbAutoSnapMarkerSize);

	m_hctlApertureSize = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CONTROL_APERTURE_SIZE);
	m_htrbApertureSize = ::CreateWindow(
		TRACKBAR_CLASS,
		NULL,
		WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_BOTH | TBS_NOTICKS | TBS_DOWNISLEFT/* | TBS_TOOLTIPS*/,
		408,
		227,
		200,
		25,
		m_hWnd,
		(HMENU)IDC_TOOLS_OPTIONS_DRAFTING_TRACKKBAR_APERTURE_SIZE,
		s_hInstance,
		(LPVOID)0
	);

	if (m_htrbApertureSize == NULL) {
		ErrorHandler();
		return FALSE;
	}

#pragma warning( push )
#pragma warning( disable : 6387)
	::SendMessage(m_htrbApertureSize, TBM_SETRANGE, (WPARAM)FALSE, (LPARAM)MAKELONG(5, 95));
	::SendMessage(m_htrbApertureSize, PBM_SETSTEP, (WPARAM)1, 0);
	::SendMessage(m_htrbApertureSize, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)50);
#pragma warning( pop )

	//setFont(m_htrbApertureSize);

	m_hbtnIgnorehatchobjects = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_OBJECTSNAPOPTIONS_HATCH);
	m_hbtnIgnoredimensionextensionlines = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_OBJECTSNAPOPTIONS_EXTENSION);
	m_hbtnIgnorenegativeZobjectsnapsforDynamicUCS = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_OBJECTSNAPOPTIONS_ZOBJECT);
	m_hbtnReplaceZvaluewithcurrentelevation = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_CHECKBOX_OBJECTSNAPOPTIONS_ZVALUE);

	m_hbtnDraftingTooltipSettings = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_BUTTON_OBJECTSNAPOPTIONS_DRAFTINGTOOLTIPSETTINGS);
	m_hbtnLightsGlyphSettings = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_BUTTON_OBJECTSNAPOPTIONS_LIGHTSGLYPHSETTINGS);
	m_hbtnCamerasGlyphSettings = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_DRAFTING_BUTTON_OBJECTSNAPOPTIONS_CAMERASGLYPHSETTINGS);

	getXMLData();

	return FALSE;
}
int DraftingTab::wm_notify(WPARAM wParam, LPARAM lParam) {
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
int DraftingTab::wm_size(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	::SetWindowPos(m_hAutoSnapSettings,
		NULL,
		3,
		8,
		(m_cx - 3 - 12 - 6) / 2,
		166,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hAutoTrackSettings,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 12,
		8,
		(m_cx - 3 - 12 - 0) / 2 - 3,
		88,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hAlignmentPointAcquisition,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 12,
		106,
		(m_cx - 3 - 12 - 0) / 2 - 3,
		68,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hAutoSnapMarkerSize,
		NULL,
		3,
		184,
		(m_cx - 3 - 12 - 6) / 2,
		101,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hApertureSize,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 12,
		184,
		(m_cx - 3 - 12 - 0) / 2 - 3,
		101,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hObjectSnapOptions,
		NULL,
		3,
		293,
		(m_cx - 3 - 12 - 6) / 2,
		119,
		SWP_SHOWWINDOW
	);


	::SetWindowPos(m_hbtnDisplaypolartrackingvector,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 23,
		28,
		100,
		14,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnDisplayfull_screentrackingvector,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 23,
		48,
		100,
		14,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnDisplayAutoTrackingtooltip,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 23,
		68,
		100,
		14,
		SWP_SHOWWINDOW
	);


	::SetWindowPos(m_hbtnAutomaticPointAquisitionAutomatic,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 24,
		127,
		100,
		14,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnAutomaticPointAquisitionShiftToAcquire,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 24,
		149,
		100,
		14,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hctlApertureSize,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 25,
		210,
		56,
		58,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_htrbApertureSize,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 89,
		227,
		200,
		25,
		SWP_SHOWWINDOW
	);
	
	::SetWindowPos(m_hbtnDraftingTooltipSettings,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 12,
		298,
		289,
		23,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnLightsGlyphSettings,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 12,
		333,
		289,
		23,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hbtnCamerasGlyphSettings,
		NULL,
		3 + (m_cx - 3 - 12 - 6) / 2 + 12,
		369,
		289,
		23,
		SWP_SHOWWINDOW
	);

	return 0;
}



ThreeDModelingTab::ThreeDModelingTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
ThreeDModelingTab::~ThreeDModelingTab()
{

}



SelectionTab::SelectionTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
SelectionTab::~SelectionTab()
{

}



ProfilesTab::ProfilesTab(HWND htabcontrol) : dlg_Tab(htabcontrol)
{

}
ProfilesTab::~ProfilesTab()
{

}



dlg_Options::dlg_Options(COMMANDINFO* command, DynamicInputWindow* commandwindows[], int tabindex, CommandMode mode)
	: CommandDialog(command, commandwindows, mode),
	m_hTabControl(0),
	m_lpTab(0),
	m_rcDisplay({ 0,0,0,0 }),
	m_lpRes(0),
	m_nCurSel{ tabindex,tabindex },
	m_hCurrentProfile(0),
	m_hCurrentDrawing(0),
	m_hOK(0),
	m_hApply(0),
	m_hCancel(0),
	m_hHelp(0)
{

}
dlg_Options::dlg_Options(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, int tabindex, CommandMode mode)
	: CommandDialog(command, commandwindows, text, mode),
	m_hTabControl(0),
	m_lpTab(0),
	m_rcDisplay({ 0,0,0,0 }),
	m_lpRes(0),
	m_nCurSel{ tabindex,tabindex },
	m_hCurrentProfile(0),
	m_hCurrentDrawing(0),
	m_hOK(0),
	m_hApply(0),
	m_hCancel(0),
	m_hHelp(0)
{

}
dlg_Options::~dlg_Options()
{

}

INT_PTR CALLBACK dlg_Options::s_nfnChildDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			log("Log::dlg_Options::s_nfnChildDialogProc: window event before creation.");
			return 0;
		}
	}
	else {
		// Events without valid hWnd
		log("Log::dlg_Options::s_nfnChildDialogProc: event without window.");
	}

	return lResult;
}

HRESULT dlg_Options::wm_initdialog_tabcontrol(HWND hDlg)
{
	m_hTabControl = ::CreateWindow(WC_TABCONTROL,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		11,
		5 + 23,
		m_cx - 38,
		m_cy - 82 - 23,
		hDlg,
		(HMENU)IDC_TOOLS_OPTIONS_TABCONTROL,
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

	tie.pszText = (LPSTR)"Files";
	int nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, 0, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"Display";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)1, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"Open and Save";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)2, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"Plot and Publish";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)3, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"System";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)4, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"User Preferences";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)5, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"Drafting";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)6, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"3D Modeling";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)7, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"Selection";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)8, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	tie.pszText = (LPSTR)"Profiles";
	nIndex = (int)::SendMessage(m_hTabControl, TCM_INSERTITEM, (WPARAM)9, (LPARAM)&tie);
	if (nIndex == -1) {
		return ErrorHandler();
	}

	//Set current tab...
	int nItemCount = (int)::SendMessage(m_hTabControl, TCM_GETITEMCOUNT, 0, 0);
	if ((m_nCurSel[1] >= 0) && (m_nCurSel[1] < nItemCount)) {
		nIndex = (int)::SendMessage(m_hTabControl, TCM_SETCURSEL, (WPARAM)m_nCurSel[1], 0);
		if (nIndex == -1) {
			return ErrorHandler();
		}
	}
	else {
		return E_INVALIDARG;
	}

	wm_notify_tcn_selchange(hDlg);

	wm_size_tabcontrol();

	return S_OK;
}

DLGTEMPLATEEX* dlg_Options::wm_notify_tcn_selchange_LockDlgRes(LPCTSTR lpszResName)
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
void dlg_Options::wm_notify_tcn_selchange(HWND hDlg)
{
	m_nCurSel[1] = (int)::SendMessage(m_hTabControl, TCM_GETCURSEL, 0, 0);

	if (m_lpTab != nullptr) {
		if (m_lpTab->getValid() == TRUE) {
			::DestroyWindow(m_lpTab->getHWND());
		}
	}

	switch (m_nCurSel[1]) {
		case 0: {
			m_lpTab = new FilesTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_FILES));
			break;
		}
		case 1: {
			m_lpTab = new DisplayTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_DISPLAY));
			break;
		}
		case 2: {
			m_lpTab = new OpenAndSaveTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_OPENANDSAVE));
			break;
		}
		case 3: {
			m_lpTab = new PlotAndPublishTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_PLOTANDPUBLISH));
			break;
		}
		case 4: {
			m_lpTab = new SystemTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_SYSTEM));
			break;
		}
		case 5: {
			m_lpTab = new UserPreferencesTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_USERPREFERENCES));
			break;
		}
		case 6: {
			m_lpTab = new DraftingTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_DRAFTING));
			break;
		}
		case 7: {
			m_lpTab = new ThreeDModelingTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_3DMODELING));
			break;
		}
		case 8: {
			m_lpTab = new SelectionTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_SELECTION));
			break;
		}
		case 9: {
			m_lpTab = new ProfilesTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_PROFILES));
			break;
		}
		default: {
			m_lpTab = new DraftingTab(m_hTabControl);
			m_lpRes = wm_notify_tcn_selchange_LockDlgRes(MAKEINTRESOURCE(IDD_OPTIONS_DRAFTING));
		}
	}

	m_lpTab->setSystem((SystemWin32*)getSystem());
	HWND hTab = CreateDialogIndirectParam(s_hInstance, (DLGTEMPLATE*)m_lpRes, hDlg, s_nfnChildDialogProc, (LPARAM)m_lpTab);
	if (hTab == NULL) {
		ErrorHandler();
	}

	wm_size_tabcontrol();
}

void dlg_Options::wm_size_tabcontrol()
{
	if (m_lpTab == nullptr) return;

	RECT rcTab = { 12, 5 + 21,m_cx - 23,m_cy - 43 - 21 };

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
void dlg_Options::wm_size()
{
	::SetWindowPos(m_hCurrentDrawing, NULL, m_cx / 2 + 52, 7, 76, 13, SWP_SHOWWINDOW);

	wm_size_tabcontrol();

	::SetWindowPos(m_hOK, NULL, m_cx - 88 - 81 - 81 - 81, m_cy - 30, 75, 23, SWP_SHOWWINDOW);
	::SetWindowPos(m_hCancel, NULL, m_cx - 88 - 81 - 81, m_cy - 30, 75, 23, SWP_SHOWWINDOW);
	::SetWindowPos(m_hApply, NULL, m_cx - 88 - 81, m_cy - 30, 75, 23, SWP_SHOWWINDOW);
	::SetWindowPos(m_hHelp, NULL, m_cx - 88, m_cy - 30, 75, 23, SWP_SHOWWINDOW);
}

int dlg_Options::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	BOOL nResult = FALSE;

	switch (LOWORD(wParam)) {
		case IDOK: {
			RECT rcWindow = { 0,0,0,0 };
			GetWindowRect(m_hWnd, &rcWindow);
			setWindowState(WINDOWSTATE_POSITION, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\Options");
			setWindowState(WINDOWSTATE_SIZE, (LPARAM)&rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\Options");

			if (m_nCurSel[0] == -1) {
				setWindowState(WINDOWSTATE_ACTIVETAB, (LPARAM)&m_nCurSel[1], "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\Options");
			}

			nResult = ::EndDialog(m_hWnd, wParam);

			break;
		}
		case IDCANCEL: {
			nResult = ::EndDialog(m_hWnd, wParam);
			break;
		}
		case IDC_TOOLS_OPTIONS_APPLY: {
			m_lpTab->setRegistryData();
			m_lpTab->setXMLData();

			::SendMessage(m_system->getApplication()->getHWND(), WM_COMMAND, MAKEWPARAM(IDC_TOOLS_OPTIONS_APPLY, 0), 0);

			nResult = TRUE;

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
int dlg_Options::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam)
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
int dlg_Options::wm_destroy()
{
	DestroyWindow(m_hTabControl);
	return FALSE;
}
int dlg_Options::wm_erasebkgnd(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);
	return FALSE;
}
int dlg_Options::wm_getminmaxinfo(LPARAM lParam)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

	lpMMI->ptMinTrackSize.x = 720;
	lpMMI->ptMinTrackSize.y = 583;

	return TRUE;
}
int dlg_Options::wm_initdialog(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	setIcon(m_hWnd, MAKEINTRESOURCE(IDI_MYCAD));

	m_hCurrentProfile = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_LTEXT_CURRENTPROFILE);
	m_hCurrentDrawing = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_LTEXT_CURRENTDRAWING);

	m_hOK = ::GetDlgItem(m_hWnd, IDOK);
	m_hCancel = ::GetDlgItem(m_hWnd, IDCANCEL);
	m_hApply = ::GetDlgItem(m_hWnd, IDC_TOOLS_OPTIONS_APPLY);
	m_hHelp = ::GetDlgItem(m_hWnd, IDHELP);

	if (m_nCurSel[0] == -1) {
		getWindowState(WINDOWSTATE_ACTIVETAB, &m_nCurSel[1], "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\Options");
	}

	wm_initdialog_tabcontrol(m_hWnd);

	RECT rcWindow = { 0,0,0,0 };
	getWindowState(WINDOWSTATE_POSITION, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\Options");
	getWindowState(WINDOWSTATE_SIZE, &rcWindow, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\Options");

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
int dlg_Options::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	BOOL nResult = FALSE;
	//m_lpFiles.m_notify();

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
int dlg_Options::wm_paint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);

	::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);

	return TRUE;
}
int dlg_Options::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	wm_size();

	return TRUE;
}
int dlg_Options::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::dlg_Options::wm_syscommand() SC_CLOSE");
			break;
		}
		default: {
			break;
		}
	}

	return FALSE;
}
