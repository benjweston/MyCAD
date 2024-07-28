#include "SYS_systemwin32.h"

#include "wm_tooltip.h"
#include "wm_statusbar.h"

#include "CMD_commands.h"
#include "CMD_commandmanager.h"

#include "DOC_document.h"
#include "DOC_glrenderwindow.h"
#include "DOC_resource.h"

#include "MKE_application.h"//#include "MKE_fakemenu.h"
#include "MKE_commonfiledialog.h"
#include "MKE_resource.h"

#include "EDL_dialog_drawingunits.h"
#include "EDL_dialog_draftingsettings.h"
#include "EDL_dialog_drawingwindowcolours.h"
#include "EDL_dialog_options.h"
#include "EDL_dialog_ucs.h"
#include "EDL_dialog_ucsicon.h"
#include "EDL_dialog_viewmanager.h"
#include "EDL_dialog_viewports.h"
#include "EDL_resource.h"

#include "EPL_palette_layerpropertiesmanager.h"
#include "EPL_resource.h"

#include "EWD_dynamicinput.h"
#include "EWD_commandline.h"
#include "EWD_textwindow.h"
#include "EWD_resource.h"

#include "resources.h"

#include "char_util.h"
#include "reg_util.h"
#include "xml_util.h"

#include "fault.h"
#include "log.h"

#include <shlwapi.h>//Required for PathStripPath().
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.
#include <windowsx.h>

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) });

using namespace mycad;

//Private static variables
BOOL Application::s_nNewDocumentOnStartup{ TRUE };
int Application::s_nTabconctrol_Documents_Height{ 25 };

//Public variables/functions.
Application::Application(WindowManager* windowmanager, DocumentManager* documentmanager)
	: WindowWin32(),
	m_lpWindowManager(windowmanager),
	m_lpDocumentManager(documentmanager),
	m_szFilter("DWG file format(*.dwg)|*.dwg|*.dxf|*.obj"),//"DWG file format(*.dwg)|*.dwg|*.dxf|*.obj"
														   //"All\0*.*\0Text\0*.TXT\0"
														   //"DXF file format(*.dxf) |*.dxf|All Files (*.*)|*.*||";
														   //"DWG file format(*.dwg) |*.dwg|All Files (*.*)|*.*||";
														   //"Wavefront(*.obj) |*.obj|All Files (*.*)|*.*||";
	m_lpStatusbar(nullptr),
	m_lpTextWindow(nullptr),
	m_lpCommandLine(nullptr),
	m_lpDynamicInput(nullptr),
	m_hToolbar_File(0),
	m_hToolbar_Draw(0),
	m_hRebar_Standard(0),
	m_hTabcontrol_Documents(0),
	m_hTabcontrol_Documents_Tooltip(0),
	m_ptDocument({ 0,0 }),
	m_szDocument({ 0,0 }),
	m_nMinFrameDuration(33),
	m_nDocumentCounter(1),
	m_bThreadSuspended(false)
{
	log("Log::Application::Application()");

	spiGetMouseHoverSize(5, 5);

	CommandWindow::setMode(CommandMode::OUTPUT);

	GLRenderWindow::setSystemColour(GLRenderWindow::SystemInterfaceElement::ActiveBorder, RGB(76, 76, 76));
	GLRenderWindow::setSplitterThickness(5);

	xmlGetOptions();
	xmlGetUCSIconProperties();
	xmlGetCommands();

	m_lpDynamicInput = new DynamicInput();//Declare m_lpDynamicInput...
	m_lpTextWindow = new TextWindow(m_lpDynamicInput);//...then declare m_lpTextWindow, with m_lpDynamicInput as the argument...
	m_lpCommandLine = new CommandLine(m_lpDynamicInput);//...and m_lpCommandLine, with m_lpDynamicInput as the argument...

	m_lpTextWindow->setCommandInput((CommandLineWindow*)m_lpCommandLine);//...then send the pointer to m_lpCommandLine to m_lpTextWindow...
	m_lpCommandLine->setCommandInput((CommandLineWindow*)m_lpTextWindow);//...and the pointer to m_lpTextWindow to m_lpCommandLine.

	m_lpCommandWindows[0] = (DynamicInputWindow*)m_lpTextWindow;
	m_lpCommandWindows[1] = (DynamicInputWindow*)m_lpCommandLine;
	m_lpCommandWindows[2] = (DynamicInputWindow*)m_lpDynamicInput;
}
Application::~Application()
{
	log("Log::Application::~Application()");

	delete m_lpStatusbar;
}

void Application::getApplicationState(RECT& position, int& visibility)//Retrieves the application window state from the registry.
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
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\FixedProfile\\Application Window");

	GetUserValue(chPath, "Position", REG_BINARY, &position, sizeof(RECT));
	GetUserValue(chPath, "Show", REG_DWORD, &visibility, sizeof(int));

	::strcpy_s(chPath, chDir);
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\Profiles\\<<Unnamed Profile>>\\Drawing Window");

	BOOL nScrollbars = false;
	GetUserValue(chPath, "Scrollbars", REG_DWORD, &nScrollbars, sizeof(BOOL));
	if (nScrollbars == TRUE) {
		GLRenderWindow::s_bScrollbars = true;
	}
}

int Application::wm_activate(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if (m_nActive > WA_CLICKACTIVE) {
		if (s_nNewDocumentOnStartup == TRUE) {
			m_nActive = 0;
			wm_command_File_New();//Let application receive WM_CREATE, WM_ACTIVATE & WM_SIZE messages before creating initial new document.
		}
	}

	m_nActive = LOWORD(wParam);

	switch (m_nActive) {
		case WA_INACTIVE: {
			log("Log::Application::wm_activate() WA_INACTIVE");
			break;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
		}
		case WA_ACTIVE:
		case WA_CLICKACTIVE: {
			log("Log::Application::wm_activate() WA_ACTIVE");
			wm_exitsizemove_statusbar_tooltips();
			break;
		}
		default:{
			break;
		}
	}

	return 0;
}
int Application::wm_char(WPARAM wParam, LPARAM lParam)
{
	return m_lpDocumentManager->sendMessage(WM_CHAR, wParam, lParam);
}
int Application::wm_close()
{
	setApplicationState();

	setWindowState(WINDOWSTATE_SHOW, TextWindow::s_nVisible, "\\FixedProfile\\Command Line Windows", "TextWindow.Show");

	if (m_lpTextWindow->getValid() == TRUE) {
		BOOL bVisible = ::IsWindowVisible(m_lpTextWindow->getHWND());
		if (bVisible == TRUE) {
			LRESULT lResult = (LRESULT)::SendMessage(m_lpTextWindow->getHWND(), WM_CLOSE, 0, 0);
			if (lResult == FALSE) {
				log("Log::Application::wm_close() WM_CLOSE = 0");
			}
		}
	}

	setWindowState(WINDOWSTATE_SHOW, CommandLine::s_nVisible, "\\FixedProfile\\Command Line Windows", "CommandLine.Show");

	if (m_lpCommandLine->getValid() == TRUE) {
		BOOL bVisible = ::IsWindowVisible(m_lpCommandLine->getHWND());
		if (bVisible == TRUE) {
			LRESULT lResult = (LRESULT)::SendMessage(m_lpCommandLine->getHWND(), WM_CLOSE, 0, 0);
			if (lResult == FALSE) {
				log("Log::Application::wm_close() WM_CLOSE = 0");
			}
		}
	}

	if (m_lpDynamicInput->getValid() == TRUE) {
		BOOL bVisible = ::IsWindowVisible(m_lpDynamicInput->getHWND());
		if (bVisible == TRUE) {
			LRESULT lResult = (LRESULT)::SendMessage(m_lpDynamicInput->getHWND(), WM_CLOSE, 0, 0);
			if (lResult == FALSE) {
				log("Log::Application::wm_close() WM_CLOSE = 0");
			}
		}
	}

	setWindowState(WINDOWSTATE_SHOW, plt_LayerPropertiesManager::s_nVisible, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\LayerManager", "Show");

	char szLayerPropertiesManagerClassName[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_LAYER_PROPERTIES_MANAGER_CLASS, szLayerPropertiesManagerClassName, sizeof(szLayerPropertiesManagerClassName) / sizeof(char));
	plt_LayerPropertiesManager* lpLayerPropertiesManager = (plt_LayerPropertiesManager*)m_system->getWindowManager().getWindow(szLayerPropertiesManagerClassName);
	if (lpLayerPropertiesManager != nullptr) {
		if (lpLayerPropertiesManager->getValid() == TRUE) {
			BOOL bVisible = ::IsWindowVisible(lpLayerPropertiesManager->getHWND());
			if (bVisible == TRUE) {
				LRESULT lResult = (LRESULT)::SendMessage(lpLayerPropertiesManager->getHWND(), WM_CLOSE, 0, 0);
				if (lResult == FALSE) {
					log("Log::Application::wm_close() WM_CLOSE = 0");
				}
			}
		}
	}

	//MSG mMsg;
	//mMsg.message = WM_CLOSE;
	//EnumWindows(s_bfnEnumWindowsProc, (LPARAM)&mMsg);

	//char szClassName[MAX_LOADSTRING]{ 0 };
	//::LoadString(s_hInstance, ID_DOCUMENT_CLASS, szClassName, sizeof(szClassName) / sizeof(char));

	//mMsg.message = WM_CLOSE;
	//mMsg.lParam = (LPARAM)szClassName;
	//EnumChildWindows(m_hWnd, s_bfnEnumChildWindowsProc, (LPARAM)&mMsg);

	::DestroyWindow(m_hWnd);

	return 0;
}
int Application::wm_command(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID = LOWORD(wParam);
	int nSource = HIWORD(wParam);

	switch (nSource) {
		case BN_CLICKED: {
			//Button clicks are passed to WM_COMMAND first then passed on to WM_NOTIFY.
			//We use distinct menu identifiers and control identifiers and use WM_COMMAND to deal with menu commands while WM_NOTIFY deals with toolbar commands. 
			log("Log::Application::wm_command() Message generated by BN_CLICKED (button or menu)");
			break;
		}
		case 1: {
			log("Log::Application::wm_command() Message generated by accelerator");
			break;
		}
		default: {
			log("Log::Application::wm_command() default");
		}
	}

	Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
	HWND hWnd = NULL;
	if (lpActiveDocument != nullptr) {
		hWnd = lpActiveDocument->getHWND();
	}

	switch (nID) {
/***************************************************************FILE***************************************************************/
		case ID_FILE_NEW: {
			log("Log::Application::wm_command_File() ID_FILE_NEW");
			wm_command_File_New();
			break;
		}
		case ID_FILE_NEWSHEETSET: {
			log("Log::Application::wm_command_File() ID_FILE_NEWSHEETSET");

			break;
		}
		case ID_FILE_OPEN: {
			log("Log::Application::wm_command_File() ID_FILE_OPEN");
			wm_command_File_Open();
			break;
		}
		case ID_FILE_OPENSHEETSET: {
			log("Log::Application::wm_command_File() ID_FILE_OPENSHEETSET");

			break;
		}
		case ID_FILE_LOADMARKUPSET: {
			log("Log::Application::wm_command_File() ID_FILE_LOADMARKUPSET");

			break;
		}
		case ID_FILE_CLOSE: {
			log("Log::Application::wm_command_File() ID_FILE_CLOSE");
			wm_command_File_Close();
			break;
		}
		case ID_FILE_PARTIALLOAD: {
			log("Log::Application::wm_command_File() ID_FILE_PARTIALLOAD");

			break;
		}
		case ID_FILE_IMPORT: {
			log("Log::Application::wm_command_File() ID_FILE_IMPORT");

			break;
		}
		case ID_FILE_ATTACH: {
			log("Log::Application::wm_command_File() ID_FILE_ATTACH");

			break;
		}
		case ID_FILE_SAVE: {
			log("Log::Application::wm_command_File() ID_FILE_SAVE");

			break;
		}
		case ID_FILE_SAVEAS: {
			log("Log::Application::wm_command_File() ID_FILE_SAVEAS");

			break;
		}
		case ID_FILE_EXPORT: {
			log("Log::Application::wm_command_File() ID_FILE_EXPORT");

			break;
		}
		case ID_FILE_EXPORTLAYOUTTOMODEL: {
			log("Log::Application::wm_command_File() ID_FILE_EXPORTLAYOUTTOMODEL");

			break;
		}
		case ID_FILE_DWGCONVERT: {
			log("Log::Application::wm_command_File() ID_FILE_DWGCONVERT");

			break;
		}
		case ID_FILE_ETRANSMIT: {
			log("Log::Application::wm_command_File() ID_FILE_ETRANSMIT");

			break;
		}
		case ID_FILE_SEND: {
			log("Log::Application::wm_command_File() ID_FILE_SEND");

			break;
		}
		case ID_FILE_PAGESETUPMANAGER: {
			log("Log::Application::wm_command_File() ID_FILE_PAGESETUPMANAGER");

			break;
		}
		case ID_FILE_PLOTTERMANAGER: {
			log("Log::Application::wm_command_File() ID_FILE_PLOTTERMANAGER");

			break;
		}
		case ID_FILE_PLOTSTYLEMANAGER: {
			log("Log::Application::wm_command_File() ID_FILE_PLOTSTYLEMANAGER");

			break;
		}
		case ID_FILE_PLOTPREVIEW: {
			log("Log::Application::wm_command_File() ID_FILE_PLOTPREVIEW");

			break;
		}
		case ID_FILE_PLOT: {
			log("Log::Application::wm_command_File() ID_FILE_PLOT");

			break;
		}
		case ID_FILE_PUBLISH: {
			log("Log::Application::wm_command_File() ID_FILE_PUBLISH");

			break;
		}
		case ID_FILE_VIEWPLOTANDPUBLISHDETAILS: {
			log("Log::Application::wm_command_File() ID_FILE_VIEWPLOTANDPUBLISHDETAILS");

			break;
		}
		case ID_FILE_DRAWINGUTILITIES_AUDIT: {
			log("Log::Application::wm_command_File() ID_FILE_DRAWINGUTILITIES_AUDIT");

			break;
		}
		case ID_FILE_DRAWINGUTILITIES_RECOVER: {
			log("Log::Application::wm_command_File() ID_FILE_DRAWINGUTILITIES_RECOVER");

			break;
		}
		case ID_FILE_DRAWINGUTILITIES_RECOVERDRAWINGANDXREFS: {
			log("Log::Application::wm_command_File() ID_FILE_DRAWINGUTILITIES_RECOVERDRAWINGANDXREFS");

			break;
		}
		case ID_FILE_DRAWINGUTILITIES_DRAWINGRECOVERYMANAGER: {
			log("Log::Application::wm_command_File() ID_FILE_DRAWINGUTILITIES_DRAWINGRECOVERYMANAGER");

			break;
		}
		case ID_FILE_DRAWINGUTILITIES_UPDATEBLOCKICONS: {
			log("Log::Application::wm_command_File() ID_FILE_DRAWINGUTILITIES_UPDATEBLOCKICONS");

			break;
		}
		case ID_FILE_DRAWINGUTILITIES_PURGE: {
			log("Log::Application::wm_command_File() ID_FILE_DRAWINGUTILITIES_PURGE");

			break;
		}
		case ID_FILE_DRAWINGPROPERTIES: {
			log("Log::Application::wm_command_File() ID_FILE_DRAWINGPROPERTIES");

			break;
		}
		case ID_FILE_EXIT: {
			log("Log::Application::wm_command_File() ID_FILE_EXIT");
			::DestroyWindow(m_hWnd);
			break;
		}
/***************************************************************EDIT***************************************************************/

/***************************************************************VIEW***************************************************************/
		case ID_VIEW_ZOOM: {
			log("Log::Application::wm_command_View() ID_VIEW_ZOOM");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_ZOOM);
			break;
		}
		case ID_VIEW_PAN: {
			log("Log::Application::wm_command_View() ID_VIEW_PAN");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_PAN);
			break;
		}
		case ID_VIEW_ORBIT_CONSTRAINEDORBIT: {
			log("Log::Application::wm_command_View() ID_VIEW_ORBIT_CONSTRAINEDORBIT");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_3DORBIT);
			break;
		}
		case ID_VIEW_ORBIT_FREEORBIT: {
			log("Log::Application::wm_command_View() ID_VIEW_ORBIT_FREEORBIT");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_3DFORBIT);
			break;
		}
		case ID_VIEW_ORBIT_CONTINUOUSORBIT: {
			log("Log::Application::wm_command_View() ID_VIEW_ORBIT_CONTINUOUSORBIT");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_3DCORBIT);
			break;
		}
		case ID_VIEW_CAMERA_ADJUSTDISTANCE: {
			log("Log::Application::wm_command_View() ID_VIEW_CAMERA_ADJUSTDISTANCE");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_3DDISTANCE);
			break;
		}
		case ID_VIEW_CAMERA_SWIVEL: {
			log("Log::Application::wm_command_View() ID_VIEW_CAMERA_SWIVEL");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_3DSWIVEL);
			break;
		}
		case ID_VIEW_VIEWPORTS_NAMED_VIEWPORTS: {
			log("Log::Application::wm_command_View() ID_VIEW_VIEWPORTS_NAMED_VIEWPORTS");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VPORTS, MAKELPARAM(0, 1));//Show Viewports window with default Named Viewports tab selected.
			break;
		}
		case ID_VIEW_VIEWPORTS_NEW_VIEWPORTS: {
			log("Log::Application::wm_command_View() ID_VIEW_VIEWPORTS_NEW_VIEWPORTS");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VPORTS, MAKELPARAM(0, 0));//Show Viewports window with New Viewports tab selected.
			break;
		}
		case ID_VIEW_VIEWMANAGER: {
			log("Log::Application::wm_command_View() ID_VIEW_VIEWMANAGER");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VIEW);
			break;
		}
		case IDC_VIEW_VIEWMANAGER_BUTTON_APPLY: {
			log("Log::Application::wm_command_View() IDC_VIEW_VIEWMANAGER_BUTTON_APPLY");
			SendCommandParam(hWnd, IDD_VIEWMANAGER, IDC__VIEW, lParam);
			break;
		}
		case ID_VIEW_3DVIEWS_TOP: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_TOP");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::Top)));
			break;
		}
		case ID_VIEW_3DVIEWS_BOTTOM: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_BOTTOM");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::Bottom)));
			break;
		}
		case ID_VIEW_3DVIEWS_LEFT: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_LEFT");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::Left)));
			break;
		}
		case ID_VIEW_3DVIEWS_RIGHT: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_RIGHT");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::Right)));
			break;
		}
		case ID_VIEW_3DVIEWS_FRONT: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_FRONT");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::Front)));
			break;
		}
		case ID_VIEW_3DVIEWS_BACK: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_BACK");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::Back)));
			break;
		}
		case ID_VIEW_3DVIEWS_SWISOMETRIC: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_SWISOMETRIC");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::SWIsometric)));
			break;
		}
		case ID_VIEW_3DVIEWS_SEISOMETRIC: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_SEISOMETRIC");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::SEIsometric)));
			break;
		}
		case ID_VIEW_3DVIEWS_NEISOMETRIC: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_NEISOMETRIC");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::NEIsometric)));
			break;
		}
		case ID_VIEW_3DVIEWS_NWISOMETRIC: {
			log("Log::Application::wm_command_View() ID_VIEW_3DVIEWS_NWISOMETRIC");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC__VIEW, MAKELPARAM(0, static_cast<int>(View::PresetView::NWIsometric)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_2DWIREFRAME: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_2DWIREFRAME");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::Wireframe2D)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_3DWIREFRAME: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_3DWIREFRAME");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::Wireframe3D)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_2DHIDDEN: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_2DHIDDEN");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::Hidden2D)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_3DHIDDEN: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_3DHIDDEN");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::Hidden3D)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_REALISTIC: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_REALISTIC");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::Realistic)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_CONCEPTUAL: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_CONCEPTUAL");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::Conceptual)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_SHADED: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_SHADED");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::Shaded)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_SHADEDWITHEDGES: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_SHADEDWITHEDGES");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::ShadedWithEdges)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_SHADESOFGREY: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_SHADESOFGREY");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::ShadesOfGrey)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_SKETCHY: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_SKETCHY");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::Sketchy)));
			break;
		}
		case ID_VIEW_VISUALSTYLES_XRAY: {
			log("Log::Application::wm_command_View() ID_VIEW_VISUALSTYLES_XRAY");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_VSCURRENT, MAKELPARAM(0, static_cast<int>(View::VisualStyle::XRay)));
			break;
		}
		case ID_VIEW_DISPLAY_UCSICON_VISIBILITY: {
			log("Log::Application::wm_command_View() ID_VIEW_DISPLAY_UCSICON_VISIBILITY");
			wm_command_View_Display_UCSIcon_On();
			break;
		}
		case ID_VIEW_DISPLAY_UCSICON_ORIGIN: {
			log("Log::Application::wm_command_View() ID_VIEW_DISPLAY_UCSICON_ORIGIN");
			wm_command_View_Display_UCSIcon_Position();
			break;
		}
		case ID_VIEW_DISPLAY_UCSICON_PROPERTIES: {
			log("Log::Application::wm_command_View() ID_VIEW_DISPLAY_UCSICON_PROPERTIES");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_UCSICON);
			break;
		}
		case ID_VIEW_DISPLAY_TEXTWINDOW: {
			log("Log::Application::wm_command_View() ID_VIEW_DISPLAY_TEXTWINDOW");
			TextWindow::s_nVisible = !TextWindow::s_nVisible;
			createTextWindow();
			break;
		}
