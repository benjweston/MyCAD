#include "DOC_document.h"
#include "DOC_documentmanager.h"

#include "fault.h"
#include "log.h"

using namespace mycad;

DocumentManager::DocumentManager()
    : ObjectManager()
{
    log("Log::DocumentManager::DocumentManager()");
}
DocumentManager::~DocumentManager()
{
	log("Log::DocumentManager::~DocumentManager()");
}

LRESULT DocumentManager::sendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_lpActiveObject == nullptr) {
        return FALSE;
    }

    HWND hWnd = m_lpActiveObject->getHWND();
    if (hWnd == NULL) {
        return FALSE;
    }

    if (IsWindow(hWnd) == FALSE) {
        return FALSE;
    }

#pragma warning( push )
#pragma warning( disable : 6387)
    LRESULT lResult = ::SendMessage(hWnd, uMsg, wParam, lParam);
#pragma warning( pop )

    return lResult;
}
