#include "DCK_pane.h"
#include "DCK_dockingmanager.h"

#include "fault.h"
#include "log.h"

using namespace mycad;

DockingManager::DockingManager()
    : ObjectManager()
{
    log("Log::DockingManager::DockingManager()");
}
DockingManager::~DockingManager()
{
	log("Log::DockingManager::~DockingManager()");
}
