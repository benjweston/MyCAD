#include "SYS_mdi_systemwin32.h"
#include "SYS_sdi_systemwin32.h"

#include "log.h"

using namespace mycad;

//System::ApplicationInterfaceType System::s_enApplicationInterfaceType{ ApplicationInterfaceType::MultipleDocuments };
System::ApplicationInterfaceType System::s_enApplicationInterfaceType{ ApplicationInterfaceType::SingleDocuments };

System* System::s_system{ NULL };//The single instance of this class

System::System()
{
    log("Log::System::System()");
}
System::System(HINSTANCE hInstance)
{
    UNREFERENCED_PARAMETER(hInstance);
    log("Log::System::System(HINSTANCE)");
}
System::~System()
{
    log("Log::System::~System()");
    delete s_system;
}

System* System::createSystem(HINSTANCE hInstance)
{
    BOOL success;
    if (!s_system) {
        switch (s_enApplicationInterfaceType) {
            case ApplicationInterfaceType::SingleDocuments: {
                log("Log::System::createSystem() SingleDocuments");
                s_system = new SDISystemWin32(hInstance);
                break;
            }
            case ApplicationInterfaceType::MultipleDocuments: {
                log("Log::System::createSystem() MultipleDocuments");
                s_system = new MDISystemWin32(hInstance);
                break;
            }
            case ApplicationInterfaceType::DialogBased: {
                log("Log::System::createSystem() DialogBased");

                break;
            }
            case ApplicationInterfaceType::MultpleTopLevelDocuments: {
                log("Log::System::createSystem() MultpleTopLevelDocuments");

                break;
            }
            default: {
                log("Log::System::createSystem() SingleDocuments");
                s_system = new SDISystemWin32(hInstance);
            }
        }
        success = s_system != NULL ? TRUE : FALSE;
    }
    else {
        success = FALSE;
    }

    if (success) {
        success = s_system->createApplication();
    }

    return s_system;
}
int System::disposeSystem()
{
    BOOL nResult = FALSE;
    if (s_system) {
        delete s_system;
        s_system = NULL;
    }
    else {
        nResult = TRUE;
    }
    return (int)nResult;
}
System* System::getSystem() { return s_system; }
