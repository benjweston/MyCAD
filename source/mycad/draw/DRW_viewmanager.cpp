#include "DRW_viewmanager.h"
#include "DRW_view.h"

#include "fault.h"
#include "log.h"

#include <cassert>

using namespace mycad;

ViewManager::ViewManager()
    : ObjectManager()
{
    log("Log::ViewManager::ViewManager()");

    m_Objects.push_back(new View("Top", View::PresetView::Top));
    m_Objects.push_back(new View("Bottom", View::PresetView::Bottom));
    m_Objects.push_back(new View("Left", View::PresetView::Left));
    m_Objects.push_back(new View("Right", View::PresetView::Right));
    m_Objects.push_back(new View("Front", View::PresetView::Front));
    m_Objects.push_back(new View("Back", View::PresetView::Back));
    m_Objects.push_back(new View("SWIsometric", View::PresetView::SWIsometric));
    m_Objects.push_back(new View("SEIsometric", View::PresetView::SEIsometric));
    m_Objects.push_back(new View("NEIsometric", View::PresetView::NEIsometric));
    m_Objects.push_back(new View("NWIsometric", View::PresetView::NWIsometric));
}