/***************************************************************INSERT*************************************************************/
		case ID_INSERT_LAYOUT_NEW_LAYOUT: {
			log("Log::Application::wm_command_Insert() ID_INSERT_LAYOUT_NEW_LAYOUT");
			wm_command_Insert_NewLayout();
			break;
		}
/***************************************************************FORMAT*************************************************************/
		case ID_FORMAT_LAYER: {
			log("Log::Application::wm_command_Format() ID_FORMAT_LAYER");
			plt_LayerPropertiesManager::s_nVisible = !plt_LayerPropertiesManager::s_nVisible;
			createLayerPropertiesManager();
			//SendCommandParam(hwnd, IDC_APPLICATION, IDC_LAYER);
			break;
		}
		case ID_FORMAT_UNITS: {
			log("Log::Application::wm_command_Format() ID_FORMAT_UNITS");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_UNITS);
			break;
		}
/***************************************************************TOOLS**************************************************************/
		case ID_TOOLS_PALETTES_LAYER: {
			log("Log::Application::wm_command_Tools() ID_TOOLS_PALETTES_LAYER");
			plt_LayerPropertiesManager::s_nVisible = !plt_LayerPropertiesManager::s_nVisible;
			createLayerPropertiesManager();
			//SendCommandParam(hwnd, IDC_APPLICATION, IDC_LAYER);
			break;
		}
		case ID_TOOLS_COMMANDLINE: {
			log("Log::Application::wm_command_Tools() ID_TOOLS_COMMANDLINE");
			CommandLine::s_nVisible = !CommandLine::s_nVisible;
			createCommandLine();
			break;
		}
		case ID_TOOLS_NAMEDUCS: {
			log("Log::Application::wm_command_Tools() ID_TOOLS_NAMEDUCS");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_UCSMANAGER);
			break;
		}
		case ID_TOOLS_DRAFTINGSETTINGS: {
			log("Log::Application::wm_command_Tools() ID_TOOLS_DRAFTINGSETTINGS");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_DSETTINGS);
			break;
		}
		case ID_TOOLS_OPTIONS: {
			log("Log::Application::wm_command_Tools() ID_TOOLS_OPTIONS");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_OPTIONS);
			break;
		}
		case IDC_TOOLS_OPTIONS_APPLY: {
			log("Log::Application::wm_command_Tools() IDC_TOOLS_OPTIONS_APPLY");
			//if (m_lpOptions) {
			//
			//}
			break;
		}
/***************************************************************DRAW***************************************************************/
		case ID_DRAW_POINT_SINGLE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_POINT_SINGLE");

			break;
		}
		case ID_DRAW_POINT_MULTIPLE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_POINT_MULTIPLE");

			break;
		}
		case ID_DRAW_POINT_DIVIDE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_POINT_DIVIDE");

			break;
		}
		case ID_DRAW_POINT_MEASURE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_POINT_MEASURE");

			break;
		}
		case ID_DRAW_LINE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_LINE");

			break;
		}
		case ID_DRAW_RAY: {
			log("Log::Application::wm_command_Draw() ID_DRAW_RAY");

			break;
		}
		case ID_DRAW_XLINE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_XLINE");

			break;
		}
		case ID_DRAW_MULTILINE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_MULTILINE");

			break;
		}
		case ID_DRAW_POLYLINE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_POLYLINE");

			break;
		}
		case ID_DRAW_3DPOLYLINE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_3DPOLYLINE");

			break;
		}
		case ID_DRAW_POLYGON: {
			log("Log::Application::wm_command_Draw() ID_DRAW_POLYGON");

			break;
		}
		case ID_DRAW_RECTANGLE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_RECTANGLE");

			break;
		}
		case ID_DRAW_HELIX: {
			log("Log::Application::wm_command_Draw() ID_DRAW_HELIX");

			break;
		}
		case ID_DRAW_ARC: {
			log("Log::Application::wm_command_Draw() ID_DRAW_ARC");

			break;
		}
		case ID_DRAW_ARC_CENTRE_START_END: {
			log("Log::Application::wm_command_Draw() ID_DRAW_ARC_CENTRE_START_END");

			break;
		}
		case ID_DRAW_ARC_THREE_POINTS: {
			log("Log::Application::wm_command_Draw() ID_DRAW_ARC_THREE_POINTS");

			break;
		}
		case ID_DRAW_CIRCLE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_CIRCLE");

			break;
		}
		case ID_DRAW_CIRCLE_CENTRE_RADIUS: {
			log("Log::Application::wm_command_Draw() ID_DRAW_CIRCLE_CENTRE_RADIUS");

			break;
		}
		case ID_DRAW_CIRCLE_THREE_POINTS: {
			log("Log::Application::wm_command_Draw() ID_DRAW_CIRCLE_THREE_POINTS");

			break;
		}
		case ID_DRAW_SPLINE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_SPLINE");

			break;
		}
		case ID_DRAW_ELLIPSE: {
			log("Log::Application::wm_command_Draw() ID_DRAW_ELLIPSE");

			break;
		}
		case ID_DRAW_ELLIPSE_CENTRE_AXES: {
			log("Log::Application::wm_command_Draw() ID_DRAW_ELLIPSE_CENTRE_AXES");

			break;
		}
		case ID_DRAW_ELLIPSE_FOUR_POINTS: {
			log("Log::Application::wm_command_Draw() ID_DRAW_ELLIPSE_FOUR_POINTS");

			break;
		}
		case ID_DRAW_ELLIPSE_ARC: {
			log("Log::Application::wm_command_Draw() ID_DRAW_ELLIPSE_ARC");

			break;
		}
		case ID_DRAW_REVISION_CLOUD: {
			log("Log::Application::wm_command_Draw() ID_DRAW_REVISION_CLOUD");

			break;
		}
/***************************************************************DIMENSION**********************************************************/

/***************************************************************MODIFY*************************************************************/

/***************************************************************WINDOW*************************************************************/
		case ID_WINDOW_CLOSE: {
			log("Log::SDIApplication::wm_command_Window() ID_WINDOW_CLOSE");
			wm_command_Window_Close();
			break;
		}
		case ID_WINDOW_CLOSEALL: {
			log("Log::SDIApplication::wm_command_Window() ID_WINDOW_CLOSEALL");
			wm_command_Window_CloseAll();
			break;
		}
		case ID_WINDOW_CASCADE: {
			log("Log::SDIApplication::wm_command_Window() ID_WINDOW_CASCADE");
			wm_command_Window_Cascade();
			break;
		}
		case ID_WINDOW_TILEHORIZONTALLY: {
			log("Log::SDIApplication::wm_command_Window() ID_WINDOW_TILEHORIZONTALLY");
			wm_command_Window_TileHorizontally();
			break;
		}
		case ID_WINDOW_TILEVERTICALLY: {
			log("Log::SDIApplication::wm_command_Window() ID_WINDOW_TILEVERTICALLY");
			wm_command_Window_TileVertically();
			break;
		}
		case ID_WINDOW_ARRANGEICONS: {
			log("Log::SDIApplication::wm_command_Window() ID_WINDOW_ARRANGEICONS");
			wm_command_Window_ArrangeIcons();
			break;
		}
		case ID_WINDOW_WINDOWS: {
			log("Log::SDIApplication::wm_command_Window() ID_WINDOW_WINDOWS");
			wm_command_Window_Windows();
			break;
		}
/***************************************************************HELP***************************************************************/
		case ID_HELP_HELP: {
			log("Log::Application::wm_command_Help() ID_HELP_HELP");
			DialogBox(s_hInstance, MAKEINTRESOURCE(IDD_HELP_HELP), m_hWnd, (DLGPROC)s_nfnHelpProc);
			break;
		}
		case ID_HELP_ABOUT: {
			log("Log::Application::wm_command_Help() ID_HELP_ABOUT");
			DialogBox(s_hInstance, MAKEINTRESOURCE(IDD_HELP_ABOUT), m_hWnd, (DLGPROC)s_nfnAboutProc);
			break;
		}
/***************************************************************EXPRESS************************************************************/

/***************************************************************DIALOG CALLBACKS***************************************************/
		case IDC__VPORTS: {
			log("Log::Application::wm_command() IDC__VPORTS");
			SendCommandParam(hWnd, IDD_VIEWPORTS, IDC__VPORTS, lParam);
			break;
		}
		//case IDC_VIEW_VIEWMANAGER_BUTTON_APPLY: {
		//	log("Log::Application::wm_command() IDC_VIEW_VIEWMANAGER_BUTTON_APPLY");
		//	SendCommandParam(hWnd, IDD_VIEWMANAGER, IDC__VIEW, lParam);
		//	break;
		//}
		//case IDC_TOOLS_OPTIONS_APPLY: {
		//	log("Log::Application::wm_command() IDC_TOOLS_OPTIONS_APPLY");
		//	//if (m_lpOptions) {
		//	//
		//	//}
		//	break;
		//}
/***************************************************************MISCELLANEOUS******************************************************/
		//case IDC_GETVAR: {
		//	log("Log::Application::wm_command() IDC_GETVAR");
		//	//getSystemVariable((int)lParam);
		//	break;
		//}
		case IDC_SETVAR: {
			log("Log::Application::wm_command() IDC_SETVAR");
			setSystemVariable(LOWORD(lParam), HIWORD(lParam));
			break;
		}
/***************************************************************DEFAULT************************************************************/
		default: {
			return wm_command_Default(uMsg, wParam, lParam);
		}
	}

	return 0;
}
int Application::wm_destroy()
{
	BOOL nResult = m_system->getWindowManager().removeObject(this);
	if (nResult == TRUE) {
		log("Log::Application::wm_destroy() m_system->getWindowManager().removeWindow == TRUE");
	}
	else {
		log("Log::Application::wm_destroy() m_system->getWindowManager().removeWindow == FALSE");
	}

	char szClassName[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_DOCUMENT_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	::UnregisterClass(szClassName, s_hInstance);
	//::UnregisterClass(m_szClassName, s_hInstance);

	DestroyWindow(m_hTabcontrol_Documents);

	DestroyWindow(m_hToolbar_Draw);
	DestroyWindow(m_hToolbar_File);
	DestroyWindow(m_hRebar_Standard);

	PostQuitMessage(0);

	return 0;
}
int Application::wm_erasebkgnd(WPARAM wParam)
{
	//UNREFERENCED_PARAMETER(wParam);
	//return 0;

	HDC hDC = (HDC)wParam;
	RECT rcClient = { 0,0,0,0 };
	::GetClientRect(m_hWnd, &rcClient);
	::FillRect(hDC, &rcClient, (HBRUSH)(COLOR_MENU + 1));
	return TRUE;
}
int Application::wm_exitmenuloop(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	//log("Log::Application::wm_exitmenuloop()");

	return 0;
}
int Application::wm_exitsizemove()
{
	Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
	if (lpActiveDocument != nullptr) {
		::SendMessage(lpActiveDocument->getHWND(), WM_EXITSIZEMOVE, 0, 0);
	}

	wm_exitsizemove_statusbar_tooltips();

	return 0;
}
int Application::wm_getminmaxinfo(LPARAM lParam)
{
	RECT rcWindow;
	::GetWindowRect(m_hWnd, &rcWindow);

	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	POINT ptMin;
	//ptMin.x = (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
	ptMin.x = 250;
	ptMin.y = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);

	RECT rcRebar;
	::GetClientRect(m_hRebar_Standard, &rcRebar);

	ptMin.y += rcRebar.bottom;

	RECT rcStatusbar;
	::GetClientRect(m_lpStatusbar->getHWND(), &rcStatusbar);

	ptMin.y += rcStatusbar.bottom;

	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	lpMMI->ptMinTrackSize = ptMin;
	return 0;
}
int Application::wm_initmenu(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	return 0;
}
int Application::wm_initmenupopup(WPARAM wParam, LPARAM lParam)//For main menu. See Document for context menus.
{
	UNREFERENCED_PARAMETER(wParam);

	log("Log::Application::wm_initmenupopup()");

	HMENU hMenu = (HMENU)wParam;

	UINT nMenuItem = LOWORD(lParam);
	BOOL bWindowMenu = HIWORD(lParam);

	//int nMenuItemCount = ::GetMenuItemCount(hMenu);
	//for (int i = 0; i < nMenuItemCount; i++) {
	//	int nMenuID = ::GetMenuItemID(hMenu, i);
	//	if (nMenuID == ID_SHORTCUT_CUSTOMIZATION_COORDINATES) {
	//		log("Log::Application::wm_initmenupopup() m_bCustomization = true");
	//		statusbar_Menu_Customization_Init(hMenu);
	//		m_bMenuCustomization = true;
	//	}
	//}

	if (::GetMenuItemID(hMenu, 0) == ID_SHORTCUT_CUSTOMIZATION_COORDINATES) {
		statusbar_Menu_Customization_Init(hMenu);
		return 0;
	}

	if (bWindowMenu == TRUE) return 0;

	switch (nMenuItem) {
		case 0: {
			log("Log::Application::wm_initmenupopup() 0");
			break;
		}
		case 1: {
			log("Log::Application::wm_initmenupopup() 1");
			break;
		}
		case 2: {
			log("Log::Application::wm_initmenupopup() 2");
			wm_initmenupopup_View(hMenu);
			break;
		}
		case 3: {
			log("Log::Application::wm_initmenupopup() 3");
			break;
		}
		case 4: {
			log("Log::Application::wm_initmenupopup() 4");
			break;
		}
		case 5: {
			log("Log::Application::wm_initmenupopup() 5");
			break;
		}
		case 6: {
			log("Log::Application::wm_initmenupopup() 6");
			break;
		}
		case 7: {
			log("Log::Application::wm_initmenupopup() 7");
			break;
		}
		case 8: {
			log("Log::Application::wm_initmenupopup() 8");
			break;
		}
		case 9: {
			log("Log::Application::wm_initmenupopup() 9");
			break;
		}
		case 10: {
			log("Log::Application::wm_initmenupopup() 10");
			break;
		}
		case 11: {
			log("Log::Application::wm_initmenupopup() 11");
			break;
		}
		case 12: {
			log("Log::Application::wm_initmenupopup() 12");
			break;
		}
		case 13: {
			log("Log::Application::wm_initmenupopup() 13");
			break;
		}
		default: {
			break;
		}
	}

	return 0;
}
int Application::wm_keydown(WPARAM wParam, LPARAM lParam)
{
	return m_lpDocumentManager->sendMessage(WM_KEYDOWN, wParam, lParam);
}
int Application::wm_move(LPARAM lParam)
{
	m_x = GET_X_LPARAM(lParam);
	m_y = GET_Y_LPARAM(lParam);

	return 0;
}
int Application::wm_notify(WPARAM wParam, LPARAM lParam)
{
#define lpnmhdr (LPNMHDR(lParam))

	log("Log::Application::wm_notify() lpnmhdr->code = %d", lpnmhdr->code);

	switch (lpnmhdr->code) {
		case NM_CLICK: {//Tool bar/Status bar left-click notifications...

#define lpnmmouse (LPNMMOUSE(lParam))

			wm_notify_nm_click(lpnmhdr->idFrom, lpnmmouse->dwItemSpec);
			break;
		}
		case NM_RCLICK: {//Tool bar/Status bar right-click notifications...

#define lpnmmouse (LPNMMOUSE(lParam))

			wm_notify_nm_rclick(lpnmhdr->idFrom, lpnmmouse->dwItemSpec);
			break;
		}
		case NM_TOOLTIPSCREATED: {
			log("Log::Application::wm_notify() NM_TOOLTIPSCREATED");
			break;
		}
		case TCN_FOCUSCHANGE: {
			log("Log::Application::wm_notify() TCN_FOCUSCHANGE");
			break;
		}
		case NM_RELEASEDCAPTURE: {
			log("Log::Application::wm_notify() NM_RELEASEDCAPTURE");
			break;
		}
		case PBM_SETPOS: {//Progress bar notifications...
			switch (lpnmhdr->idFrom) {
				case IDC_DOCUMENT: {
					int nPos = (int)(LOWORD(wParam));
					m_lpStatusbar->showProgressbar(1, nPos);
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case RBN_HEIGHTCHANGE: {//Ribbon notifications...
			wm_notify_rbn_heightchange();
			break;
		}
		case SB_SETTEXT: {//Status bar notifications...
			switch (lpnmhdr->idFrom) {
				case IDC_DOCUMENT: {
					const char* szText = (LPSTR)wParam;
					m_lpStatusbar->setText(0, szText);
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case TBN_DROPDOWN: {//Tool bar notifications...

#define lpnmTB (LPNMTOOLBAR(lParam))

			wm_notify_tbn_dropdown(lpnmTB->hdr.hwndFrom, lpnmTB->iItem);
			return 0;//Return FALSE from TBN_DROPDOWN.
		}
		case TTN_GETDISPINFO: {//Tool tip notifications...
			log("Log::Application::wm_notify() TTN_GETDISPINFO");

#define lpttt (LPTOOLTIPTEXT(lParam))
			
			wm_notify_ttn_getdispinfo(lpttt->hdr, lpttt->lpszText);
			break;
		}
		case TCN_SELCHANGE: {//Tab control notifications...
			wm_notify_tcn_selchange(lpnmhdr->idFrom);
			break;
		}
		case TCN_SELCHANGING: {//Tab control notifications...
			wm_notify_tcn_selchanging(lpnmhdr->idFrom);
			break;
		}
		default: {
			break;
		}
	}
	return 0;
}
int Application::wm_paint()//TODO::This repaints the document, causing flickering between the OpenGL render context and the Windows device context.
{
	//PAINTSTRUCT ps;
	//HDC hDC = ::BeginPaint(m_hWnd, &ps);
	//::FillRect(hDC, &ps.rcPaint, (HBRUSH)(COLOR_MENU + 1));
	//::EndPaint(m_hWnd, &ps);
	//::ReleaseDC(m_hWnd, hDC);
	return 0;
}
int Application::wm_parentnotify(WPARAM wParam, LPARAM lParam)
{
	log("Log::Application::wm_parentnotify()");

	int nID = LOWORD(wParam);
	switch (nID) {
		case WM_CREATE: {
			log("Log::Application::wm_parentnotify() WM_CREATE");
			wm_parentnotify_create((UINT_PTR)HIWORD(wParam), (HWND)lParam);
			return(TRUE);
		}
		case WM_DESTROY: {
			log("Log::Application::wm_parentnotify() WM_DESTROY");
			wm_parentnotify_destroy((UINT_PTR)HIWORD(wParam), (HWND)lParam);
			break;
		}
		case WM_LBUTTONDOWN: {
			log("Log::Application::wm_parentnotify() WM_LBUTTONDOWN");
			//POINT ptCursor = MAKEPOINT(lParam);
			break;
		}
		case WM_MBUTTONDOWN: {
			log("Log::Application::wm_parentnotify() WM_MBUTTONDOWN");
			//POINT ptCursor = MAKEPOINT(lParam);
			break;
		}
		case WM_RBUTTONDOWN: {
			log("Log::Application::wm_parentnotify() WM_RBUTTONDOWN");
			//POINT ptCursor = MAKEPOINT(lParam);
			break;
		}
		case WM_XBUTTONDOWN: {
			log("Log::Application::wm_parentnotify() WM_XBUTTONDOWN");
			//switch (HIWORD(wParam)) {
			//	case XBUTTON1: {
			//		log("Log::Application::wm_parentnotify() WM_XBUTTONDOWN XBUTTON1 ");
			//		INT uID = (INT)HIWORD(wParam);
			//		HWND hWnd = (HWND)lParam;
			//		return(TRUE);
			//	}
			//	case XBUTTON2: {
			//		log("Log::Application::wm_parentnotify() WM_XBUTTONDOWN XBUTTON2");
			//		INT uID = (INT)HIWORD(wParam);
			//		HWND hWnd = (HWND)lParam;
			//		break;
			//	}
			//	default: {
			//		log("Log::Application::wm_parentnotify() WM_XBUTTONDOWN default");
			//		break;
			//	}
			//}
			//POINT ptCursor = MAKEPOINT(lParam);
			break;
		}
		case WM_POINTERDOWN: {
			log("Log::Application::wm_parentnotify() WM_POINTERDOWN");
			//INT npid = GET_POINTERID_WPARAM(wParam);
			//POINT ptCursor = MAKEPOINT(lParam);
			break;
		}
		default: {
			log("Log::Application::wm_parentnotify() default");
			break;
		}
	}

	return 0;
}
int Application::wm_setcursor(WPARAM wParam, LPARAM lParam)
{
	//Top-level window; set cursor and return DefWindowProc for further processing (child windows, dialogs, etc.)
	UNREFERENCED_PARAMETER(lParam);

	Document* document = (Document*)m_lpDocumentManager->getActiveObject();
	if (document) {
		if ((HWND)wParam == document->getHWND()) {
			return 1;
		}
	}

	return ::DefWindowProc(m_hWnd, WM_SETCURSOR, wParam, lParam);
}
int Application::wm_setfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	log("Log::Application::wm_setfocus()");

	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow != nullptr) {
			if (m_lpTextWindow->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
					::SendMessage(m_lpTextWindow->getHWND(), WM_KILLFOCUS, (WPARAM)m_hWnd, 0);
				}
			}
		}
	}

	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine != nullptr) {
			if (m_lpCommandLine->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
					::SendMessage(m_lpCommandLine->getHWND(), WM_KILLFOCUS, (WPARAM)m_hWnd, 0);
				}
			}
		}
	}

	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpDynamicInput != nullptr) {
			if (m_lpDynamicInput->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpDynamicInput->getHWND()) == TRUE) {
					::ShowWindow(m_lpDynamicInput->getHWND(), SW_HIDE);
				}
			}
		}
	}

	return 0;
}
int Application::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	//::SetFocus(m_hWnd);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::Application::wm_syscommand() SC_CLOSE");
			::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
			return(TRUE);
		}
		case SC_MAXIMIZE: {
			log("Log::Application::wm_syscommand() SC_MAXIMIZE");
			//setWindowState();
			break;
		}
		case SC_MINIMIZE: {
			log("Log::Application::wm_syscommand() SC_MINIMIZE");
			//setWindowState();
			break;
		}
		case SC_RESTORE: {
			log("Log::Application::wm_syscommand() SC_RESTORE");
			break;
		}
		default: {
			log("Log::Application::wm_syscommand() default");
			break;
		}
	}

	return 0;
}
int Application::wm_uninitmenupopup(WPARAM wParam, LPARAM lParam)//For main menu. See Document for context menus.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	//log("Log::Application::wm_uninitmenupopup()");

	//HMENU hMenu = (HMENU)wParam;
	//
	//int nMenuItemCount = ::GetMenuItemCount(hMenu);
	//for (int i = 0; i < nMenuItemCount; i++) {
	//	int nMenuID = ::GetMenuItemID(hMenu, i);
	//	if (nMenuID == ID_SHORTCUT_CUSTOMIZATION_COORDINATES) {
	//		m_bCustomization = false;
	//		log("Log::Application::wm_uninitmenupopup() m_bCustomization = false");
	//	}
	//}

	return 0;
}
int Application::wm_windowposchanged(LPARAM lParam)
{
	//UNREFERENCED_PARAMETER(lParam);

	log("Log::Document::wm_windowposchanged()");

	LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;

	m_x = lpwp->x;
	m_y = lpwp->y;

	m_cx = lpwp->cx;
	m_cy = lpwp->cy;

	wm_size();

	return 0;
}
int Application::wm_windowposchanging(LPARAM lParam)
{
	//UNREFERENCED_PARAMETER(lParam);

	log("Log::Document::wm_windowposchanging()");

	LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;

	m_x = lpwp->x;
	m_y = lpwp->y;

	m_cx = lpwp->cx;
	m_cy = lpwp->cy;

	wm_size();

	return 0;
}

