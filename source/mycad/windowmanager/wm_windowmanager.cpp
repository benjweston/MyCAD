#include "wm_window.h"
#include "wm_windowmanager.h"

#include "log.h"

using namespace mycad;

WindowManager::WindowManager()
    : ObjectManager()
{
    log("Log::WindowManager::WindowManager()");
}

BOOL WindowManager::getWindow(const char* classname, Window* window)
{
    for (unsigned int i = 0; i < m_Objects.size(); ++i) {
        char szClassName[MAX_LOADSTRING]{ 0 };
        ::GetClassName(m_Objects[i]->getHWND(), szClassName, MAX_LOADSTRING);
        if (::strcmp(classname, szClassName) == 0) {
            window = m_Objects[i];
            return TRUE;
        }
    }
    return FALSE;
}
Window* WindowManager::getWindow(const char* classname)
{
    for (unsigned int i = 0; i < m_Objects.size(); ++i) {
        char szClassName[MAX_LOADSTRING]{ 0 };
        ::GetClassName(m_Objects[i]->getHWND(), szClassName, MAX_LOADSTRING);
        if (::strcmp(classname, szClassName) == 0) {
            return m_Objects[i];
        }
    }
    return nullptr;
}
