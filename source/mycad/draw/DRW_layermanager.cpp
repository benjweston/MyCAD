#include "DRW_layer.h"
#include "DRW_layermanager.h"

#include "log.h"

#include <cassert>

using namespace mycad;

LayerManager::LayerManager()
    : ObjectManager()
{
    log("Log::LayerManager::LayerManager()");
}