//Private variables/functions.
void Application::setApplicationState()//Saves the application window state to the registry.
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
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\FixedProfile\\Application Window");

	RECT rcPosition = { 0,0,0,0 };
	int nShow = SW_NORMAL;
	if (::IsIconic(m_hWnd) == FALSE) {//If the application window is not minimized...
		if (::IsZoomed(m_hWnd) == TRUE) {
			nShow = SW_SHOWMAXIMIZED;
		}
		else {//...and the application window is not maximized...
			::GetWindowRect(m_hWnd, &rcPosition);
			rcPosition.right -= rcPosition.left;
			rcPosition.bottom -= rcPosition.top;
			SetUserValue(chPath, "Position", REG_BINARY, &rcPosition, sizeof(RECT));//...then save the application window position.
		}
	}
	SetUserValue(chPath, "Show", REG_DWORD, &nShow, sizeof(long));

	//::strcpy_s(chPath, chDir);
	//_tcscat_s(chPath, STRING_SIZE(chPath), "\\Profiles\\<<Unnamed Profile>>\\Drawing Window");
	//
	//BOOL nScrollbars = false;
	//if (GLRenderWindow::s_bScrollbars == true) {
	//	nScrollbars = true;
	//}
	//SetUserValue(chPath, "Scrollbars", REG_DWORD, &nScrollbars, sizeof(BOOL));
}

void Application::setSystemVariable(int variableid, int value)
{
	char szTooltipTitle[MAX_TTM_SETTITLE];
	::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_ON_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);

	switch (variableid) {
		case IDC_PERSPECTIVE: {
			Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
			if (lpActiveDocument == nullptr) {
				ErrorHandler();
				break;
			}

			if (value == 0) {
				if (lpActiveDocument->getActiveLayout()->getActiveSpace()->setPerspective(false) == true) {
					SendMessage(lpActiveDocument->getHWND(), WM_PAINT, 0, 0);
				}
			}
			else if (value == 1) {
				if (lpActiveDocument->getActiveLayout()->getActiveSpace()->setPerspective(true) == true) {
					SendMessage(lpActiveDocument->getHWND(), WM_PAINT, 0, 0);
				}
			}
			break;
		}
		case IDC_TILEMODE: {
			Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
			if (lpActiveDocument == nullptr) {
				ErrorHandler();
				break;
			}
			lpActiveDocument->getLayoutManager()->setTilemode(value);
			break;
		}
		case IDC_DYNINFOTIPS: {

			break;
		}
		case IDC_DYNMODE: {
			if (value > 0) {
				DynamicInput::s_nVisible = TRUE;
				::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_ON_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);
			}
			else {
				DynamicInput::s_nVisible = FALSE;
				::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_OFF_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);
			}
			m_lpStatusbar->setTooltipTitle(5, szTooltipTitle);
			createDynamicInput();

			if (value <= 0) {
				wnd_PointerInput::s_nVisible = FALSE;
				wnd_DimensionInput::s_nVisible = FALSE;
			}
			if (value == 1) {
				wnd_PointerInput::s_nVisible = TRUE;
				wnd_DimensionInput::s_nVisible = FALSE;
			}
			if (value == 2) {
				wnd_PointerInput::s_nVisible = FALSE;
				wnd_DimensionInput::s_nVisible = TRUE;
			}
			if (value == 3) {
				wnd_PointerInput::s_nVisible = TRUE;
				wnd_DimensionInput::s_nVisible = TRUE;
			}

			break;
		}
		case IDC_DYNPROMPT: {
			int nDynmode = CommandWindow::getSystemVariable(IDC_DYNMODE);
			if (nDynmode <= 0) {
				DynamicInput::s_nVisible = FALSE;
				::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_OFF_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);
			}
			else if (nDynmode > 0) {
				if (value == 0) {
					DynamicInput::s_nVisible = FALSE;
					::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_OFF_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);
				}
				else if (value == 1) {
					DynamicInput::s_nVisible = TRUE;
					::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_ON_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);
				}
			}
			m_lpStatusbar->setTooltipTitle(5, szTooltipTitle);
			createDynamicInput();
			break;
		}
		default: {
			break;
		}
	}
	CommandWindow::setSystemVariable(variableid, value);
}
void Application::getSystemVariables()
{
	char szTooltipTitle[MAX_TTM_SETTITLE];
	::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_ON_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);

	int nDynmode = CommandWindow::getSystemVariable(IDC_DYNMODE);
	int nDynprompt = CommandWindow::getSystemVariable(IDC_DYNPROMPT);
	//int nDyninfotips = CommandWindow::getSystemVariable(IDC_DYNINFOTIPS);

	if (nDynmode <= 0) {
		wnd_PointerInput::s_nVisible = FALSE;
		wnd_DimensionInput::s_nVisible = FALSE;
	}
	if (nDynmode == 1) {
		wnd_PointerInput::s_nVisible = TRUE;
		wnd_DimensionInput::s_nVisible = FALSE;
	}
	if (nDynmode == 2) {
		wnd_PointerInput::s_nVisible = FALSE;
		wnd_DimensionInput::s_nVisible = TRUE;
	}
	if (nDynmode == 3) {
		wnd_PointerInput::s_nVisible = TRUE;
		wnd_DimensionInput::s_nVisible = TRUE;
	}

	if (nDynmode <= 0) {
		DynamicInput::s_nVisible = FALSE;
		::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_OFF_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);
	}
	else if (nDynmode > 0) {
		if (nDynprompt == 0) {
			DynamicInput::s_nVisible = FALSE;
			::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_OFF_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);
		}
		else if (nDynprompt == 1) {
			DynamicInput::s_nVisible = TRUE;
			::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_ON_TITLE, szTooltipTitle, MAX_TTM_SETTITLE);
		}
	}
	m_lpStatusbar->setTooltipTitle(5, szTooltipTitle);
}

//GetSystemMetrics
int Application::gsmGetScreenSize()//Returns diagonal size of screen.
{
	int cx = ::GetSystemMetrics(SM_CXSCREEN);//1600
	int cy = ::GetSystemMetrics(SM_CYSCREEN);//900
	int nSceensize = sqrt(pow(cx, 2) + pow(cy, 2));//1835
	return nSceensize;
}

//SystemParametersInfo
void Application::spiGetMouseHoverSize(UINT cx, UINT cy)
{
	BOOL nResult = FALSE;

	UINT nWidth = cx;
	UINT nHeight = cy;

	nResult = ::SystemParametersInfo(SPI_GETMOUSEHOVERWIDTH, sizeof(UINT), &nWidth, 0);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	nResult = ::SystemParametersInfo(SPI_GETMOUSEHOVERHEIGHT, sizeof(UINT), &nHeight, 0);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	CSelect::s_szMouseHoverSize = { (long)nWidth, (long)nHeight };
}

void Application::calculateWindowPosition(HWND hwnd, int x, int y, int cx, int cy, POINT* lppt)
{
	//First get the dimensions of the monitor that contains (x, y).
	lppt->x = x;
	lppt->y = y;
	HMONITOR hMon = ::MonitorFromPoint(*lppt, MONITOR_DEFAULTTONULL);

	//If (x, y) is not on any monitor, then use the monitor that the owner window is on.
	if (hMon == NULL) {
		hMon = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	}

	MONITORINFO minf;
	minf.cbSize = sizeof(minf);
	GetMonitorInfo(hMon, &minf);

	//Now slide things around until they fit.

	//If too high, then slide down.
	if (lppt->y < minf.rcMonitor.top) {
		lppt->y = minf.rcMonitor.top;
	}

	//If too far left, then slide right.
	if (lppt->x < minf.rcMonitor.left) {
		lppt->x = minf.rcMonitor.left;
	}

	//If too low, then slide up.
	if (lppt->y > minf.rcMonitor.bottom - cy) {
		lppt->y = minf.rcMonitor.bottom - cy;
	}

	//If too far right, then flip left.
	if (lppt->x > minf.rcMonitor.right - cx) {
		lppt->x = lppt->x - cx;
	}
}

//Document Window
void Application::createDocument(const char* filename) { UNREFERENCED_PARAMETER(filename); }
void Application::createDocument(const char* filename, DWORD style, DWORD exstyle) { UNREFERENCED_PARAMETER(filename); UNREFERENCED_PARAMETER(style); UNREFERENCED_PARAMETER(exstyle); }

//Command Line Windows
void Application::createTextWindow()
{
	if (m_lpTextWindow == nullptr) {
		return;
	}

	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow->getValid() == FALSE) {
			char szTextWindowClassName[MAX_LOADSTRING]{ 0 };
			::LoadString(s_hInstance, ID_TEXTWINDOW_CLASS, szTextWindowClassName, sizeof(szTextWindowClassName) / sizeof(char));

			char szTextWindowName[MAX_LOADSTRING]{ 0 };
			::LoadString(NULL, ID_TEXTWINDOW_NAME, szTextWindowName, sizeof(szTextWindowName) / sizeof(char));

			Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
			if (lpActiveDocument) {
				char* lpszWindowName = lpActiveDocument->getWindowName();
				::strcat_s(szTextWindowName, MAX_LOADSTRING, " - ");
				::strcat_s(szTextWindowName, MAX_LOADSTRING, lpszWindowName);
			}

			//DWORD dwExStyle = WS_EX_TOPMOST;
			DWORD dwExStyle = NULL;

			DWORD dwStyle = WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_TABSTOP | WS_VISIBLE;
			HMENU hMenu = NULL;
			if (TextWindow::s_nDocked == FALSE) {

			}
			else {
				dwStyle |= WS_CHILD;
				hMenu = (HMENU)IDC_TEXTWINDOW;
			}

			RECT rcPosition = { 0,0,0,0 };
			getWindowState(WINDOWSTATE_RECT, &rcPosition, "\\FixedProfile\\Command Line Windows", "TextWindow.Position");

			m_system->createWindow(szTextWindowClassName,
				szTextWindowName,
				(WindowWin32*)m_lpTextWindow,
				dwExStyle,
				dwStyle,
				rcPosition.left,
				rcPosition.top,
				rcPosition.right,
				rcPosition.bottom,
				NULL
			);
		}
		else {
			BOOL bVisible = ::IsWindowVisible(m_lpTextWindow->getHWND());
			if (bVisible == FALSE) {
				bVisible = ::ShowWindow(m_lpTextWindow->getHWND(), SW_SHOW);
				if (bVisible == FALSE) {
					log("Log::Application::createTextWindow() TextWindow was hidden");
				}
			}
		}
	}
	else if (TextWindow::s_nVisible == FALSE) {
		if (m_lpTextWindow->getValid() == TRUE) {
			BOOL bVisible = ::IsWindowVisible(m_lpTextWindow->getHWND());
			if (bVisible == TRUE) {
				LRESULT lResult = (LRESULT)::SendMessage(m_lpTextWindow->getHWND(), WM_CLOSE, 0, 0);
				if (lResult == FALSE) {
					log("Log::Application::createTextWindow() WM_CLOSE = 0");
				}
			}
		}
	}
}
void Application::createCommandLine()//Uses SetLayeredWindowAttributes for transparent window.
{
	if (m_lpCommandLine == nullptr) {
		return;
	}

	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine->getValid() == FALSE) {
			char szCommandLineClassName[MAX_LOADSTRING]{ 0 };
			::LoadString(s_hInstance, ID_COMMANDLINE_CLASS, szCommandLineClassName, sizeof(szCommandLineClassName) / sizeof(char));

			char chWindowName[MAX_LOADSTRING]{ 0 };
			::LoadString(NULL, ID_COMMANDLINE_NAME, chWindowName, sizeof(chWindowName) / sizeof(char));

			//DWORD dwExStyle = WS_EX_TOPMOST;
			DWORD dwExStyle = NULL;

			DWORD dwStyle = WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_TABSTOP | WS_VISIBLE;
			HMENU hMenu = NULL;
			if (CommandLine::s_nDocked == FALSE) {
				dwExStyle |= WS_EX_LAYERED;
			}
			else {
				dwStyle |= WS_CHILD;
				hMenu = (HMENU)IDC_COMMANDLINE;
			}

			RECT rcPosition = { 0,0,0,0 };
			getWindowState(WINDOWSTATE_RECT, &rcPosition, "\\FixedProfile\\Command Line Windows", "CommandLine.Position");

			m_system->createWindow(
				szCommandLineClassName,
				chWindowName,
				(WindowWin32*)m_lpCommandLine,
				dwExStyle,
				dwStyle,
				rcPosition.left,
				rcPosition.top,
				rcPosition.right,
				rcPosition.bottom,
				m_hWnd,
				hMenu
			);

			if (m_lpCommandLine->getValid()) {
				if (CommandLine::s_nDocked == FALSE) {
					::SetLayeredWindowAttributes(m_lpCommandLine->getHWND(), 0, (255 * 75) / 100, LWA_ALPHA);
				}
			}

		}
		else {
			BOOL bVisible = ::IsWindowVisible(m_lpCommandLine->getHWND());
			if (bVisible == FALSE) {
				bVisible = ::ShowWindow(m_lpCommandLine->getHWND(), SW_SHOW);
				if (bVisible == FALSE) {
					log("Log::Application::createCommandLine() CommandLine was hidden");
				}
			}
		}
	}
	else if (CommandLine::s_nVisible == FALSE) {
		if (m_lpCommandLine->getValid() == TRUE) {
			BOOL bVisible = ::IsWindowVisible(m_lpCommandLine->getHWND());
			if (bVisible == TRUE) {
				LRESULT lResult = (LRESULT)::SendMessage(m_lpCommandLine->getHWND(), WM_CLOSE, 0, 0);
				if (lResult == FALSE) {
					log("Log::Application::createCommandLine() WM_CLOSE = 0");
				}
				//if (CommandLine::s_nCheckBeforeClose == TRUE) {
				//	if (DialogBox(s_hInstance, MAKEINTRESOURCE(IDD_COMMANDLINE_CLOSECHECK), m_hWnd, (DLGPROC)CommandLine::s_nfnCommandLineCloseCheckProc) == IDYES) {
				//		if (::SendMessage(m_lpCommandLine->getHWND(), WM_CLOSE, 0, 0) == 0) {
				//			log("Log::Application::createCommandLine() WM_CLOSE = 0");
				//		}
				//	}
				//	else {
				//
				//	}
				//}
				//else {
				//	if (::SendMessage(m_lpCommandLine->getHWND(), WM_CLOSE, 0, 0) == 0) {
				//		log("Log::Application::createCommandLine() WM_CLOSE = 0");
				//	}
				//}
			}
		}
	}
}
void Application::createDynamicInput()
{
	if (m_lpDynamicInput == nullptr) {
		return;
	}

	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpDynamicInput->getValid() == FALSE) {
			char szDynamicInputClassName[MAX_LOADSTRING]{ 0 };
			::LoadString(s_hInstance, ID_DYNAMICINPUT_CLASS, szDynamicInputClassName, sizeof(szDynamicInputClassName) / sizeof(char));

			POINT ptCursor = { 0,0 };
			::GetCursorPos(&ptCursor);

			m_system->createWindow(
				szDynamicInputClassName,
				NULL,
				(WindowWin32*)m_lpDynamicInput,
				WS_EX_NOACTIVATE,
				WS_POPUP | WS_BORDER,
				ptCursor.x + DynamicInput::s_szOffset.cx,
				ptCursor.y + DynamicInput::s_szOffset.cy,
				DynamicInput::s_szSize.cx,
				DynamicInput::s_szSize.cy,
				m_hWnd,
				NULL,
				SW_HIDE
			);
		}
	}
	else if (DynamicInput::s_nVisible == FALSE) {
		if (m_lpDynamicInput->getValid() == TRUE) {
			BOOL bVisible = ::IsWindowVisible(m_lpDynamicInput->getHWND());
			if (bVisible == TRUE) {
				LRESULT lResult = (LRESULT)::SendMessage(m_lpDynamicInput->getHWND(), WM_CLOSE, 0, 0);
				if (lResult == FALSE) {
					log("Log::Application::createDynamicInput() WM_CLOSE = 0");
				}
			}
		}
	}
}

