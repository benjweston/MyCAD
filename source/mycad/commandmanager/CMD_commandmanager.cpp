#include "CMD_commandmanager.h"

#include "EWD_textwindow.h"
#include "EWD_commandline.h"
#include "EWD_dynamicinput.h"
#include "EWD_resource.h"

#include "resources.h"

#include "reg_util.h"

#include "fault.h"
#include "log.h"

#include <strsafe.h>
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.
#include <windowsx.h>

using namespace mycad;

unsigned int CommandManager::s_nObjectSize(10);

std::vector<COMMANDINFO> CommandManager::s_vecCommands;
std::map<int, COMMANDINFO> CommandManager::s_mCommandByID;
std::map<std::string, int> CommandManager::s_mCommandIDByAlias;
std::map<std::string, int> CommandManager::s_mCommandIDByName;
std::map<std::string, int> CommandManager::s_mCommandIDByMacro;

CommandManager::CommandManager()
    : ObjectManager(),
    m_lpActiveViewObject(nullptr),
    m_ptMouseMove({ 0,0 }),
    m_cch(0),
    m_pchInputBuf(0),
    m_ch(0)
{
    log("Log::CommandManager::CommandManager()");
    m_Objects.reserve(s_nObjectSize);
}
CommandManager::~CommandManager()
{
    log("Log::CommandManager::~CommandManager()");

    m_Objects.erase(m_Objects.begin(), m_Objects.end());
    m_Objects.clear();

    delete m_lpActiveObject;
    m_lpActiveObject = nullptr;

    delete m_lpActiveViewObject;
    m_lpActiveViewObject = NULL;
}

int CommandManager::getIDByText(const char* text)
{
    //HRESULT hResult = S_OK;
    int nCommandID = -1;

    try {
        nCommandID = CommandManager::s_mCommandIDByAlias.at(text);//Check text against command alias...
    }
    catch (std::out_of_range exception) {
        //hResult = ErrorHandler();
        nCommandID = CommandManager::getIDByPartialText(text);//...if that fails, try checking text against partial command name...
        if (nCommandID == -1) {
            try {
                nCommandID = CommandManager::s_mCommandIDByName.at(text);
            }
            catch (std::out_of_range exception) {
                //hResult = ErrorHandler();
                try {
                    nCommandID = CommandManager::s_mCommandIDByMacro.at(text);//...and if that fails check text against complete command name.
                }
                catch (std::out_of_range exception) {
                    //hResult = ErrorHandler();
                    nCommandID = IDC_UNKNOWN;
                }
            }
        }
    }

    //log("Log::CommandManager::getIDByText() Command ID = %d", nCommandID);

    return nCommandID;
}
void CommandManager::setObjectHistory(unsigned int commandhistory) { s_nObjectSize = commandhistory; }

int CommandManager::getIDByPartialText(const char* text)
{
    HRESULT hResult = S_OK;
    int nCommandID = -1;
    size_t m_cch;

    hResult = ::StringCchLength(text, MAX_LOADSTRING, &m_cch);
    if (FAILED(hResult)) {
        ErrorHandler();
    }

    std::map<int, COMMANDINFO>::iterator it;
    for (it = CommandManager::s_mCommandByID.begin(); it != CommandManager::s_mCommandByID.end(); it++) {
        char szCommandName[256];
        for (unsigned int i = 0; i < m_cch; i++) {
            szCommandName[i] = it->second.szCommandDisplayName[i];
        }
        szCommandName[m_cch] = '\0';

        if (::strcmp(szCommandName, text) == 0) {
            nCommandID = it->second.nID;
        }
    }

    return nCommandID;
}

BOOL CommandManager::addObject(Command* command)
{
    BOOL nSuccess = FALSE;

    if (command != nullptr) {
        if (getObjectFound(command) == false) {
            if (command->getID() != IDC_CANCEL) {
                if (m_Objects.size() >= s_nObjectSize) {
                    m_Objects.erase(m_Objects.begin());
                }
                m_Objects.push_back(command);
                nSuccess = TRUE;
            }
        }
    }

    return nSuccess;
}

