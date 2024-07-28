#include "SYS_sdi_systemwin32.h"

#include "DOC_document.h"
#include "DOC_resource.h"

#include "MKE_sdi_application.h"
#include "MKE_resource.h"

#include "EDL_resource.h"
#include "EPL_resource.h"
#include "EWD_resource.h"

#include "resources.h"

#include "fault.h"
#include "log.h"

using namespace mycad;

SDISystemWin32::SDISystemWin32(HINSTANCE hInstance)
	: SystemWin32(hInstance)
{
	log("Log::SDISystemWin32::SDISystemWin32(HINSTANCE hInstance)");

	m_lpApplication = new SDIApplication(&m_WindowManager, &m_DocumentManager);

	registerClasses();
}
SDISystemWin32::~SDISystemWin32()
{
	log("Log::SDISystemWin32::~SDISystemWin32()");

}

void SDISystemWin32::registerApplicationClasses(HINSTANCE hModule)
{
	BOOL nResult = FALSE;
	char szClassName[MAX_LOADSTRING]{ 0 };

	HICON hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_MYCAD));
	HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW);

	::LoadString(hModule, ID_APPLICATION_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName, hIcon, GetSysColor(COLOR_MENU + 1), hCursor, NULL, NULL);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_APPLICATION_CLASS FAILED!");
	}

	::LoadString(hModule, ID_FAKEMENU_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName, NULL, GetSysColor(COLOR_MENU + 1), hCursor, NULL, NULL);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_DOCUMENT_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_DOCUMENT_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName, hIcon, GetSysColor(COLOR_WINDOW + 1), hCursor, NULL, NULL);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_DOCUMENT_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_RENDERWINDOW_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName, hIcon, GetSysColor(COLOR_WINDOW + 1), hCursor, NULL, CS_DBLCLKS | CS_OWNDC);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_RENDERWINDOW_CLASS FAILED!");
	}
}