//Tool Palette Windows
void Application::createLayerPropertiesManager()
{
	char szLayerPropertiesManagerClassName[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_LAYER_PROPERTIES_MANAGER_CLASS, szLayerPropertiesManagerClassName, sizeof(szLayerPropertiesManagerClassName) / sizeof(char));

	if (plt_LayerPropertiesManager::s_nVisible == TRUE) {
		char szLayerPropertiesManagerWindowName[MAX_LOADSTRING]{ 0 };
		::LoadString(s_hInstance, ID_LAYER_PROPERTIES_MANAGER_NAME, szLayerPropertiesManagerWindowName, sizeof(szLayerPropertiesManagerWindowName) / sizeof(char));

		RECT rcPosition = { 0,0,0,0 };
		getWindowState(WINDOWSTATE_POSITION, &rcPosition, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\LayerManager");
		getWindowState(WINDOWSTATE_SIZE, &rcPosition, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\LayerManager");

		plt_LayerPropertiesManager* lpLayerPropertiesManager = new plt_LayerPropertiesManager();

		DWORD dwExStyle = WS_EX_TOPMOST;
		//DWORD dwStyle = WS_CLIPSIBLINGS | WS_VISIBLE | DS_CENTER | WS_TABSTOP;
		DWORD dwStyle = WS_GROUP | WS_TABSTOP | WS_VISIBLE;
		HMENU hMenu = 0;

		if (plt_LayerPropertiesManager::s_nDocked == TRUE) {
			dwStyle |= WS_CHILD;
			hMenu = (HMENU)IDC_LAYERPROPERTIESMANAGER;
		}
		else {
			dwExStyle |= WS_EX_TOOLWINDOW;
			dwStyle |= WS_OVERLAPPEDWINDOW;
		}

		m_system->createWindow(szLayerPropertiesManagerClassName,
			szLayerPropertiesManagerWindowName,
			(WindowWin32*)lpLayerPropertiesManager,
			dwExStyle,
			dwStyle,
			rcPosition.left,
			rcPosition.top,
			rcPosition.right,
			rcPosition.bottom,
			m_hWnd,
			hMenu
		);
	}
	else if (plt_LayerPropertiesManager::s_nVisible == FALSE) {
		plt_LayerPropertiesManager* lpLayerPropertiesManager = (plt_LayerPropertiesManager*)m_system->getWindowManager().getWindow(szLayerPropertiesManagerClassName);
		if (lpLayerPropertiesManager != nullptr) {
			if (lpLayerPropertiesManager->getValid() == TRUE) {
				if (::SendMessage(lpLayerPropertiesManager->getHWND(), WM_CLOSE, 0, 0) == 0) {
					log("Log::Application::createLayerPropertiesManager() WM_CLOSE = 0");
				}
			}
		}
	}
}

//Statusbar shortcut menus.
void Application::statusbar_Menu_Coordinates()
{
	int nStatusbarPanelPosition[STATUSBARPANELCOUNT];
	m_lpStatusbar->getParts(STATUSBARPANELCOUNT, nStatusbarPanelPosition);

	RECT rcStatusbar;
	GetClientRect(m_lpStatusbar->getHWND(), &rcStatusbar);

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_SHORTCUT_COORDINATES));
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
	POINT ptCursor = { m_x + nStatusbarPanelPosition[0] - 1,m_y + m_cy - rcStatusbar.bottom - 3 };
	UINT uReturnCmd = ::TrackPopupMenuEx(
		hSubMenu,
		TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
		ptCursor.x,
		ptCursor.y,
		m_hWnd,
		NULL
	);

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	switch (uReturnCmd) {
		case ID_SHORTCUT_COORDINATES_RELATIVE: {

			break;
		}
		case ID_SHORTCUT_COORDINATES_ABSOLUTE: {

			break;
		}
		case ID_SHORTCUT_COORDINATES_GEOGRAPHIC: {

			break;
		}
		case ID_SHORTCUT_COORDINATES_SPECIFIC: {

			break;
		}
		default: {
			break;
		}
	}

	DestroyMenu(hMenu);
}
void Application::statusbar_Menu_Grid()
{
	int nStatusbarPanelPosition[STATUSBARPANELCOUNT];
	m_lpStatusbar->getParts(STATUSBARPANELCOUNT, nStatusbarPanelPosition);

	RECT rcStatusbar;
	GetClientRect(m_lpStatusbar->getHWND(), &rcStatusbar);

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_SHORTCUT_GRID));
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
	POINT ptCursor = { m_x + nStatusbarPanelPosition[2] - 1,m_y + m_cy - rcStatusbar.bottom - 3 };
	UINT uReturnCmd = ::TrackPopupMenuEx(
		hSubMenu,
		TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
		ptCursor.x,
		ptCursor.y,
		m_hWnd,
		NULL
	);

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	switch (uReturnCmd) {
		case ID_SHORTCUT_GRID_SETTINGS: {

			break;
		}
		default: {
			break;
		}
	}

	DestroyMenu(hMenu);
}
void Application::statusbar_Menu_SnapMode()
{
	int nStatusbarPanelPosition[STATUSBARPANELCOUNT];
	m_lpStatusbar->getParts(STATUSBARPANELCOUNT, nStatusbarPanelPosition);

	RECT rcStatusbar;
	GetClientRect(m_lpStatusbar->getHWND(), &rcStatusbar);

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_SHORTCUT_SNAPMODE));
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
	POINT ptCursor = { m_x + nStatusbarPanelPosition[3] - 1,m_y + m_cy - rcStatusbar.bottom - 3 };
	UINT uReturnCmd = ::TrackPopupMenuEx(
		hSubMenu,
		TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
		ptCursor.x,
		ptCursor.y,
		m_hWnd,
		NULL
	);

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	switch (uReturnCmd) {
		case ID_SHORTCUT_SNAPMODE_POLARSNAP: {

			break;
		}
		case ID_SHORTCUT_SNAPMODE_GRIDSNAP: {

			break;
		}
		case ID_SHORTCUT_SNAPMODE_SETTINGS: {

			break;
		}
		default: {
			break;
		}
	}

	DestroyMenu(hMenu);
}
void Application::statusbar_Menu_DynamicInput()
{
	int nStatusbarPanelPosition[STATUSBARPANELCOUNT];
	m_lpStatusbar->getParts(STATUSBARPANELCOUNT, nStatusbarPanelPosition);

	RECT rcStatusbar;
	GetClientRect(m_lpStatusbar->getHWND(), &rcStatusbar);

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_SHORTCUT_DYNAMICINPUT));
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
	POINT ptCursor = { m_x + nStatusbarPanelPosition[5] - 1,m_y + m_cy - rcStatusbar.bottom - 3 };
	UINT uReturnCmd = ::TrackPopupMenuEx(
		hSubMenu,
		TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
		ptCursor.x,
		ptCursor.y,
		m_hWnd,
		NULL
	);

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	switch (uReturnCmd) {
		case ID_SHORTCUT_DYNAMICINPUT_SETTINGS: {
			Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
			if (lpActiveDocument == nullptr) {
				break;
			}
			if (lpActiveDocument->getValid()) {
				HWND hWnd = lpActiveDocument->getHWND();
				SendCommandParam(hWnd, IDC_APPLICATION, IDC_DSETTINGS);
			}
			break;
		}
		default: {
			break;
		}
	}

	DestroyMenu(hMenu);
}
void Application::statusbar_Menu_Customization_Init(HMENU hMenu)
{
	BOOL nResult = FALSE;

	MENUITEMINFO mii{ 0 };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;

	if (m_lpStatusbar->getPanelVisibility(0) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_COORDINATES, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(1) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_MODELSPACE, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(2) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_GRID, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(3) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_SNAPMODE, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(4) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_INFERCONSTRAINTS, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(5) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_DYNAMICINPUT, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(6) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_ORTHOMODE, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(7) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_POLARTRACKING, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(8) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_ISOMETRICDRAFTING, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(9) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_OBJECTSNAPTRACKING, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(10) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_2DOBJECTSNAP, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(11) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_LINEWEIGHT, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(12) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_TRANSPARENCY, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(13) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_SELECTIONCYCLING, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(14) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_3DOBJECTSNAP, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(15) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_DYNAMICUCS, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(16) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_SELECTIONFILTERING, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(17) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_GIZMO, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(18) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_ANNOTATIONVISIBILITY, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(19) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_AUTOSCALE, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(20) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_ANNOTATIONSCALE, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(21) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_WORKSPACESWITCHING, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(22) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_ANNOTATIONMONITIOR, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(23) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_UNITS, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(24) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_QUICKPROPERTIES, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(25) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_LOCKUI, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(26) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_ISOLATEOBJECTS, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(27) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_GRAPHICSPERFORMANCE, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	if (m_lpStatusbar->getPanelVisibility(28) == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}
	nResult = ::SetMenuItemInfo(hMenu, ID_SHORTCUT_CUSTOMIZATION_CLEANSCREEN, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}
}
void Application::statusbar_Menu_Customization()
{
	int nStatusbarPanelPosition[STATUSBARPANELCOUNT];
	m_lpStatusbar->getParts(STATUSBARPANELCOUNT, nStatusbarPanelPosition);

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_SHORTCUT_CUSTOMIZATION));
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);

	RECT rcStatusbar;
	GetClientRect(m_lpStatusbar->getHWND(), &rcStatusbar);

	POINT ptCursor = { m_x + nStatusbarPanelPosition[30],m_y + m_cy - rcStatusbar.bottom - 3 };

	UINT uReturnCmd = ::TrackPopupMenuEx(
		hSubMenu,
		TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
		ptCursor.x,
		ptCursor.y,
		m_hWnd,
		NULL
	);

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	switch (uReturnCmd) {
		case ID_SHORTCUT_CUSTOMIZATION_COORDINATES: {
			m_lpStatusbar->setPanelVisibility(0, !m_lpStatusbar->getPanelVisibility(0));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_MODELSPACE: {
			m_lpStatusbar->setPanelVisibility(1, !m_lpStatusbar->getPanelVisibility(1));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_GRID: {
			m_lpStatusbar->setPanelVisibility(2, !m_lpStatusbar->getPanelVisibility(2));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_SNAPMODE: {
			m_lpStatusbar->setPanelVisibility(3, !m_lpStatusbar->getPanelVisibility(3));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_INFERCONSTRAINTS: {
			m_lpStatusbar->setPanelVisibility(4, !m_lpStatusbar->getPanelVisibility(4));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_DYNAMICINPUT: {
			m_lpStatusbar->setPanelVisibility(5, !m_lpStatusbar->getPanelVisibility(5));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_ORTHOMODE: {
			m_lpStatusbar->setPanelVisibility(6, !m_lpStatusbar->getPanelVisibility(6));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_POLARTRACKING: {
			m_lpStatusbar->setPanelVisibility(7, !m_lpStatusbar->getPanelVisibility(7));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_ISOMETRICDRAFTING: {
			m_lpStatusbar->setPanelVisibility(8, !m_lpStatusbar->getPanelVisibility(8));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_OBJECTSNAPTRACKING: {
			m_lpStatusbar->setPanelVisibility(9, !m_lpStatusbar->getPanelVisibility(9));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_2DOBJECTSNAP: {
			m_lpStatusbar->setPanelVisibility(10, !m_lpStatusbar->getPanelVisibility(10));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_LINEWEIGHT: {
			m_lpStatusbar->setPanelVisibility(11, !m_lpStatusbar->getPanelVisibility(11));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_TRANSPARENCY: {
			m_lpStatusbar->setPanelVisibility(12, !m_lpStatusbar->getPanelVisibility(12));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_SELECTIONCYCLING: {
			m_lpStatusbar->setPanelVisibility(13, !m_lpStatusbar->getPanelVisibility(13));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_3DOBJECTSNAP: {
			m_lpStatusbar->setPanelVisibility(14, !m_lpStatusbar->getPanelVisibility(14));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_DYNAMICUCS: {
			m_lpStatusbar->setPanelVisibility(15, !m_lpStatusbar->getPanelVisibility(15));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_SELECTIONFILTERING: {
			m_lpStatusbar->setPanelVisibility(16, !m_lpStatusbar->getPanelVisibility(16));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_GIZMO: {
			m_lpStatusbar->setPanelVisibility(17, !m_lpStatusbar->getPanelVisibility(17));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_ANNOTATIONVISIBILITY: {
			m_lpStatusbar->setPanelVisibility(18, !m_lpStatusbar->getPanelVisibility(18));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_AUTOSCALE: {
			m_lpStatusbar->setPanelVisibility(19, !m_lpStatusbar->getPanelVisibility(19));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_ANNOTATIONSCALE: {
			m_lpStatusbar->setPanelVisibility(20, !m_lpStatusbar->getPanelVisibility(20));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_WORKSPACESWITCHING: {
			m_lpStatusbar->setPanelVisibility(21, !m_lpStatusbar->getPanelVisibility(21));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_ANNOTATIONMONITIOR: {
			m_lpStatusbar->setPanelVisibility(22, !m_lpStatusbar->getPanelVisibility(22));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_UNITS: {
			m_lpStatusbar->setPanelVisibility(23, !m_lpStatusbar->getPanelVisibility(23));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_QUICKPROPERTIES: {
			m_lpStatusbar->setPanelVisibility(24, !m_lpStatusbar->getPanelVisibility(24));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_LOCKUI: {
			m_lpStatusbar->setPanelVisibility(25, !m_lpStatusbar->getPanelVisibility(25));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_ISOLATEOBJECTS: {
			m_lpStatusbar->setPanelVisibility(26, !m_lpStatusbar->getPanelVisibility(26));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_GRAPHICSPERFORMANCE: {
			m_lpStatusbar->setPanelVisibility(27, !m_lpStatusbar->getPanelVisibility(27));
			break;
		}
		case ID_SHORTCUT_CUSTOMIZATION_CLEANSCREEN: {
			m_lpStatusbar->setPanelVisibility(28, !m_lpStatusbar->getPanelVisibility(28));
			break;
		}
		default: {
			break;
		}
	}

	::DestroyMenu(hMenu);

	wm_size_statusbar();//Resize statusbar panels.
	wm_exitsizemove_statusbar_tooltips();//Resize tooltip positions.
}

void Application::wm_command_File_New()
{
	char szFileName[MAX_LOADSTRING]{ 0 };

	char szDefaultDirectory[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_DEFAULTDIRECTORY, szDefaultDirectory, sizeof(szDefaultDirectory) / sizeof(char));

	::strcpy_s(szFileName, szDefaultDirectory);

	char szDefaultDocumentName[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_DEFAULTDOCUMENTNAME, szDefaultDocumentName, sizeof(szDefaultDocumentName) / sizeof(char));

	::strcat_s(szFileName, szDefaultDocumentName);
	::strcat_s(szFileName, "[");

	const char* szDocumentNumber = toChar((long)m_nDocumentCounter++);
	::strcat_s(szFileName, szDocumentNumber);

	::strcat_s(szFileName, "]");

	char szDefaultFileType[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_DEFAULTFILETYPE, szDefaultFileType, sizeof(szDefaultFileType) / sizeof(char));

	::strcat_s(szFileName, szDefaultFileType);

	createDocument(szFileName);
}
void Application::wm_command_File_Open()
{
	char szFileName[MAX_LOADSTRING]{ 0 };

	char szTitle[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_DEFAULTOPENFILETITLE, szTitle, sizeof(szTitle) / sizeof(char));

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFileName;
	ofn.lpstrFile[0] = '\0'; //Set lpstrFile[0] to '\0' so that GetOpenFileName does not use the contents of szFile to initialize itself.
	ofn.nMaxFile = sizeof(szFileName);
	ofn.lpstrFilter = m_szFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrTitle = szTitle;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (::GetOpenFileName(&ofn) == TRUE) {
		createDocument(ofn.lpstrFile);
	}
	else {
		ErrorHandler();
	}
}
void Application::wm_command_File_Import() {}
void Application::wm_command_File_Close() { wm_command_Window_Close(); }

void Application::wm_command_View_Display_UCSIcon_On()
{
	BOOL nResult = FALSE;

	Document* lpDocument = (Document*)m_lpDocumentManager->getActiveObject();
	if (lpDocument == nullptr) return;
	if (lpDocument->getLayoutManager() == nullptr) return;
	if (lpDocument->getLayoutManager()->getSize() == 0) return;

	Layout* lpActiveLayout = lpDocument->getLayoutManager()->getActiveObject();
	if (lpActiveLayout == nullptr) return;

	HMENU hMenu = ::GetMenu(m_hWnd);
	HMENU hSubMenu = ::GetSubMenu(hMenu, 2);
	HMENU hSubSubMenu = ::GetSubMenu(hSubMenu, 20);
	HMENU hSubSubSubMenu = ::GetSubMenu(hSubSubMenu, 0);

	MENUITEMINFO mii{ 0 };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;

	BOOL nVisible = lpActiveLayout->getActiveViewport()->getUCSVisible();
	if (nVisible == FALSE) {
		lpActiveLayout->getActiveViewport()->setUCSVisible(TRUE);
		mii.fState = MFS_CHECKED;
	}
	else {
		lpActiveLayout->getActiveViewport()->setUCSVisible(FALSE);
		mii.fState = MFS_UNCHECKED;
	}

	nResult = ::SetMenuItemInfo(hSubSubSubMenu, ID_VIEW_DISPLAY_UCSICON_VISIBILITY, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	nResult = DrawMenuBar(m_hWnd);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	::SendMessage(lpDocument->getHWND(), WM_PAINT, 0, 0);
}
void Application::wm_command_View_Display_UCSIcon_Position()
{
	BOOL nResult = FALSE;

	Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
	if (lpActiveDocument == nullptr) return;
	if (lpActiveDocument->getLayoutManager() == nullptr) return;
	if (lpActiveDocument->getLayoutManager()->getSize() == 0) return;

	Layout* lpActiveLayout = lpActiveDocument->getLayoutManager()->getActiveObject();
	if (lpActiveLayout == nullptr) return;

	HMENU hMenu = ::GetMenu(m_hWnd);
	HMENU hSubMenu = ::GetSubMenu(hMenu, 2);
	HMENU hSubSubMenu = ::GetSubMenu(hSubMenu, 20);
	HMENU hSubSubSubMenu = ::GetSubMenu(hSubSubMenu, 0);

	MENUITEMINFO mii{ 0 };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;

	BOOL nOrigin = lpActiveLayout->getActiveViewport()->getUCSOrigin();
	if (nOrigin == FALSE) {
		lpActiveLayout->getActiveViewport()->setUCSOrigin(TRUE);
		mii.fState = MFS_CHECKED;
	}
	else {
		lpActiveLayout->getActiveViewport()->setUCSOrigin(FALSE);
		mii.fState = MFS_UNCHECKED;
	}

	nResult = ::SetMenuItemInfo(hSubSubSubMenu, ID_VIEW_DISPLAY_UCSICON_ORIGIN, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	nResult = DrawMenuBar(m_hWnd);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	::SendMessage(lpActiveDocument->getHWND(), WM_PAINT, 0, 0);
}

void Application::wm_command_Insert_NewLayout()
{
	Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();

	if (lpActiveDocument == nullptr) return;
	if (lpActiveDocument->getLayoutManager() == nullptr) return;
	if (lpActiveDocument->getLayoutManager()->getSize() == 0) return;

	char szLayout[8];
	int nIndex = lpActiveDocument->getLayoutManager()->getSize();
	::sprintf_s(szLayout, "%d", nIndex);

	char szLayoutName[16] = "Layout";
	::strcat_s(szLayoutName, 16, szLayout);
	::strcat_s(szLayoutName, 16, "]");

	lpActiveDocument->getLayoutManager()->addObject(nIndex, szLayoutName);
	lpActiveDocument->getLayoutManager()->setActiveLayout(nIndex);
}

void Application::wm_command_Window_Close()
{
	int nItemCount = (int)::SendMessage(m_hTabcontrol_Documents, TCM_GETITEMCOUNT, 0, 0);
	int itemIndex = ::SendMessage(m_hTabcontrol_Documents, TCM_GETCURSEL, 0, 0);

	Document* lpDocument = (Document*)m_lpDocumentManager->getActiveObject();

	if (lpDocument == nullptr) {
		return;
	}

	::SendMessage(lpDocument->getHWND(), WM_CLOSE, 0, 0);
	::SendMessage(m_hTabcontrol_Documents, TCM_DELETEITEM, (WPARAM)itemIndex, 0);

	delete lpDocument;
	lpDocument = NULL;

	if (nItemCount > 1) {
		if (itemIndex == nItemCount - 1) {
			int nItemSum = nItemCount - 2;
			::SendMessage(m_hTabcontrol_Documents, TCM_SETCURSEL, (WPARAM)nItemSum, 0);
		}
		else {
			::SendMessage(m_hTabcontrol_Documents, TCM_SETCURSEL, (WPARAM)itemIndex, 0);
		}

		TCITEM tcItem{ TCIF_PARAM };
		itemIndex = ::SendMessage(m_hTabcontrol_Documents, TCM_GETCURSEL, 0, 0);
		::SendMessage(m_hTabcontrol_Documents, TCM_GETITEM, (WPARAM)itemIndex, (LPARAM)&tcItem);
		lpDocument = (Document*)tcItem.lParam;
		if (lpDocument != nullptr) {
			m_lpDocumentManager->setActiveObject(lpDocument);
			HWND hWnd = lpDocument->getHWND();
			::SetWindowPos(hWnd, NULL, m_ptDocument.x, m_ptDocument.y, m_szDocument.cx, m_szDocument.cy, SWP_SHOWWINDOW);
			::SendMessage(hWnd, WM_ACTIVATE, MAKEWPARAM(WA_ACTIVE, 0), (LPARAM)hWnd);
			::SetFocus(hWnd);
		}
	}
	else {
		m_lpDocumentManager->setActiveObject(nullptr);
	}

	wm_notify_tcn_selchange_documents_callback();
}
void Application::wm_command_Window_CloseAll()
{
	m_lpDocumentManager->setActiveObject(nullptr);

	::SendMessage(m_hTabcontrol_Documents, TCM_DELETEALLITEMS, 0, 0);

	char szClassName[MAX_LOADSTRING]{ 0 };
	::LoadStringA(s_hInstance, ID_DOCUMENT_CLASS, szClassName, sizeof(szClassName) / sizeof(char));

	MSG mMsg;
	mMsg.message = WM_CLOSE;
	mMsg.lParam = (LPARAM)szClassName;

	::EnumChildWindows(m_hWnd, s_bfnEnumChildWindowsProc, (LPARAM)&mMsg);//EnumChildWindows doesn't return a value.
}
void Application::wm_command_Window_Cascade()
{

}
void Application::wm_command_Window_TileHorizontally()
{

}
void Application::wm_command_Window_TileVertically()
{

}
void Application::wm_command_Window_ArrangeIcons()
{

}
void Application::wm_command_Window_Windows()
{
	DialogBox(s_hInstance, MAKEINTRESOURCE(IDD_WINDOW_WINDOWS), m_hWnd, (DLGPROC)s_nfnWindowsProc);
}

int Application::wm_command_Default(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	log("Log::Application::wm_command() default");
	//return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	return 0;
}

void Application::wm_create_Menu()
{
	m_hMenu = ::CreateMenu();

	HMENU hFile = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_FILE));
	HMENU hWindow = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_WINDOW));
	HMENU hHelp = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_HELP));

	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hFile, "&File");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hWindow, "&Window");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hHelp, "&Help");
}
void Application::wm_create_toolbar_standard()
{
	BOOL nResult = FALSE;
	HRESULT hResult = S_OK;

	m_hToolbar_File = ::CreateWindowEx(
		WS_EX_NOPARENTNOTIFY,
		TOOLBARCLASSNAME,
		NULL,
		WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_TOOLTIPS | CCS_NOPARENTALIGN | CCS_NORESIZE | TBSTYLE_FLAT | CCS_ADJUSTABLE,
		0,
		0,
		0,
		0,
		m_hWnd,
		(HMENU)IDC_TOOLBAR_FILE,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hToolbar_File == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {

#pragma warning( push )
#pragma warning( disable : 6387)
		::SendMessage(m_hToolbar_File, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
#pragma warning( pop )

		TBADDBITMAP tbab;
		tbab.hInst = HINST_COMMCTRL;
		tbab.nID = IDB_STD_SMALL_COLOR;

		int nIndex = ::SendMessage(m_hToolbar_File, TB_ADDBITMAP, 0, (LPARAM)&tbab);

		if (nIndex == -1) {
			hResult = ErrorHandler();
		}
	}

	if (SUCCEEDED(hResult)) {
		TBBUTTON tbb[3];

		ZeroMemory(tbb, sizeof(tbb));

		tbb[0].iBitmap = STD_FILENEW;
		tbb[0].fsState = TBSTATE_ENABLED;
		tbb[0].fsStyle = TBSTYLE_BUTTON;
		tbb[0].idCommand = IDC_TOOLBAR_FILE_NEW;

		tbb[1].iBitmap = STD_FILEOPEN;
		tbb[1].fsState = TBSTATE_ENABLED;
		tbb[1].fsStyle = TBSTYLE_BUTTON;
		tbb[1].idCommand = IDC_TOOLBAR_FILE_OPEN;

		tbb[2].iBitmap = STD_FILESAVE;
		tbb[2].fsState = TBSTATE_ENABLED;
		tbb[2].fsStyle = TBSTYLE_BUTTON;
		tbb[2].idCommand = IDC_TOOLBAR_FILE_SAVE;

		nResult = (BOOL)::SendMessage(m_hToolbar_File, TB_ADDBUTTONS, (WPARAM)(sizeof(tbb) / sizeof(TBBUTTON)), (LPARAM)&tbb);
		if (nResult == FALSE) {
			hResult = ErrorHandler();
		}
	}

	if (SUCCEEDED(hResult)) {
		::SendMessage(m_hToolbar_File, TB_AUTOSIZE, 0, 0);

#pragma warning( push )
#pragma warning( disable : 6387)
		::ShowWindow(m_hToolbar_File, SW_NORMAL);
#pragma warning( pop )

	}
}
void Application::wm_create_toolbar_draw()
{
	BOOL nResult = FALSE;
	HRESULT hResult = S_OK;

	m_hToolbar_Draw = ::CreateWindowEx(
		WS_EX_NOPARENTNOTIFY,
		TOOLBARCLASSNAME,
		NULL,
		WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_TOOLTIPS | CCS_NOPARENTALIGN | CCS_NORESIZE | TBSTYLE_FLAT | CCS_ADJUSTABLE,
		0,
		0,
		0,
		0,
		m_hWnd,
		(HMENU)IDC_TOOLBAR_DRAW,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hToolbar_Draw == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)
		::SendMessage(m_hToolbar_Draw, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
#pragma warning( pop )

		TBADDBITMAP tbab;
		tbab.hInst = HINST_COMMCTRL;
		tbab.nID = IDB_STD_SMALL_COLOR;

		INT nIndex = (INT)::SendMessage(m_hToolbar_Draw, TB_ADDBITMAP, 0, (LPARAM)&tbab);

		if (nIndex == -1) {
			hResult = ErrorHandler();
		}
	}

	HIMAGELIST hImageList{ 0 };
	if (SUCCEEDED(hResult)) {
		hImageList = ImageList_Create(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 | ILC_MASK, 12, 0);
		if (hImageList == NULL) {
			hResult = ErrorHandler();
		}
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		HINSTANCE hModule = ::LoadLibrary("mycaddata.dll");
		if (hModule != NULL) {
			HICON hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_LINE));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_RAY));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_POLYLINE));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_POLYGON));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_RECTANGLE));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_ARC));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_CIRCLE));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_REVISION_CLOUD));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_SPLINE));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_ELLIPSE));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_ELLIPSE_ARC));
			ImageList_AddIcon(hImageList, hIcon);

			hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_DRAW_POINT_SINGLE));
			ImageList_AddIcon(hImageList, hIcon);

			if (::SendMessage(m_hToolbar_Draw, TB_SETIMAGELIST, 0, (LPARAM)hImageList) == NULL) {
				hResult = ErrorHandler();
			}
			else {
				::SendMessage(m_hToolbar_Draw, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
				::SendMessage(m_hToolbar_Draw, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_DRAWDDARROWS);
			}

			//ImageList_Destroy(hImageList);
			//DestroyIcon(hIcon);

			::FreeLibrary(hModule);
		}