Command* CommandManager::getActiveObject() const {
    Command* lpActiveObject = nullptr;
    if (m_lpActiveViewObject != nullptr) {
        lpActiveObject = m_lpActiveViewObject;
    }
    else {
        if (m_lpActiveObject != nullptr) {
            lpActiveObject = m_lpActiveObject;
        }
    }
    return lpActiveObject;
}
Command* CommandManager::getActiveObject(CommandContext context) const {
    Command* lpActiveObject = nullptr;
    if (context == CommandContext::COMMAND) {
        lpActiveObject = m_lpActiveObject;
    }
    else if (context == CommandContext::VIEW) {
        lpActiveObject = m_lpActiveViewObject;
    }
    return lpActiveObject;
}
BOOL CommandManager::setActiveObject(Command* command, CommandContext context)
{
    BOOL nSuccess = TRUE;
    if (context == CommandContext::COMMAND) {
        if (command != m_lpActiveObject) {
            if (getObjectFound(command) == true) {
                m_lpActiveObject = command;
            }
            else {
                return FALSE;
            }
        }
    }
    else if (context == CommandContext::VIEW) {
        if (command != m_lpActiveViewObject) {
            if (getObjectFound(command) == true) {
                m_lpActiveViewObject = command;
            }
            else {
                return FALSE;
            }
        }
    }
    return nSuccess;
}


void CommandManager::setObjectInactive()
{
    if (m_lpActiveViewObject != nullptr) {
        if (m_lpActiveViewObject->getValid() == TRUE) {
            ::SendMessage(m_lpActiveViewObject->getHWND(), WM_CLOSE, 0, 0);
        }
        m_lpActiveViewObject = 0;
        if (m_lpActiveViewObject == nullptr) {
            log("Log::CommandManager::setObjectInactive() m_lpActiveViewObject = nullptr");
        }
    }
    if (m_lpActiveObject != nullptr) {
        if (m_lpActiveObject->getValid() == TRUE) {
            ::SendMessage(m_lpActiveObject->getHWND(), WM_CLOSE, 0, 0);
        }
        m_lpActiveObject = 0;
        if (m_lpActiveObject == nullptr) {
            log("Log::CommandManager::setObjectInactive() m_lpActiveObject = nullptr");
        }
    }
}
void CommandManager::setObjectInactive(CommandContext context)
{
    if (context == CommandContext::COMMAND) {
        if (m_lpActiveObject != nullptr) {
            if (m_lpActiveObject->getValid() == TRUE) {
                ::SendMessage(m_lpActiveObject->getHWND(), WM_CLOSE, 0, 0);
            }
            m_lpActiveObject = 0;
            if (m_lpActiveObject == nullptr) {
                log("Log::CommandManager::setObjectInactive() m_lpActiveObject = nullptr");
            }
        }
    }
    else if (context == CommandContext::VIEW) {
        if (m_lpActiveViewObject != nullptr) {
            if (m_lpActiveViewObject->getValid() == TRUE) {
                ::SendMessage(m_lpActiveViewObject->getHWND(), WM_CLOSE, 0, 0);
            }
            m_lpActiveViewObject = 0;
            if (m_lpActiveViewObject == nullptr) {
                log("Log::CommandManager::setObjectInactive() m_lpActiveViewObject = nullptr");
            }
        }
    }
}
void CommandManager::setObjectInactive(const Command* command, CommandContext context)
{
    if (context == CommandContext::COMMAND) {
        if (command == m_lpActiveObject) {
            if (command->getValid() == TRUE) {
                ::SendMessage(command->getHWND(), WM_CLOSE, 0, 0);
            }
            m_lpActiveObject = 0;
            if (m_lpActiveObject == nullptr) {
                log("Log::CommandManager::setObjectInactive() m_lpActiveObject = nullptr");
            }
        }
    }
    else if (context == CommandContext::VIEW) {
        if (command == m_lpActiveViewObject) {
            if (command->getValid() == TRUE) {
                ::SendMessage(command->getHWND(), WM_CLOSE, 0, 0);
            }
            m_lpActiveViewObject = 0;
            if (m_lpActiveViewObject == nullptr) {
                log("Log::CommandManager::setObjectInactive() m_lpActiveViewObject = nullptr");
            }
        }
    }
}

LRESULT CommandManager::sendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Command* lpActiveObject = getActiveObject();

    if (lpActiveObject == nullptr) {
        return FALSE;
    }

    HWND hWnd = lpActiveObject->getHWND();
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

void CommandManager::wm_char_default(WPARAM wParam)
{
    HRESULT hResult = S_OK;

    m_pchInputBuf = (char*)wParam;
    char pchInputBuf[MAX_LOADSTRING]{ 0 };
    ::strcpy_s(pchInputBuf, m_pchInputBuf);

    hResult = ::StringCchLength(pchInputBuf, MAX_LOADSTRING, &m_cch);
    if (FAILED(hResult)) {
        ErrorHandler();
    }
}