#pragma warning( pop )
	}

	if (SUCCEEDED(hResult)) {
		TBBUTTON tbb[13];
		ZeroMemory(tbb, sizeof(tbb));

		tbb[0].iBitmap = 0;
		tbb[0].fsState = TBSTATE_ENABLED;
		tbb[0].fsStyle = TBSTYLE_BUTTON;
		tbb[0].idCommand = IDC_TOOLBAR_DRAW_LINE;

		tbb[1].iBitmap = 1;
		tbb[1].fsState = TBSTATE_ENABLED;
		tbb[1].fsStyle = TBSTYLE_BUTTON;
		tbb[1].idCommand = IDC_TOOLBAR_DRAW_RAY;

		tbb[2].iBitmap = 2;
		tbb[2].fsState = TBSTATE_ENABLED;
		tbb[2].fsStyle = TBSTYLE_BUTTON;
		tbb[2].idCommand = IDC_TOOLBAR_DRAW_POLYLINE;

		tbb[3].iBitmap = 3;
		tbb[3].fsState = TBSTATE_ENABLED;
		tbb[3].fsStyle = TBSTYLE_BUTTON;
		tbb[3].idCommand = IDC_TOOLBAR_DRAW_POLYGON;

		tbb[4].iBitmap = 4;
		tbb[4].fsState = TBSTATE_ENABLED;
		tbb[4].fsStyle = TBSTYLE_BUTTON;
		tbb[4].idCommand = IDC_TOOLBAR_DRAW_RECTANGLE;

		tbb[5].fsStyle = TBSTYLE_SEP;

		tbb[6].iBitmap = 5;
		tbb[6].fsState = TBSTATE_ENABLED;
		tbb[6].fsStyle = TBSTYLE_BUTTON | TBSTYLE_DROPDOWN;
		tbb[6].idCommand = IDC_TOOLBAR_DRAW_ARC;

		tbb[7].iBitmap = 6;
		tbb[7].fsState = TBSTATE_ENABLED;
		tbb[7].fsStyle = TBSTYLE_BUTTON | TBSTYLE_DROPDOWN;
		tbb[7].idCommand = IDC_TOOLBAR_DRAW_CIRCLE;

		tbb[8].iBitmap = 7;
		tbb[8].fsState = TBSTATE_ENABLED;
		tbb[8].fsStyle = TBSTYLE_BUTTON;
		tbb[8].idCommand = IDC_TOOLBAR_DRAW_REVISION_CLOUD;

		tbb[9].iBitmap = 8;
		tbb[9].fsState = TBSTATE_ENABLED;
		tbb[9].fsStyle = TBSTYLE_BUTTON;
		tbb[9].idCommand = IDC_TOOLBAR_DRAW_SPLINE;

		tbb[10].iBitmap = 9;
		tbb[10].fsState = TBSTATE_ENABLED;
		tbb[10].fsStyle = TBSTYLE_BUTTON | TBSTYLE_DROPDOWN;
		tbb[10].idCommand = IDC_TOOLBAR_DRAW_ELLIPSE;

		tbb[11].iBitmap = 10;
		tbb[11].fsState = TBSTATE_ENABLED;
		tbb[11].fsStyle = TBSTYLE_BUTTON;
		tbb[11].idCommand = IDC_TOOLBAR_DRAW_ELLIPSE_ARC;

		tbb[12].iBitmap = 11;
		tbb[12].fsState = TBSTATE_ENABLED;
		tbb[12].fsStyle = TBSTYLE_BUTTON;
		tbb[12].idCommand = IDC_TOOLBAR_DRAW_POINT_SINGLE;

		nResult = (BOOL)::SendMessage(m_hToolbar_Draw, TB_ADDBUTTONS, (WPARAM)(sizeof(tbb) / sizeof(TBBUTTON)), (LPARAM)&tbb);
		if (nResult == FALSE) {
			hResult = ErrorHandler();
		}
	}

	if (SUCCEEDED(hResult)) {
		::SendMessage(m_hToolbar_Draw, TB_AUTOSIZE, 0, 0);

#pragma warning( push )
#pragma warning( disable : 6387)
		::ShowWindow(m_hToolbar_Draw, SW_NORMAL);
#pragma warning( pop )

	}
}
void Application::wm_create_rebar_standard()
{
	BOOL nResult = FALSE;
	HRESULT hResult = S_OK;

	m_hRebar_Standard = ::CreateWindowEx(
		WS_EX_NOPARENTNOTIFY,
		REBARCLASSNAME,
		NULL,
		WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN | RBS_VARHEIGHT,
		0,
		0,
		0,
		0,
		m_hWnd,
		(HMENU)IDC_REBAR,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hRebar_Standard == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
		setFont(m_hRebar_Standard);
	}

	REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };
	rbBand.fMask = RBBIM_COLORS | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID;
	rbBand.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbBand.clrBack = GetSysColor(COLOR_BTNFACE);

	if (SUCCEEDED(hResult)) {
		DWORD dwBtnSize = (DWORD)::SendMessage(m_hToolbar_File, TB_GETBUTTONSIZE, 0, 0);

		rbBand.fStyle = RBBS_NOVERT;
		rbBand.hwndChild = m_hToolbar_File;
		rbBand.wID = IDC_TOOLBAR_FILE;
		rbBand.cyChild = LOWORD(dwBtnSize);
		rbBand.cxMinChild = 70;
		rbBand.cyMinChild = LOWORD(dwBtnSize);
		rbBand.cx = 70;

#pragma warning( push )
#pragma warning( disable : 6387)
		nResult = (BOOL)::SendMessage(m_hRebar_Standard, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
#pragma warning( pop )

		if (nResult == FALSE) {
			hResult = ErrorHandler();
		}
	}

	if (SUCCEEDED(hResult)) {
		DWORD dwBtnSize = (DWORD)::SendMessage(m_hToolbar_Draw, TB_GETBUTTONSIZE, 0, 0);

		rbBand.fStyle = RBBS_NOVERT | RBBS_BREAK;
		rbBand.hwndChild = m_hToolbar_Draw;
		rbBand.wID = IDC_TOOLBAR_DRAW;
		rbBand.cyChild = LOWORD(dwBtnSize);
		rbBand.cxMinChild = 190;
		rbBand.cyMinChild = LOWORD(dwBtnSize);
		rbBand.cx = 200;

#pragma warning( push )
#pragma warning( disable : 6387)
		nResult = (BOOL)::SendMessage(m_hRebar_Standard, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
#pragma warning( pop )

		if (nResult == FALSE) {
			hResult = ErrorHandler();
		}
	}
}
void Application::wm_create_tabcontrol_documents()
{
	RECT rcRebar;
	GetClientRect(m_hRebar_Standard, &rcRebar);

	m_hTabcontrol_Documents = ::CreateWindowEx(
		WS_EX_NOPARENTNOTIFY,
		WC_TABCONTROL,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_TOOLTIPS,
		3,
		rcRebar.bottom + 3,
		m_cx - 6,
		s_nTabconctrol_Documents_Height,
		m_hWnd,
		(HMENU)IDC_TABCTRL_DOCUMENTS,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hTabcontrol_Documents == NULL) {
		ErrorHandler();
	}

	setFont(m_hTabcontrol_Documents);

	m_hTabcontrol_Documents_Tooltip = (HWND)::SendMessage(m_hTabcontrol_Documents, TCM_GETTOOLTIPS, 0, 0);
	if (m_hTabcontrol_Documents_Tooltip == NULL) {
		ErrorHandler();
	}
}
void Application::wm_create_statusbar_tooltips()
{
	char szTooltipTitle[STATUSBARPANELCOUNT][MAX_TTM_SETTITLE];
	char szTooltipText[STATUSBARPANELCOUNT][MAX_LOADSTRING];

	::LoadString(s_hInstance, ID_STATUSBAR_COORDINATES_TITLE, szTooltipTitle[0], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_COORDINATES_TEXT, szTooltipText[0], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_MODELSPACE_TEXT, szTooltipText[1], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_GRID_TITLE, szTooltipTitle[2], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_GRID_TEXT, szTooltipText[2], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_SNAPMODE_TITLE, szTooltipTitle[3], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_SNAPMODE_TEXT, szTooltipText[3], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_INFERCONSTRAINTS_TITLE, szTooltipTitle[4], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_INFERCONSTRAINTS_TEXT, szTooltipText[4], MAX_LOADSTRING);

	if (DynamicInput::s_nVisible == TRUE) {
		::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_ON_TITLE, szTooltipTitle[5], MAX_TTM_SETTITLE);
	}
	else {
		::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_OFF_TITLE, szTooltipTitle[5], MAX_TTM_SETTITLE);
	}

	::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICINPUT_TEXT, szTooltipText[5], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_ORTHOMODE_TITLE, szTooltipTitle[6], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_ORTHOMODE_TEXT, szTooltipText[6], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_POLARTRACKING_TITLE, szTooltipTitle[7], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_POLARTRACKING_TEXT, szTooltipText[7], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_ISOMETRICDRAFTING_TITLE, szTooltipTitle[8], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_ISOMETRICDRAFTING_TEXT, szTooltipText[8], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_OBJECTSNAPTRACKING_TITLE, szTooltipTitle[9], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_OBJECTSNAPTRACKING_TEXT, szTooltipText[9], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_2DOBJECTSNAP_TITLE, szTooltipTitle[10], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_2DOBJECTSNAP_TEXT, szTooltipText[10], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_LINEWEIGHT_TITLE, szTooltipTitle[11], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_LINEWEIGHT_TEXT, szTooltipText[11], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_TRANSPARENCY_TITLE, szTooltipTitle[12], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_TRANSPARENCY_TEXT, szTooltipText[12], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_SELECTIONCYCLING_TITLE, szTooltipTitle[13], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_SELECTIONCYCLING_TEXT, szTooltipText[13], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_3DOBJECTSNAP_TITLE, szTooltipTitle[14], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_3DOBJECTSNAP_TEXT, szTooltipText[14], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICUCS_TITLE, szTooltipTitle[15], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_DYNAMICUCS_TEXT, szTooltipText[15], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_SELECTIONFILTERING_TITLE, szTooltipTitle[16], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_SELECTIONFILTERING_TEXT, szTooltipText[16], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_GIZMO_TEXT, szTooltipText[17], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_ANNOTATIONVISIBILITY_TEXT, szTooltipText[18], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_AUTOSCALE_TEXT, szTooltipText[19], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_ANNOTATIONSCALE_TITLE, szTooltipTitle[20], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_ANNOTATIONSCALE_TEXT, szTooltipText[20], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_WORKSPACESWITCHING_TITLE, szTooltipTitle[21], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_WORKSPACESWITCHING_TEXT, szTooltipText[21], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_ANNOTATIONMONITIOR_TITLE, szTooltipTitle[22], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_ANNOTATIONMONITIOR_TEXT, szTooltipText[22], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_UNITS_TITLE, szTooltipTitle[23], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_UNITS_TEXT, szTooltipText[23], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_QUICKPROPERTIES_TITLE, szTooltipTitle[24], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_QUICKPROPERTIES_TEXT, szTooltipText[24], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_LOCKUI_TITLE, szTooltipTitle[25], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_LOCKUI_TEXT, szTooltipText[25], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_ISOLATEOBJECTS_TEXT, szTooltipText[26], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_GRAPHICSPERFORMANCE_TITLE, szTooltipTitle[27], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_GRAPHICSPERFORMANCE_TEXT, szTooltipText[27], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_CLEANSCREEN_TITLE, szTooltipTitle[28], MAX_TTM_SETTITLE);
	::LoadString(s_hInstance, ID_STATUSBAR_CLEANSCREEN_TEXT, szTooltipText[28], MAX_LOADSTRING);

	::LoadString(s_hInstance, ID_STATUSBAR_CUSTOMIZATION_TEXT, szTooltipText[29], MAX_LOADSTRING);

	m_lpStatusbar->setTooltip(0, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_COORDINATES, szTooltipTitle[0], szTooltipText[0]);
	m_lpStatusbar->setTooltip(1, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_MODELSPACE, szTooltipText[1]);
	m_lpStatusbar->setTooltip(2, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_GRID, szTooltipTitle[2], szTooltipText[2]);
	m_lpStatusbar->setTooltip(3, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_SNAPMODE, szTooltipTitle[3], szTooltipText[3]);
	m_lpStatusbar->setTooltip(4, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_INFERCONSTRAINTS, szTooltipTitle[4], szTooltipText[4]);
	m_lpStatusbar->setTooltip(5, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_DYNAMICINPUT, szTooltipTitle[5], szTooltipText[5]);
	m_lpStatusbar->setTooltip(6, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_ORTHOMODE, szTooltipTitle[6], szTooltipText[6]);
	m_lpStatusbar->setTooltip(7, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_POLARTRACKING, szTooltipTitle[7], szTooltipText[7]);
	m_lpStatusbar->setTooltip(8, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_ISOMETRICDRAFTING, szTooltipTitle[8], szTooltipText[8]);
	m_lpStatusbar->setTooltip(9, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_OBJECTSNAPTRACKKING, szTooltipTitle[9], szTooltipText[9]);
	m_lpStatusbar->setTooltip(10, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_2DOBJECTSNAP, szTooltipTitle[10], szTooltipText[10]);
	m_lpStatusbar->setTooltip(11, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_LINEWEIGHT, szTooltipTitle[11], szTooltipText[11]);
	m_lpStatusbar->setTooltip(12, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_TRANSPARENCY, szTooltipTitle[12], szTooltipText[12]);
	m_lpStatusbar->setTooltip(13, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_SELECTIONCYCLING, szTooltipTitle[13], szTooltipText[13]);
	m_lpStatusbar->setTooltip(14, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_3DOBJECTSNAP, szTooltipTitle[14], szTooltipText[14]);
	m_lpStatusbar->setTooltip(15, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_DYNAMICUCS, szTooltipTitle[15], szTooltipText[15]);
	m_lpStatusbar->setTooltip(16, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_SELECTIONFILTERING, szTooltipTitle[16], szTooltipText[16]);
	m_lpStatusbar->setTooltip(17, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_GIZMO, szTooltipText[17]);
	m_lpStatusbar->setTooltip(18, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_ANNOTATIONVISIBILITY, szTooltipText[18]);
	m_lpStatusbar->setTooltip(19, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_AUTOSCALE, szTooltipText[19]);
	m_lpStatusbar->setTooltip(20, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_ANNOTATIONSCALE, szTooltipTitle[20], szTooltipText[20]);
	m_lpStatusbar->setTooltip(21, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_WORKSPACESWITCHING, szTooltipTitle[21], szTooltipText[21]);
	m_lpStatusbar->setTooltip(22, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_ANNOTATIONMONITIOR, szTooltipTitle[22], szTooltipText[22]);
	m_lpStatusbar->setTooltip(23, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_UNITS, szTooltipTitle[23], szTooltipText[23]);
	m_lpStatusbar->setTooltip(24, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_QUICKPROPERTIES, szTooltipTitle[24], szTooltipText[24]);
	m_lpStatusbar->setTooltip(25, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_LOCKUI, szTooltipTitle[25], szTooltipText[25]);
	m_lpStatusbar->setTooltip(26, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_ISOLATEOBJECTS, szTooltipText[26]);
	m_lpStatusbar->setTooltip(27, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_GRAPHICSPERFORMANCE, szTooltipTitle[27], szTooltipText[27]);
	m_lpStatusbar->setTooltip(28, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_CLEANSCREEN, szTooltipTitle[28], szTooltipText[28]);
	m_lpStatusbar->setTooltip(29, TTS_NOPREFIX | TTS_ALWAYSTIP, TTF_SUBCLASS | TTF_CENTERTIP, IDC_STATUSBAR_CUSTOMIZATION, szTooltipText[29]);
}
void Application::wm_create_statusbar()
{
	m_lpStatusbar = new Statusbar(WS_CHILD | WS_VISIBLE | CCS_BOTTOM, m_hWnd, IDC_STATUSBAR, s_hInstance);

	std::array<int, STATUSBARPANELCOUNT>nPanelWidths = { 190,50,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,0 };
	m_lpStatusbar->setParts(nPanelWidths);

	m_lpStatusbar->setPanelVisibility(4, FALSE);
	m_lpStatusbar->setPanelVisibility(11, FALSE);
	m_lpStatusbar->setPanelVisibility(12, FALSE);
	m_lpStatusbar->setPanelVisibility(13, FALSE);
	m_lpStatusbar->setPanelVisibility(14, FALSE);
	m_lpStatusbar->setPanelVisibility(15, FALSE);
	m_lpStatusbar->setPanelVisibility(16, FALSE);
	m_lpStatusbar->setPanelVisibility(17, FALSE);
	m_lpStatusbar->setPanelVisibility(23, FALSE);
	m_lpStatusbar->setPanelVisibility(24, FALSE);
	m_lpStatusbar->setPanelVisibility(25, FALSE);
	m_lpStatusbar->setPanelVisibility(27, FALSE);

	wm_create_statusbar_tooltips();

	wm_size_statusbar();
	wm_exitsizemove_statusbar_tooltips();
}

void Application::wm_exitsizemove_statusbar_tooltips()
{
	if (m_lpStatusbar == nullptr) return;
	m_lpStatusbar->wm_exitsizemove();
}

void Application::wm_initmenupopup_View(HMENU hmenu)
{
	BOOL nResult = FALSE;

	MENUITEMINFO mii{ 0 };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_BITMAP;

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_TOP), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_TOP, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_BOTTOM), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_BOTTOM, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_LEFT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_LEFT, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_RIGHT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_RIGHT, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_FRONT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_FRONT, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_BACK), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_BACK, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_SWISOMETRIC), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_SWISOMETRIC, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_SEISOMETRIC), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_SEISOMETRIC, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_NEISOMETRIC), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_NEISOMETRIC, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	mii.hbmpItem = (HBITMAP)::LoadImage(s_hInstance, MAKEINTRESOURCE(IDB_VIEW_3DVIEWS_NWISOMETRIC), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);;
	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_3DVIEWS_NWISOMETRIC, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}


	Document* lpDocument = m_lpDocumentManager->getActiveObject();
	if (lpDocument == nullptr) {
		ErrorHandler();
		return;
	}
	if (lpDocument->getLayoutManager() == nullptr) {
		ErrorHandler();
		return;
	}
	if (lpDocument->getLayoutManager()->getSize() == 0) {
		ErrorHandler();
		return;
	}

	Layout* lpActiveLayout = lpDocument->getLayoutManager()->getActiveObject();
	if (lpActiveLayout == nullptr) {
		ErrorHandler();
		return;
	}

	mii.fMask = MIIM_STATE;

	BOOL nVisible = lpActiveLayout->getActiveViewport()->getUCSVisible();
	if (nVisible == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}

	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_DISPLAY_UCSICON_VISIBILITY, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	BOOL nOrigin = lpActiveLayout->getActiveViewport()->getUCSOrigin();
	if (nOrigin == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}

	nResult = ::SetMenuItemInfo(hmenu, ID_VIEW_DISPLAY_UCSICON_ORIGIN, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}
}

void Application::wm_notify_nm_click_toolbar_File(DWORD_PTR dwItemSpec)
{
	switch (dwItemSpec) {
		case IDC_TOOLBAR_FILE_NEW: {
			log("Log::Application::wm_notify_nm_click_toolbar_File() IDC_TOOLBAR_FILE_NEW");
			wm_command_File_New();
			break;
		}
		case IDC_TOOLBAR_FILE_OPEN: {
			log("Log::Application::wm_notify_nm_click_toolbar_File() IDC_TOOLBAR_FILE_OPEN");
			wm_command_File_Open();
			break;
		}
		case IDC_TOOLBAR_FILE_SAVE: {
			log("Log::Application::wm_notify_nm_click_toolbar_File() IDC_TOOLBAR_FILE_SAVE");
			break;
		}
		default: {
			break;
		}
	}
}
void Application::wm_notify_nm_click_toolbar_Draw(DWORD_PTR dwItemSpec)
{
	Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
	HWND hWnd = NULL;
	if (lpActiveDocument != nullptr) {
		hWnd = lpActiveDocument->getHWND();
	}

	switch (dwItemSpec) {
		case IDC_TOOLBAR_DRAW_POINT_SINGLE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_POINT_SINGLE");

			break;
		}
		case IDC_TOOLBAR_DRAW_POINT_MULTIPLE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_POINT_MULTIPLE");

			break;
		}
		case IDC_TOOLBAR_DRAW_POINT_DIVIDE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_POINT_DIVIDE");
			break;
		}
		case IDC_TOOLBAR_DRAW_POINT_MEASURE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_POINT_MEASURE");
			break;
		}
		case IDC_TOOLBAR_DRAW_LINE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_LINE");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_LINE);
			break;
		}
		case IDC_TOOLBAR_DRAW_RAY: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_RAY");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_RAY);
			break;
		}
		case IDC_TOOLBAR_DRAW_XLINE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_XLINE");
			SendCommandParam(hWnd, IDC_APPLICATION, IDC_XLINE);
			break;
		}
		case IDC_TOOLBAR_DRAW_MULTILINE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_MULTILINE");
			break;
		}
		case IDC_TOOLBAR_DRAW_POLYLINE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_POLYLINE");
			break;
		}
		case IDC_TOOLBAR_DRAW_3DPOLYLINE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_3DPOLYLINE");
			break;
		}
		case IDC_TOOLBAR_DRAW_POLYGON: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_POLYGON");
			break;
		}
		case IDC_TOOLBAR_DRAW_RECTANGLE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_RECTANGLE");
			break;
		}
		case IDC_TOOLBAR_DRAW_HELIX: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_HELIX");
			break;
		}
		case IDC_TOOLBAR_DRAW_ARC: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_ARC");
			break;
		}
		case IDC_TOOLBAR_DRAW_ARC_CENTRE_START_END: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_ARC_CENTRE_START_END");
			break;
		}
		case IDC_TOOLBAR_DRAW_ARC_THREE_POINTS: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_ARC_THREE_POINTS");
			break;
		}
		case IDC_TOOLBAR_DRAW_CIRCLE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_CIRCLE");
			break;
		}
		case IDC_TOOLBAR_DRAW_CIRCLE_CENTRE_RADIUS: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_CIRCLE_CENTRE_RADIUS");
			break;
		}
		case IDC_TOOLBAR_DRAW_CIRCLE_THREE_POINTS: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_CIRCLE_THREE_POINTS");
			break;
		}
		case IDC_TOOLBAR_DRAW_SPLINE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_SPLINE");
			break;
		}
		case IDC_TOOLBAR_DRAW_ELLIPSE: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_ELLIPSE");
			break;
		}
		case IDC_TOOLBAR_DRAW_ELLIPSE_CENTRE_AXES: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_ELLIPSE_CENTRE_AXES");
			break;
		}
		case IDC_TOOLBAR_DRAW_ELLIPSE_FOUR_POINTS: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_ELLIPSE_FOUR_POINTS");
			break;
		}
		case IDC_TOOLBAR_DRAW_ELLIPSE_ARC: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_ELLIPSE_ARC");
			break;
		}
		case IDC_TOOLBAR_DRAW_REVISION_CLOUD: {
			log("Log::Application::wm_notify_nm_click_toolbar_Draw() IDC_TOOLBAR_DRAW_REVISION_CLOUD");
			break;
		}
		default: {
			break;
		}
	}
}

void Application::wm_notify_nm_click_statusbar(DWORD_PTR dwItemSpec)
{
	log("Log::Application::wm_notify_nm_click_statusbar()");

	switch (dwItemSpec) {
		case 0: {

			break;
		}
		case 1: {

			break;
		}
		case 2: {
			Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();//Get active document,...
			if (lpActiveDocument == nullptr) break;
			if (lpActiveDocument->getLayoutManager() == nullptr) break;
			if (lpActiveDocument->getLayoutManager()->getSize() == 0) break;

			Layout* lpActiveLayout = lpActiveDocument->getLayoutManager()->getActiveObject();//...then get active layout.
			if (lpActiveLayout == nullptr) break;

			if (lpActiveDocument->getLayoutManager()->setTilemode(0)) {//If TILEMODE = 1 (Modellayout), change TILEMODE to 0 (Paperlayouts),...
				if (lpActiveDocument->getLayoutManager()->getActiveObject() != nullptr) {
					//int nIndex = lpActiveDocument->getLayoutManager()->getActiveObject()->getIndex();//...get active Paperlayout index...
					//::SendMessage(m_hTabcontrol_Layouts, TCM_SETCURSEL, (WPARAM)nIndex, 0);//...and set layouts tab control by index.
				}
			}
			else {//If TILEMODE = 0 (Paperlayouts), flip active layout space context.
				if (lpActiveDocument->getLayoutManager()->getActiveObject() != nullptr) {
					if (lpActiveLayout->getActiveSpace()->getContext() == Space::Context::ModelSpace) {
						lpActiveLayout->setActiveSpace(Space::Context::PaperSpace);
					}
					else if (lpActiveLayout->getActiveSpace()->getContext() == Space::Context::PaperSpace) {
						lpActiveLayout->setActiveSpace(Space::Context::ModelSpace);
					}
				}
			}

			lpActiveLayout = lpActiveDocument->getLayoutManager()->getActiveObject();//Get active layout, (again; it may have changed),...
			if (lpActiveLayout == nullptr) break;

			::SendMessage(lpActiveDocument->getHWND(), WM_PAINT, 0, 0);//...paint active document,...

			lpActiveDocument->setStatusbarText(m_lpStatusbar->getHWND(), 2);//Update statusbar text for panel[2].
			lpActiveDocument->setDynamicInputStyle();//Update dynamic input style.

			break;
		}
		case 3: {

			break;
		}
		case 4: {

			break;
		}
		case 5: {

			break;
		}
		case 6: {
			int nVal = CommandWindow::getSystemVariable(IDC_DYNMODE);
			nVal = -1 * nVal;
			setSystemVariable(IDC_DYNMODE, nVal);
			break;
		}
		case 7: {

			break;
		}
		case 8: {

			break;
		}
		case 9: {

			break;
		}
		case 10: {

			break;
		}
		case 11: {

			break;
		}
		case 12: {

			break;
		}
		case 13: {

			break;
		}
		case 14: {

			break;
		}
		case 15: {

			break;
		}
		case 16: {

			break;
		}
		case 17: {

			break;
		}
		case 18: {

			break;
		}
		case 19: {

			break;
		}
		case 20: {

			break;
		}
		case 21: {

			break;
		}
		case 22: {

			break;
		}
		case 23: {

			break;
		}
		case 24: {

			break;
		}
		case 25: {

			break;
		}
		case 26: {

			break;
		}
		case 27: {

			break;
		}
		case 28: {

			break;
		}
		case 29: {

			break;
		}
		case 30: {
			log("Log::Application::wm_notify_nm_click_statusbar() 30");
			statusbar_Menu_Customization();
			break;
		}
		default: {
			break;
		}
	}
}
void Application::wm_notify_nm_click(UINT_PTR idFrom, DWORD_PTR dwItemSpec)
{
	switch (idFrom) {
		case IDC_TOOLBAR_FILE: {
			wm_notify_nm_click_toolbar_File(dwItemSpec);
			break;
		}
		case IDC_TOOLBAR_DRAW: {
			wm_notify_nm_click_toolbar_Draw(dwItemSpec);
			break;
		}
		case IDC_STATUSBAR: {
			wm_notify_nm_click_statusbar(dwItemSpec);
			break;
		}
		default: {
			break;
		}
	}
}

void Application::wm_notify_nm_rclick_statusbar(DWORD_PTR dwItemSpec)
{
	switch (dwItemSpec) {
		//case 0: {
		//
		//	break;
		//}
		case 1: {
			statusbar_Menu_Coordinates();
			break;
		}
			  //case 2: {
			  //
			  //	break;
			  //}
		case 3: {
			statusbar_Menu_Grid();
			break;
		}
		case 4: {
			statusbar_Menu_SnapMode();
			break;
		}
		case 5: {

			break;
		}
		case 6: {
			statusbar_Menu_DynamicInput();
			break;
		}
		case 7: {
			break;
		}
		case 8: {

			break;
		}
		case 9: {

			break;
		}
		case 10: {

			break;
		}
		case 11: {

			break;
		}
		case 12: {

			break;
		}
		case 13: {

			break;
		}
		case 14: {

			break;
		}
		case 15: {

			break;
		}
		case 16: {

			break;
		}
		case 17: {

			break;
		}
		case 18: {

			break;
		}
		case 19: {

			break;
		}
		case 20: {

			break;
		}
		case 21: {

			break;
		}
		case 22: {

			break;
		}
		case 23: {

			break;
		}
		case 24: {

			break;
		}
		case 25: {

			break;
		}
		case 26: {

			break;
		}
		case 277: {

			break;
		}
		case 28: {

			break;
		}
		case 29: {

			break;
		}
		default: {
			break;
		}
	}
}
void Application::wm_notify_nm_rclick(UINT_PTR idFrom, DWORD_PTR dwItemSpec)
{
	switch (idFrom) {
		case IDC_STATUSBAR: {
			wm_notify_nm_rclick_statusbar(dwItemSpec);
			break;
		}
		default: {
			break;
		}
	}
}

void Application::wm_notify_rbn_heightchange() { wm_size(); }
void Application::wm_notify_tbn_dropdown(HWND hwndFrom, int iItem)
{
	RECT rc{};
	::SendMessage(hwndFrom, TB_GETRECT, (WPARAM)iItem, (LPARAM)&rc); //Get the coordinates of the button.           
	MapWindowPoints(hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2); //Convert to screen coordinates. 

	HMENU hDraw = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_DRAW));

	//HMENU hSubMenu = ::GetSubMenu(hMenuLoaded, 6);
	HMENU hPopupMenu{};
	switch (iItem) {
		case IDC_TOOLBAR_DRAW_ARC: {
			hPopupMenu = ::GetSubMenu(hDraw, 11);
			break;
		}
		case IDC_TOOLBAR_DRAW_CIRCLE: {
			hPopupMenu = ::GetSubMenu(hDraw, 12);
			break;
		}
		case IDC_TOOLBAR_DRAW_ELLIPSE: {
			hPopupMenu = ::GetSubMenu(hDraw, 13);
			break;
		}
		default: {
			break;
		}
	}

	//Set up the pop-up menu.
	//In case the toolbar is too close to the bottom of the screen, 
	//set rcExclude equal to the button rectangle and the menu will appear above 
	//the button rather than below it.
	TPMPARAMS tpm;
	tpm.cbSize = sizeof(TPMPARAMS);
	tpm.rcExclude = rc;
	if (hPopupMenu != NULL) {
		::TrackPopupMenuEx(
			hPopupMenu,
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
			rc.left,
			rc.bottom,
			m_hWnd,
			&tpm
		);
	}

	//DestroyMenu(hPopupMenu);
	DestroyMenu(hDraw);
}

void Application::wm_notify_ttn_getdispinfo_File(UINT_PTR idFrom, LPSTR& lpszText)
{
	switch (idFrom) {
		case IDC_TOOLBAR_FILE_NEW: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_FILE_NEW);
			break;
		}
		case IDC_TOOLBAR_FILE_OPEN: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_FILE_OPEN);
			break;
		}
		case IDC_TOOLBAR_FILE_SAVE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_FILE_SAVE);
			break;
		}
		default: {
			break;
		}
	}
}
void Application::wm_notify_ttn_getdispinfo_Draw(UINT_PTR idFrom, LPSTR& lpszText)
{
	switch (idFrom) {
		case IDC_TOOLBAR_DRAW_LINE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_LINE);
			break;
		}
		case IDC_TOOLBAR_DRAW_RAY: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_RAY);
			break;
		}
		case IDC_TOOLBAR_DRAW_POLYLINE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_POLYLINE);
			break;
		}
		case IDC_TOOLBAR_DRAW_POLYGON: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_POLYGON);
			break;
		}
		case IDC_TOOLBAR_DRAW_RECTANGLE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_RECTANGLE);
			break;
		}
		case IDC_TOOLBAR_DRAW_ARC: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_ARC);
			break;
		}
		case IDC_TOOLBAR_DRAW_CIRCLE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_CIRCLE);
			break;
		}
		case IDC_TOOLBAR_DRAW_REVISION_CLOUD: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_REVISION_CLOUD);
			break;
		}
		case IDC_TOOLBAR_DRAW_SPLINE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_SPLINE);
			break;
		}
		case IDC_TOOLBAR_DRAW_ELLIPSE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_ELLIPSE);
			break;
		}
		case IDC_TOOLBAR_DRAW_ELLIPSE_ARC: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_ELLIPSE_ARC);
			break;
		}
		case IDC_TOOLBAR_DRAW_POINT_SINGLE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_POINT_SINGLE);
			break;
		}
		default: {
			break;
		}
	}
}
void Application::wm_notify_ttn_getdispinfo_Documents(UINT_PTR idFrom, LPSTR& lpszText)
{
	UNREFERENCED_PARAMETER(idFrom);

	//BOOL nResult = FALSE;
	//
	//TCITEM tcItem;
	//tcItem.mask = TCIF_TEXT;
	//tcItem.pszText = lpszText;
	//tcItem.cchTextMax = MAX_LOADSTRING;
	//nResult = (BOOL)::SendMessage(m_hTabcontrol_Documents, TCM_GETITEM, (WPARAM)(unsigned int)idFrom, (LPARAM)&tcItem);
	//if (nResult == FALSE) {
	//	ErrorHandler();
	//}

	if (m_lpDocumentManager->getSize() == 0) return;//Exit if there are no documents...

	Document* lpActiveDocument = (Document*)m_lpDocumentManager->getObjects().at((unsigned int)idFrom);
	if (lpActiveDocument == nullptr) return;

	lpszText = lpActiveDocument->getFileName();
}
void Application::wm_notify_ttn_getdispinfo(NMHDR nmHDR, LPSTR& lpszText)
{
	//if (nmHDR.hwndFrom == m_hToolbar_File) {
	//	wm_notify_ttn_getdispinfo_File(nmHDR.idFrom, lpszText);
	//}
	//if (nmHDR.hwndFrom == m_hToolbar_Draw) {
	//	wm_notify_ttn_getdispinfo_File(nmHDR.idFrom, lpszText);
	//}
	if (nmHDR.hwndFrom == m_hTabcontrol_Documents_Tooltip) {
		wm_notify_ttn_getdispinfo_Documents(nmHDR.idFrom, lpszText);
	}

	switch (nmHDR.idFrom) {
		case IDC_TOOLBAR_FILE_NEW: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_FILE_NEW);
			break;
		}
		case IDC_TOOLBAR_FILE_OPEN: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_FILE_OPEN);
			break;
		}
		case IDC_TOOLBAR_FILE_SAVE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_FILE_SAVE);
			break;
		}
		case IDC_TOOLBAR_DRAW_LINE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_LINE);
			break;
		}
		case IDC_TOOLBAR_DRAW_RAY: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_RAY);
			break;
		}
		case IDC_TOOLBAR_DRAW_POLYLINE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_POLYLINE);
			break;
		}
		case IDC_TOOLBAR_DRAW_POLYGON: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_POLYGON);
			break;
		}
		case IDC_TOOLBAR_DRAW_RECTANGLE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_RECTANGLE);
			break;
		}
		case IDC_TOOLBAR_DRAW_ARC: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_ARC);
			break;
		}
		case IDC_TOOLBAR_DRAW_CIRCLE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_CIRCLE);
			break;
		}
		case IDC_TOOLBAR_DRAW_REVISION_CLOUD: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_REVISION_CLOUD);
			break;
		}
		case IDC_TOOLBAR_DRAW_SPLINE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_SPLINE);
			break;
		}
		case IDC_TOOLBAR_DRAW_ELLIPSE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_ELLIPSE);
			break;
		}
		case IDC_TOOLBAR_DRAW_ELLIPSE_ARC: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_ELLIPSE_ARC);
			break;
		}
		case IDC_TOOLBAR_DRAW_POINT_SINGLE: {
			lpszText = MAKEINTRESOURCE(IDC_TOOLBAR_DRAW_POINT_SINGLE);
			break;
		}
		default: {
			break;
		}
	}
}

void Application::wm_notify_tcn_selchange_documents()
{
	int itemIndex = ::SendMessage(m_hTabcontrol_Documents, TCM_GETCURSEL, 0, 0);

	TCITEM tcItem{ TCIF_PARAM };
	::SendMessage(m_hTabcontrol_Documents, TCM_GETITEM, (WPARAM)itemIndex, (LPARAM)&tcItem);

	Document* lpDocument = (Document*)tcItem.lParam;
	if (lpDocument == nullptr) {
		m_lpDocumentManager->setActiveObject(nullptr);
	}
	else {
		m_lpDocumentManager->setActiveObject(lpDocument);//Set active document,...

		HWND hWnd = lpDocument->getHWND();
		::SetWindowPos(hWnd, HWND_TOP, m_ptDocument.x, m_ptDocument.y, m_szDocument.cx, m_szDocument.cy, SWP_SHOWWINDOW);
		::SendMessage(hWnd, WM_ACTIVATE, MAKEWPARAM(WA_ACTIVE, 0), (LPARAM)hWnd);

		NMHDR nmh;
		nmh.hwndFrom = m_hWnd;
		nmh.idFrom = IDC_TABCTRL_DOCUMENTS;
		nmh.code = TCN_SELCHANGE;
		::SendMessage(lpDocument->getRenderWindow()->getHWND(), WM_NOTIFY, (WPARAM)IDC_TABCTRL_DOCUMENTS, (LPARAM)&nmh);

#pragma warning( push )
#pragma warning( disable : 26454 )
		//NMHDR nmh{ m_hWnd,IDC_TABCTRL_DOCUMENTS,TCN_SELCHANGE };//Send WM_NOTIFY message to command windows to let them know the active document has changed.
#pragma warning( pop )

		if (m_lpTextWindow != nullptr) {
			::SendMessage(m_lpTextWindow->getHWND(), WM_NOTIFY, (WPARAM)IDC_TABCTRL_DOCUMENTS, (LPARAM)&nmh);
		}
		if (m_lpCommandLine != nullptr) {
			::SendMessage(m_lpCommandLine->getHWND(), WM_NOTIFY, (WPARAM)IDC_TABCTRL_DOCUMENTS, (LPARAM)&nmh);
		}
		if (m_lpDynamicInput != nullptr) {
			::SendMessage(m_lpDynamicInput->getHWND(), WM_NOTIFY, (WPARAM)IDC_TABCTRL_DOCUMENTS, (LPARAM)&nmh);
		}

		::SetFocus(hWnd);//Set focus for keyboard input.
	}
}
void Application::wm_notify_tcn_selchange_documents_callback()
{
	show_statusbar();

	//::SendMessage(m_hTabcontrol_Layouts, TCM_DELETEALLITEMS, 0, 0);//Delete existing tabs...

	if (m_lpDocumentManager->getSize() == 0) return;//Exit if there are no documents...

	Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
	if (lpActiveDocument == nullptr) return;
	if (lpActiveDocument->getLayoutManager() == nullptr) return;
	if (lpActiveDocument->getLayoutManager()->getSize() == 0) return;//Exit if there are no layouts...

	//Paint active document...
	//::SendMessage(lpActiveDocument->getHWND(), WM_PAINT, 0, 0);

	lpActiveDocument->setStatusbarText(m_lpStatusbar->getHWND(), 2);
}
void Application::wm_notify_tcn_selchange(UINT_PTR idFrom)
{
	switch (idFrom) {
		case IDC_TABCTRL_DOCUMENTS: {
			log("Log::Application::wm_notify() TCN_SELCHANGE lpnm->idFrom = IDC_TABCTRL_DOCUMENTS");
			wm_notify_tcn_selchange_documents();
			break;
		}
		case IDC_DOCUMENT: {
			log("Log::Application::wm_notify() TCN_SELCHANGE lpnm->idFrom = IDC_DOCUMENT");
			wm_notify_tcn_selchange_documents_callback();
			break;
		}
		case IDC_LAYOUT: {//Render thread returns WM_NOTIFY on receipt of WM_NOTIFY message so that the application knows the renderthread is readu to paint.
			log("Log::Application::wm_notify() TCN_SELCHANGE lpnm->idFrom = IDC_LAYOUT");
			wm_notify_tcn_selchange_documents_callback();
			break;
		}
		default: {
			log("Log::Application::wm_notify() TCN_SELCHANGE lpnm->idFrom = default");
			break;
		}
	}
}

int Application::wm_notify_tcn_selchanging_documents()
{
	int nResult = 0;

	Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
	if (lpActiveDocument == nullptr) {
		nResult = 1;
		return nResult;
	}

	HWND hWnd = lpActiveDocument->getHWND();
	::SendMessage(hWnd, WM_CHAR, (WPARAM)VK_ESCAPE, 0);//Cancel any active commands...
	::SendMessage(hWnd, WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, 0), (LPARAM)hWnd);//...and suspend active drawing thread.

	return nResult;
}
int Application::wm_notify_tcn_selchanging_documents_callback()
{
	return 0;
}
int Application::wm_notify_tcn_selchanging(UINT_PTR idFrom)
{
	int nResult = 0;
	switch (idFrom) {
		case IDC_TABCTRL_DOCUMENTS: {
			log("Log::Application::wm_notify() TCN_SELCHANGING lpnm->idFrom = IDC_TABCTRL_DOCUMENTS");
			nResult = wm_notify_tcn_selchanging_documents();
			break;
		}
		case IDC_DOCUMENT: {
			log("Log::Application::wm_notify() TCN_SELCHANGING lpnm->idFrom = IDC_DOCUMENT");
			nResult = wm_notify_tcn_selchanging_documents_callback();
			break;
		}
		case IDC_LAYOUT: {
			log("Log::Application::wm_notify() TCN_SELCHANGING lpnm->idFrom = IDC_LAYOUT");
			nResult = wm_notify_tcn_selchanging_documents_callback();
			break;
		}
		default: {
			log("Log::Application::wm_notify() TCN_SELCHANGING lpnm->idFrom = default");
			break;
		}
	}
	return nResult;
}

void Application::wm_parentnotify_create(UINT_PTR id, HWND hwnd)
{
	if (id != IDC_DOCUMENT) return;
	if (!::IsWindow(hwnd)) return;

	int n = m_lpDocumentManager->getSize();
	if (m_lpDocumentManager->getSize() > 0) return;

	createTextWindow();//Disable for production version - TextWindow visibility should be reset on application start.
	createCommandLine();
	createDynamicInput();

	createLayerPropertiesManager();
}
void Application::wm_parentnotify_destroy(UINT_PTR id, HWND hwnd)
{
	if (id != IDC_DOCUMENT) return;
	if (!::IsWindow(hwnd)) return;

	int n = m_lpDocumentManager->getSize();
	if (m_lpDocumentManager->getSize() > 1) return;

	::SetMenu(m_hWnd, m_hMenu);//Menu is usually set by document WM_ACTIVATE message, but WM_ACTIVATE is not sent when last document closes so we need to take care of it here. 

	if (TextWindow::s_nVisible == TRUE) {
		//TextWindow::s_nVisible = FALSE;
		if (m_lpTextWindow->getValid() == TRUE) {
			if (IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
				if (::SendMessage(m_lpTextWindow->getHWND(), WM_CLOSE, 0, 0) == 0) {
					log("Log::Application::wm_command_Window_Close() m_TextWindow WM_CLOSE = 0");
				}
			}
		}
	}
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine->getValid() == TRUE) {
			if (IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
				if (::SendMessage(m_lpCommandLine->getHWND(), WM_CLOSE, 0, 0) == 0) {
					log("Log::Application::wm_command_Window_Close() m_CommandLine WM_CLOSE = 0");
				}
			}
		}
	}
}

void Application::wm_size_rebar()
{
	RECT rcRebar;
	GetClientRect(m_hRebar_Standard, &rcRebar);
	::SetWindowPos(m_hRebar_Standard, NULL, 0, 0, m_cx, rcRebar.bottom, SWP_SHOWWINDOW);
}
void Application::wm_size_documents()
{
	RECT rcRebar;
	GetClientRect(m_hRebar_Standard, &rcRebar);

	::SetWindowPos(m_hTabcontrol_Documents, NULL, 3, rcRebar.bottom + 3, m_cx - 6, s_nTabconctrol_Documents_Height, SWP_SHOWWINDOW);

	RECT rcStatusbar;
	GetClientRect(m_lpStatusbar->getHWND(), &rcStatusbar);

	m_ptDocument.x = 0;
	m_ptDocument.y = rcRebar.bottom + s_nTabconctrol_Documents_Height;
	m_szDocument.cx = m_cx;
	m_szDocument.cy = m_cy - rcRebar.bottom - rcStatusbar.bottom - s_nTabconctrol_Documents_Height;

	Document* lpDocument = (Document*)m_lpDocumentManager->getActiveObject();
	if (lpDocument == nullptr) {
		return;
	}

	if (lpDocument->getValid() == FALSE) {
		return;
	}

	::SetWindowPos(lpDocument->getHWND(), NULL, m_ptDocument.x, m_ptDocument.y, m_szDocument.cx, m_szDocument.cy, SWP_SHOWWINDOW);
}
void Application::wm_size_statusbar()
{
	if (m_lpStatusbar == nullptr) {
		return;
	}
	m_lpStatusbar->wm_size();
}
void Application::wm_size()
{
	wm_size_rebar();
	wm_size_statusbar();
	wm_size_documents();
}

void Application::show_statusbar()
{
	//m_lpStatusbar = NULL;
	//delete m_lpStatusbar;

	//m_lpStatusbar = new Statusbar(WS_CHILD | WS_VISIBLE | CCS_BOTTOM, m_hWnd, IDC_STATUSBAR, s_hInstance);
	std::array<int, STATUSBARPANELCOUNT>nPanelWidths = { 190,50,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,0 };
	m_lpStatusbar->setParts(nPanelWidths);

	m_lpStatusbar->setPanelVisibility(4, FALSE);
	m_lpStatusbar->setPanelVisibility(11, FALSE);
	m_lpStatusbar->setPanelVisibility(12, FALSE);
	m_lpStatusbar->setPanelVisibility(13, FALSE);
	m_lpStatusbar->setPanelVisibility(14, FALSE);
	m_lpStatusbar->setPanelVisibility(15, FALSE);
	m_lpStatusbar->setPanelVisibility(16, FALSE);
	m_lpStatusbar->setPanelVisibility(17, FALSE);
	m_lpStatusbar->setPanelVisibility(23, FALSE);
	m_lpStatusbar->setPanelVisibility(24, FALSE);
	m_lpStatusbar->setPanelVisibility(25, FALSE);
	m_lpStatusbar->setPanelVisibility(27, FALSE);

	wm_create_statusbar_tooltips();

	wm_size_statusbar();
	wm_exitsizemove_statusbar_tooltips();
}
void Application::show_statusbar(const char* filename)
{
	//UNREFERENCED_PARAMETER(filename);

	//m_lpStatusbar = NULL;
	//delete m_lpStatusbar;

	//m_lpStatusbar = new Statusbar(WS_CHILD | WS_VISIBLE | CCS_BOTTOM, m_hWnd, IDC_STATUSBAR, s_hInstance);
	std::array<int, 3>nPanelWidths = { m_cx / 5 * 2,m_cx / 5 * 3,0 };
	m_lpStatusbar->setParts(nPanelWidths);

	m_lpStatusbar->setText(0, filename);
	m_lpStatusbar->showProgressbar(1);

	wm_size_statusbar();
	wm_exitsizemove_statusbar_tooltips();
}

//Private static functions
INT_PTR CALLBACK Application::s_nfnHelpProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (uMsg) {
		case WM_INITDIALOG: {
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				::EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK Application::s_nfnAboutProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (uMsg) {
		case WM_INITDIALOG: {
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				::EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK Application::s_nfnWindowsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (uMsg) {
		case WM_INITDIALOG: {
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				::EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
	}
	return (INT_PTR)FALSE;
}

BOOL CALLBACK Application::s_bfnEnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	const char szClassName[MAX_LOADSTRING]{ 0 };
	::GetClassName(hWnd, (LPSTR)szClassName, MAX_LOADSTRING);

	MSG* mMsg = (MSG*)lParam;

	//log("Log::Application::s_bfnEnumWindowsProc() %s", szClassName);

	switch (mMsg->message) {
		case WM_CLOSE: {
			int nID = GetDlgCtrlID(hWnd);
			log("Log::Application::s_bfnEnumWindowsProc() %d", nID);
			switch (nID) {
				case IDC_COMMANDLINE:
				case IDC_DYNAMICINPUT:
				case IDC_TEXTWINDOW:
				case IDC_LAYERPROPERTIESMANAGER: {
					::SendMessage(hWnd, WM_CLOSE, 0, 0);

					//LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
					//Document* lpWindow = reinterpret_cast<Document*>(ptr);
					//s_lpApplication->m_lpWindowManager->removeObject(lpWindow);
					//delete lpWindow;

					break;
				}
				default: {
					break;
				}
			}
			break;
		}
					 //case WM_KILLFOCUS: {
					 //	::SendMessage(hWnd, WM_KILLFOCUS, 0, 0);
					 //	break;
					 //}
					 //case WM_SETFOCUS: {
					 //	::SendMessage(hWnd, WM_SETFOCUS, 0, 0);
					 //	break;
					 //}
		default: {
			break;
		}
	}

	return 1;
}
BOOL CALLBACK Application::s_bfnEnumChildWindowsProc(HWND hWnd, LPARAM lParam)
{
	const char szClassName[MAX_LOADSTRING]{ 0 };
	::GetClassName(hWnd, (LPSTR)szClassName, MAX_LOADSTRING);

	MSG* mMsg = (MSG*)lParam;

	log("Log::Application::s_bfnEnumChildWindowsProc() %s", szClassName);

	//if (::strcmp(szClassName, (LPCSTR)mMsg->lParam) != 0) return 1;

	switch (mMsg->message) {
		case WM_CLOSE: {
			int nID = GetDlgCtrlID(hWnd);
			switch (nID) {
				case IDC_DOCUMENT: {
					::SendMessage(hWnd, WM_CLOSE, 0, 0);

					LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
					Document* lpDocument = reinterpret_cast<Document*>(ptr);
					//s_lpApplication->m_lpDocumentManager->removeObject(lpDocument);
					delete lpDocument;

					break;
				}
				default: {
					break;
				}
			}
			break;
		}
					 //case WM_KILLFOCUS: {
					 //	::SendMessage(hWnd, WM_KILLFOCUS, 0, 0);
					 //	break;
					 //}
					 //case WM_SETFOCUS: {
					 //	::SendMessage(hWnd, WM_SETFOCUS, 0, 0);
					 //	break;
					 //}
		default: {
			break;
		}
	}

	return 1;
}

//XML data functions
using namespace tinyxml2;

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }
#endif

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

class clsXMLNode : public XMLNode {};

int Application::xmlGetModelspace2D(LONG_PTR element)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpModelspace2D = ((XMLElement*)element)->FirstChildElement("Modelspace2D");
	if (lpModelspace2D) {
		XMLElement* lpUniformBackground = lpModelspace2D->FirstChildElement("UniformBackground");
		if (lpUniformBackground) {
			Colour3<int> c3nUniformBackground = { 255,255,255 };
			if (xmlutil::getXMLColour(lpUniformBackground, c3nUniformBackground) == XML_SUCCESS) {
				//GLRenderWindow::setColour(GLRenderWindow::Context::ModelSpace2D, GLRenderWindow::InterfaceElement::UniformBackground, RGB(c3nUniformBackground.r, c3nUniformBackground.g, c3nUniformBackground.b));
			}
		}
	}
	else {
		eResult = XML_ERROR_PARSING_ELEMENT;
	}

	return (int)eResult;
}
int Application::xmlGetSheetLayout(LONG_PTR element)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpLayout = ((XMLElement*)element)->FirstChildElement("Sheet_Layout");
	if (lpLayout) {
		XMLElement* lpUniformBackground = lpLayout->FirstChildElement("UniformBackground");
		if (lpUniformBackground) {
			Colour3<int> c3nUniformBackground = { 255,255,255 };
			if (xmlutil::getXMLColour(lpUniformBackground, c3nUniformBackground) == XML_SUCCESS) {
				log("Log::Application::getOptions f3Colour = { %d, %d, %d }", c3nUniformBackground.r, c3nUniformBackground.g, c3nUniformBackground.b);
				GLRenderWindow::setColour(GLRenderWindow::Context::Sheet_Layout, GLRenderWindow::InterfaceElement::UniformBackground, RGB(c3nUniformBackground.r, c3nUniformBackground.g, c3nUniformBackground.b));
			}
		}

		XMLElement* lpPaperBackground = lpLayout->FirstChildElement("PaperBackground");
		if (lpPaperBackground) {
			Colour3<int> c3nPaperBackground = { 255,255,255 };
			if (xmlutil::getXMLColour(lpPaperBackground, c3nPaperBackground) == XML_SUCCESS) {
				log("Log::Application::getOptions f3Colour = { %d, %d, %d }", c3nPaperBackground.r, c3nPaperBackground.g, c3nPaperBackground.b);
				GLRenderWindow::setColour(GLRenderWindow::Context::Sheet_Layout, GLRenderWindow::InterfaceElement::PaperBackground, RGB(c3nPaperBackground.r, c3nPaperBackground.g, c3nPaperBackground.b));
			}
		}

		XMLElement* lpCrosshairs = lpLayout->FirstChildElement("Crosshairs");
		if (lpCrosshairs) {
			Colour3<int> c3nCrosshairs{ 255,255,255 };
			if (xmlutil::getXMLColour(lpCrosshairs, c3nCrosshairs) == XML_SUCCESS) {
				log("Log::Application::getOptions f3Colour = { %d, %d, %d }", c3nCrosshairs.r, c3nCrosshairs.g, c3nCrosshairs.b);
				XMLElement* lpTint = lpCrosshairs->FirstChildElement("TintForXYZ");
				BOOL nTint = false;
				if (xmlutil::getXMLInt(lpTint, nTint) == XML_SUCCESS) {
					log("Log::Application::getOptions bTintForXYZ = %d,", nTint);
					GLRenderWindow::setColour(GLRenderWindow::Context::Sheet_Layout, GLRenderWindow::InterfaceElement::Crosshairs, RGB(c3nCrosshairs.r, c3nCrosshairs.g, c3nCrosshairs.b), nTint);
				}
			}
		}
	}
	else {
		eResult = XML_ERROR_PARSING_ELEMENT;
	}

	return (int)eResult;
}
int Application::xmlGetParallelProjection3D(LONG_PTR element)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpParallelProjection3D = ((XMLElement*)element)->FirstChildElement("ParallelProjection3D");
	if (lpParallelProjection3D) {
		//InterfaceElement::UniformBackground
		XMLElement* lpUniformBackground = lpParallelProjection3D->FirstChildElement("UniformBackground");
		if (lpUniformBackground) {
			Colour3<int> c3nUniformBackground = { 255,255,255 };
			if (xmlutil::getXMLColour(lpUniformBackground, c3nUniformBackground) == XML_SUCCESS) {
				log("Log::Application::getOptions f3Colour = { %d, %d, %d }", c3nUniformBackground.r, c3nUniformBackground.g, c3nUniformBackground.b);
				GLRenderWindow::setColour(GLRenderWindow::Context::ParallelProjection3D, GLRenderWindow::InterfaceElement::UniformBackground, RGB(c3nUniformBackground.r, c3nUniformBackground.g, c3nUniformBackground.b));
			}
		}

		XMLElement* lpCrosshairs = lpParallelProjection3D->FirstChildElement("Crosshairs");
		if (lpCrosshairs) {
			Colour3<int> c3nCrosshairs{ 255,255,255 };
			if (xmlutil::getXMLColour(lpCrosshairs, c3nCrosshairs) == XML_SUCCESS) {
				log("Log::Application::getOptions f3Colour = { %d, %d, %d }", c3nCrosshairs.r, c3nCrosshairs.g, c3nCrosshairs.b);
				XMLElement* lpTint = lpCrosshairs->FirstChildElement("TintForXYZ");
				BOOL nTint = false;
				if (xmlutil::getXMLInt(lpTint, nTint) == XML_SUCCESS) {
					log("Log::Application::getOptions bTintForXYZ = %d,", nTint);
					GLRenderWindow::setColour(GLRenderWindow::Context::ParallelProjection3D, GLRenderWindow::InterfaceElement::Crosshairs, RGB(c3nCrosshairs.r, c3nCrosshairs.g, c3nCrosshairs.b), nTint);
				}
			}
		}
	}
	else {
		eResult = XML_ERROR_PARSING_ELEMENT;
	}

	return (int)eResult;
}
int Application::xmlGetPerspectiveProjection3D(LONG_PTR element)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpPerspectiveProjection3D = ((XMLElement*)element)->FirstChildElement("PerspectiveProjection3D");
	if (lpPerspectiveProjection3D) {

	}
	else {
		eResult = XML_ERROR_PARSING_ELEMENT;
	}

	return (int)eResult;
}
int Application::xmlGetBlockEditor(LONG_PTR element)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpBlockEditor = ((XMLElement*)element)->FirstChildElement("BlockEditor");
	if (lpBlockEditor) {

	}
	else {
		eResult = XML_ERROR_PARSING_ELEMENT;
	}

	return (int)eResult;
}
int Application::xmlGetCommandLine(LONG_PTR element)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpCommandLine = ((XMLElement*)element)->FirstChildElement("CommandLine");
	if (lpCommandLine) {

	}
	else {
		eResult = XML_ERROR_PARSING_ELEMENT;
	}

	return (int)eResult;
}
int Application::xmlGetPlotPreview(LONG_PTR element)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpPlotPreview = ((XMLElement*)element)->FirstChildElement("PlotPreview");
	if (lpPlotPreview) {

	}
	else {
		eResult = XML_ERROR_PARSING_ELEMENT;
	}

	return (int)eResult;
}

int Application::xmlGetDisplay(LONG_PTR root)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpDisplay = ((XMLElement*)root)->FirstChildElement("Display");
	if (lpDisplay == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpWindowElements = lpDisplay->FirstChildElement("WindowElements");
	if (lpWindowElements == nullptr) return XML_ERROR_PARSING_ELEMENT;

	//XMLElement* lpDisplayScrollbars = lpWindowElements->FirstChildElement("Displayscrollbarsindrawingwindow");
	//if (lpDisplayScrollbars) {
	//	bool bDisplaysScrollbars = false;
	//	if (xmlutil::getXMLBool(lpDisplayScrollbars, bDisplaysScrollbars) == XML_SUCCESS) {
	//		log("Log::Application::xmlGetDisplayScrollbars Displayscrollbarsindrawingwindow =  %d", bDisplaysScrollbars);
	//		GLRenderWindow::s_bScrollbars = bDisplaysScrollbars;
	//	}
	//}

	XMLElement* lpCrosshairsize = lpDisplay->FirstChildElement("Crosshairsize");
	if (lpCrosshairsize) {
		int nCrosshairsizePercent = 5;
		if (xmlutil::getXMLInt(lpCrosshairsize, nCrosshairsizePercent) == XML_SUCCESS) {
			log("Log::Application::getCrosshairsize nCrosshairsize =  %d", nCrosshairsizePercent);
			int nSceensize = gsmGetScreenSize();
			int nCrosshairsize = (nCrosshairsizePercent * nSceensize) / 180;
			GLPointer::setCrosshairSize(nCrosshairsize);
			GLPointer::setPicboxSize(3);
		}
	}

	return (int)eResult;
}
int Application::xmlGetColours(LONG_PTR root)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpShared = ((XMLElement*)root)->FirstChildElement("Shared");
	if (lpShared == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpColours = lpShared->FirstChildElement("Colours");
	if (lpColours == nullptr) return XML_ERROR_PARSING_ELEMENT;

	xmlGetModelspace2D((LONG_PTR)lpColours);
	xmlGetSheetLayout((LONG_PTR)lpColours);
	xmlGetParallelProjection3D((LONG_PTR)lpColours);
	xmlGetPerspectiveProjection3D((LONG_PTR)lpColours);
	xmlGetBlockEditor((LONG_PTR)lpColours);
	xmlGetCommandLine((LONG_PTR)lpColours);
	xmlGetPlotPreview((LONG_PTR)lpColours);

	return (int)eResult;
}

int Application::xmlGetOptions()
{
	XMLError eResult = XML_SUCCESS;

	//char chSolutionDir[MAX_STRING];
	//const char* pchSolutionDir = xmlutil::getPath(EXPAND(SOLDIR));
	//::strcpy_s(chSolutionDir, pchSolutionDir);

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

	xmlGetDisplay((LONG_PTR)lpRoot);
	//getDrafting((LONG_PTR)lpRoot);

	xmlGetColours((LONG_PTR)lpRoot);

	return (int)eResult;
}
int Application::xmlGetUCSIconProperties()
{
	XMLError eResult = XML_SUCCESS;

	const char* pchSolutionDir = EXPAND(SOLDIR);
	char chSolutionDir[MAX_LOADSTRING]{ 0 };
	char chSolutionDirTemp[MAX_LOADSTRING]{ 0 };
	::strcpy_s(chSolutionDirTemp, pchSolutionDir);//Copy const char* into char[].
	char* substr = chSolutionDirTemp + 1;//Remove the quotes at the beginning of the string.
	substr[::strlen(substr) - 2] = '\0';//Remove the quotes and colon at the end of the string.
	::strcpy_s(chSolutionDir, substr);
	char chData[MAX_LOADSTRING]{ "source\\mycad\\cadkernel\\MKE_settings.xml" };
	::strcat_s(chSolutionDir, chData);

	tinyxml2::XMLDocument xmlDoc;
	eResult = xmlDoc.LoadFile(chSolutionDir);
	XMLCheckResult(eResult);

	XMLNode* lpRoot = xmlDoc.FirstChild();
	if (lpRoot == nullptr) return XML_ERROR_FILE_READ_ERROR;

	XMLElement* lpUCSIcon = lpRoot->FirstChildElement("UCSIcon");
	if (lpUCSIcon == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpStyle = lpUCSIcon->FirstChildElement("Style");
	if (lpStyle) {
		int nStyle = 1;
		if (xmlutil::getXMLInt(lpStyle, nStyle) == XML_SUCCESS) {
			log("Log::Application::getSettings nStyle =  %d", nStyle);
			mycad::GLUCSIcon::setStyle(static_cast<mycad::GLUCSIcon::Style>(nStyle));
		}
	}

	XMLElement* lpLineWidth = lpUCSIcon->FirstChildElement("LineWidth");
	if (lpLineWidth) {
		int nLineWidth = 1;
		if (xmlutil::getXMLInt(lpLineWidth, nLineWidth) == XML_SUCCESS) {
			log("Log::Application::getSettings nLineWidth =  %d", nLineWidth);
			mycad::GLUCSIcon::setLineWidth(nLineWidth);
		}
	}

	XMLElement* lpSize = lpUCSIcon->FirstChildElement("Size");
	if (lpSize) {
		int nSize = 12;
		if (xmlutil::getXMLInt(lpSize, nSize) == XML_SUCCESS) {
			log("Log::Application::getSettings Size =  %d", nSize);
			mycad::GLUCSIcon::setSize(nSize);
		}
	}

	XMLElement* lpModelspaceIconColour = lpUCSIcon->FirstChildElement("ModelspaceIconColour");
	if (lpModelspaceIconColour) {
		Colour3<int> c3nColour = { 255,255,255 };
		if (xmlutil::getXMLColour(lpModelspaceIconColour, c3nColour) == XML_SUCCESS) {
			log("Log::Application::xmlGetOptions f3Colour = { %d, %d, %d }", c3nColour.r, c3nColour.g, c3nColour.b);

			Colour3<float> c3fColour = { 0.f,0.f,0.f };

			c3fColour.r = (float)c3nColour.r / 255.f;
			c3fColour.g = (float)c3nColour.g / 255.f;
			c3fColour.b = (float)c3nColour.b / 255.f;

			mycad::ModelspaceUCSIcon::setColour(c3fColour);
		}
	}

	XMLElement* lpLayoutTabIconColour = lpUCSIcon->FirstChildElement("LayoutTabIconColour");
	if (lpLayoutTabIconColour) {
		Colour3<int> c3nColour = { 255,255,255 };
		if (xmlutil::getXMLColour(lpLayoutTabIconColour, c3nColour) == XML_SUCCESS) {
			log("Log::Application::xmlGetOptions f3Colour = { %d, %d, %d }", c3nColour.r, c3nColour.g, c3nColour.b);

			Colour3<float> c3fColour = { 0.f,0.f,0.f };

			c3fColour.r = (float)c3nColour.r / 255.f;
			c3fColour.g = (float)c3nColour.g / 255.f;
			c3fColour.b = (float)c3nColour.b / 255.f;

			mycad::LayoutTabUCSIcon::setColour(c3fColour);
		}
	}

	return (int)eResult;
}

HRESULT Application::xmlGetCommand_Prompt(LONG_PTR lpNode, COMMANDINFO* lpCmdInfo)
{
	//const char* lpszPrompt2 = xmlutil::getXMLText(((XMLElement*)lpNode)->ToElement());

	char lpszPrompt_tmp[MAX_LOADSTRING] = "";
	char lpszPrompt[MAX_LOADSTRING]{ 0 };
	::strcpy_s(lpszPrompt, xmlutil::getXMLText(((XMLElement*)lpNode)->ToElement()));
	HRESULT hResult = xmlutil::getFirstAndLastCharStripped(lpszPrompt, lpszPrompt_tmp);
	if (SUCCEEDED(hResult)) {
		std::string str = lpszPrompt_tmp;
		lpCmdInfo->vecPrompts.push_back(str);
		//log("Log::Application::getCommand Prompt = %s", lpszPrompt2);
	}

	return hResult;
}
int Application::xmlGetCommand_Advanced(LONG_PTR node, COMMANDINFO* lpCmdInfo)
{
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpAdvanced = ((XMLElement*)node)->FirstChildElement("Advanced");
	if (lpAdvanced == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpID = lpAdvanced->FirstChildElement("ID");
	if (lpID != nullptr) {
		int nID = 0;
		eResult = xmlutil::getXMLInt(lpID, nID);
		if (eResult == XML_SUCCESS) {
			lpCmdInfo->nID = nID;
			//log("Log::Application::getCommand ID = %d", nID);
		}
	}

	return (int)eResult;
}
int Application::xmlGetCommand_Command(LONG_PTR node, COMMANDINFO* lpCmdInfo)
{
	HRESULT hResult = S_OK;
	XMLError eResult = XML_SUCCESS;

	XMLElement* lpCommand = ((XMLElement*)node)->FirstChildElement("Command");
	if (lpCommand == nullptr) return XML_ERROR_PARSING_ELEMENT;

	XMLElement* lpAlias = lpCommand->FirstChildElement("Alias");
	if (lpAlias != nullptr) {
		const char* lpszAlias = xmlutil::getXMLText(lpAlias);
		if (lpszAlias != nullptr) {
			char szAlias[MAX_LOADSTRING] = "";
			hResult = xmlutil::getFirstAndLastCharStripped(lpszAlias, szAlias);
			if (SUCCEEDED(hResult)) {
				::strcpy_s(lpCmdInfo->szAlias, szAlias);
				//log("Log::Application::getCommand Alias = %s", lpszAlias);
			}
		}
	}

	XMLElement* lpCommandDisplayName = lpCommand->FirstChildElement("CommandDisplayName");
	if (lpCommandDisplayName != nullptr) {
		const char* lpszCommandDisplayName = xmlutil::getXMLText(lpCommandDisplayName);
		if (lpszCommandDisplayName != nullptr) {
			char szCommandDisplayName[MAX_LOADSTRING] = "";
			hResult = xmlutil::getFirstAndLastCharStripped(lpszCommandDisplayName, szCommandDisplayName);
			if (SUCCEEDED(hResult)) {
				::strcpy_s(lpCmdInfo->szCommandDisplayName, szCommandDisplayName);
				//log("Log::Application::getCommand CommandDisplayName = %s", lpszCommandDisplayName);
			}
		}
	}

	XMLElement* lpMacro = lpCommand->FirstChildElement("Macro");
	if (lpMacro != nullptr) {
		const char* lpszMacro = xmlutil::getXMLText(lpMacro);
		if (lpszMacro != nullptr) {
			char szMacro[MAX_LOADSTRING] = "";
			hResult = xmlutil::getFirstAndLastCharStripped(lpszMacro, szMacro);
			if (SUCCEEDED(hResult)) {
				::strcpy_s(lpCmdInfo->szMacro, szMacro);
				//log("Log::Application::getCommand Macro = %s", lpszMacro);
			}
		}
	}

	XMLElement* lpPrompts = lpCommand->FirstChildElement("Prompts");
	if (lpPrompts != nullptr) {
		//log("Log::Application::getCommand Prompts");
		for (XMLNode* e = lpPrompts->FirstChild(); e != NULL; e = e->NextSiblingElement()) {
			xmlGetCommand_Prompt((LONG_PTR)e, lpCmdInfo);
		}
	}

	XMLElement* lpTooltip = lpCommand->FirstChildElement("Tooltip");
	if (lpTooltip != nullptr) {
		const char* lpszTooltip = xmlutil::getXMLText(lpTooltip);
		if (lpszTooltip != nullptr) {
			char szTooltip[MAX_LOADSTRING] = "";
			hResult = xmlutil::getFirstAndLastCharStripped(lpszTooltip, szTooltip);
			if (SUCCEEDED(hResult)) {
				::strcpy_s(lpCmdInfo->szTooltip, szTooltip);
				//log("Log::Application::getCommand Tooltip = %s", lpszTooltip);
			}
		}
	}

	XMLElement* lpDescription = lpCommand->FirstChildElement("Description");
	if (lpDescription != nullptr) {
		const char* lpszDescription = xmlutil::getXMLText(lpDescription);
		if (lpszDescription != nullptr) {
			char szDescription[MAX_LOADSTRING] = "";
			hResult = xmlutil::getFirstAndLastCharStripped(lpszDescription, szDescription);
			if (SUCCEEDED(hResult)) {
				::strcpy_s(lpCmdInfo->szDescription, szDescription);
				//log("Log::Application::getCommand Description = %s", lpszDescription);
			}
		}
	}

	XMLElement* lpValue = lpCommand->FirstChildElement("Value");
	if (lpValue != nullptr) {
		int nValue = 0;
		eResult = xmlutil::getXMLInt(lpValue, nValue);
		if (eResult == XML_SUCCESS) {
			lpCmdInfo->nValue = nValue;
			//log("Log::Application::getCommand Value = %d", nValue);
		}
	}

	return (int)eResult;
}

int Application::xmlGetCommand(LONG_PTR node)
{
	XMLError eResult = XML_SUCCESS;
	COMMANDINFO cmdInfo = { 0 };

	xmlGetCommand_Advanced(node, &cmdInfo);
	xmlGetCommand_Command(node, &cmdInfo);

	CommandManager::s_vecCommands.push_back(cmdInfo);
	CommandManager::s_mCommandByID.insert(std::pair<int, COMMANDINFO>(cmdInfo.nID, cmdInfo));
	CommandManager::s_mCommandIDByAlias.insert(std::pair<std::string, int>(cmdInfo.szAlias, cmdInfo.nID));
	CommandManager::s_mCommandIDByName.insert(std::pair<std::string, int>(cmdInfo.szCommandDisplayName, cmdInfo.nID));
	CommandManager::s_mCommandIDByMacro.insert(std::pair<std::string, int>(cmdInfo.szMacro, cmdInfo.nID));

	return (int)eResult;
}
int Application::xmlGetCommands()
{
	XMLError eResult = XML_SUCCESS;

	const char* pchSolutionDir = EXPAND(SOLDIR);
	char chSolutionDir[MAX_LOADSTRING]{ 0 };
	char chSolutionDirTemp[MAX_LOADSTRING]{ 0 };
	::strcpy_s(chSolutionDirTemp, pchSolutionDir);//Copy const char* into char[].
	char* substr = chSolutionDirTemp + 1;//Remove the quotes at the beginning of the string.
	substr[::strlen(substr) - 2] = '\0';//Remove the quotes and colon at the end of the string.
	::strcpy_s(chSolutionDir, substr);
	char chData[MAX_LOADSTRING]{ "source\\mycad\\commandmanager\\CMD_commands.xml" };
	::strcat_s(chSolutionDir, chData);

	tinyxml2::XMLDocument xmlDoc;
	eResult = xmlDoc.LoadFile(chSolutionDir);
	XMLCheckResult(eResult);

	log("");
	log("Log::Application::xmlGetCommands");

	XMLNode* lpRoot = xmlDoc.FirstChild();
	if (lpRoot == nullptr) return XML_ERROR_FILE_READ_ERROR;

	for (XMLNode* e = lpRoot->FirstChild(); e != NULL; e = e->NextSiblingElement()) {
		log("");
		xmlGetCommand((LONG_PTR)e);
	}

	log("");

	return (int)eResult;
}
