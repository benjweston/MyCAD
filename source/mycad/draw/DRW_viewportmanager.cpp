#include "DRW_viewportmanager.h"
#include "DRW_viewport.h"

#include "fault.h"
#include "log.h"

#include <cassert>

using namespace mycad;

ViewportManager::ViewportManager()
    : ObjectManager(),
    m_enViewportConfiguration(Configuration::One)
{
    log("Log::ViewportManager::ViewportManager()");

    setViewportConfiguration(m_enViewportConfiguration);
    m_lpActiveObject = m_Objects[0];
}
ViewportManager::~ViewportManager()
{
    log("Log::ViewportManager::~ViewportManager()");

    m_Splitters.erase(m_Splitters.begin(), m_Splitters.end());
    m_Splitters.clear();

    m_Objects.erase(m_Objects.begin(), m_Objects.end());
    m_Objects.clear();

    delete m_lpActiveObject;
    m_lpActiveObject = nullptr;
}

void ViewportManager::setViewportConfiguration(ViewportManager::Configuration configuration)
{
    m_Splitters.erase(m_Splitters.begin(), m_Splitters.end());

    unsigned int nViewports = 1;
    switch (configuration) {
        case ViewportManager::Configuration::One: {
            nViewports = 1;
            break;
        }
        case ViewportManager::Configuration::TwoHorizontal: {
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.5f));
            nViewports = 2;
            break;
        }
        case ViewportManager::Configuration::TwoVertical: {
            m_Splitters.push_back(new Splitter(Orientation::Vertical, 0.5f));
            nViewports = 2;
            break;
        }
        case ViewportManager::Configuration::ThreeRight:
        case ViewportManager::Configuration::ThreeLeft: {
            m_Splitters.push_back(new Splitter(Orientation::Vertical, 0.5f));
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.5f));
            nViewports = 3;
            break;
        }
        case ViewportManager::Configuration::ThreeAbove:
        case ViewportManager::Configuration::ThreeBelow: {
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.5f));
            m_Splitters.push_back(new Splitter(Orientation::Vertical, 0.5f));
            nViewports = 3;
            break;
        }
        case ViewportManager::Configuration::ThreeVertical: {
            m_Splitters.push_back(new Splitter(Orientation::Vertical, 0.3333f));
            m_Splitters.push_back(new Splitter(Orientation::Vertical, 0.6666f));
            nViewports = 3;
            break;
        }
        case ViewportManager::Configuration::ThreeHorizontal: {
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.3333f));
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.6666f));
            nViewports = 3;
            break;
        }
        case ViewportManager::Configuration::FourEqual: {
            m_Splitters.push_back(new Splitter(Orientation::Vertical, 0.5f));
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.5f));
            nViewports = 4;
            break;
        }
        case ViewportManager::Configuration::FourRight: {
            m_Splitters.push_back(new Splitter(Orientation::Vertical, 0.75f));
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.3333f));
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.6666f));
            nViewports = 4;
            break;
        }
        case ViewportManager::Configuration::FourLeft: {
            m_Splitters.push_back(new Splitter(Orientation::Vertical, 0.25f));
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.3333f));
            m_Splitters.push_back(new Splitter(Orientation::Horizontal, 0.6666f));
            nViewports = 4;
            break;
        }
        default: {
            nViewports = 1;
            break;
        }
    }

    if (m_Objects.size() > nViewports) {
        while (m_Objects.size() > nViewports) {
            m_Objects.pop_back();
        }
    }
    else if (m_Objects.size() < nViewports) {
        while (m_Objects.size() < nViewports) {
            m_Objects.push_back(new MViewport());
        }
    }

    m_enViewportConfiguration = configuration;
}
ViewportManager::Configuration ViewportManager::getViewportConfiguration() const { return m_enViewportConfiguration; }

const std::vector<MViewport*> &ViewportManager::getViewports() const { return m_Objects; }
const std::vector<Splitter*> &ViewportManager::getSplitters() const { return m_Splitters; }

void ViewportManager::size(long x, long y, long width, long height)
{
    switch (m_enViewportConfiguration) {
        case ViewportManager::Configuration::One: {
            m_Objects[0]->size(x, y, width, height);
            break;
        }
        case ViewportManager::Configuration::TwoVertical: {
            long cx = width;
            float fx = cx * m_Splitters[0]->getRatio();
            long dx = static_cast<long>(fx);

            m_Splitters[0]->size(x + dx, y, 0, width);

            m_Objects[0]->size(x, y, dx, height);
            m_Objects[1]->size(m_Splitters[0]->right, y, cx - dx, height);
            break;
        }
        case ViewportManager::Configuration::TwoHorizontal: {
            long cy = height;
            float fy = cy * m_Splitters[0]->getRatio();
            long dy = static_cast<long>(fy);

            m_Splitters[0]->size(x, y + dy, width, 0);

            m_Objects[0]->size(x, y, width, dy);
            m_Objects[1]->size(x, m_Splitters[0]->bottom, width, cy - dy);
            break;
        }
        case ViewportManager::Configuration::ThreeRight: {
            long cx = width;
            float fx = cx * m_Splitters[0]->getRatio();
            long dx = static_cast<long>(fx);
            long cy = height;
            float fy = cy * m_Splitters[1]->getRatio();
            long dy = static_cast<long>(fy);

            m_Splitters[0]->size(x + dx, y, 0, height);
            m_Splitters[1]->size(x, y + dy, dx, 0);

            m_Objects[0]->size(x, y, dx, dy);
            m_Objects[1]->size(x, m_Splitters[1]->bottom, dx, cy - dy);
            m_Objects[2]->size(m_Splitters[0]->right, y, cx - dx, height);
            break;
        }
        case ViewportManager::Configuration::ThreeLeft: {
            long cx = width;
            float fx = cx * m_Splitters[0]->getRatio();
            long dx = static_cast<long>(fx);
            long cy = height;
            float fy = cy * m_Splitters[1]->getRatio();
            long dy = static_cast<long>(fy);

            m_Splitters[0]->size(x + dx, y, 0, height);
            m_Splitters[1]->size(x + dx + 0, y + dy, cx - dx, 0);

            m_Objects[0]->size(x, y, dx, height);
            m_Objects[1]->size(m_Splitters[0]->right, y, cx - dx, dy);
            m_Objects[2]->size(m_Splitters[0]->right, m_Splitters[1]->bottom, cx - dx, cy - dy);
            break;
        }
        case ViewportManager::Configuration::ThreeAbove: {
            long cx = width;
            float fx = cx * m_Splitters[1]->getRatio();
            long dx = static_cast<long>(fx);
            long cy = height;
            float fy = cy * m_Splitters[0]->getRatio();
            long dy = static_cast<long>(fy);

            m_Splitters[0]->size(x, y + dy, width, 0);
            m_Splitters[1]->size(x + dx, y, 0, dy);

            m_Objects[0]->size(x, y, dx, dy);
            m_Objects[1]->size(m_Splitters[1]->right, y, cx - dx, dy);
            m_Objects[2]->size(x, m_Splitters[0]->bottom, width, cy - dy);
            break;
        }
        case ViewportManager::Configuration::ThreeBelow: {
            long cx = width;
            float fx = cx * m_Splitters[0]->getRatio();
            long dx = static_cast<long>(fx);
            long cy = height;
            float fy = cy * m_Splitters[1]->getRatio();
            long dy = static_cast<long>(fy);

            m_Splitters[0]->size(x, y + dy, width, 0);
            m_Splitters[1]->size(x + dx, y + dy + 0, 0, height - dy - 0);

            m_Objects[0]->size(x, y, width, cy - dy);
            m_Objects[1]->size(x, m_Splitters[0]->bottom, dx, cy - dy);
            m_Objects[2]->size(m_Splitters[1]->right, m_Splitters[0]->bottom, cx - dx, cy - dy);
            break;
        }
        case ViewportManager::Configuration::ThreeVertical: {
            long cx = width;
            float fx[2] = { static_cast<float>(cx) * m_Splitters[0]->getRatio(),static_cast<float>(cx) * m_Splitters[1]->getRatio() };
            long dx[2] = { static_cast<long>(fx[0]),static_cast<long>(fx[1]) };

            m_Splitters[0]->size(x + dx[0], y, 0, height);
            m_Splitters[1]->size(x + dx[1] + 0, y, 0, height);

            m_Objects[0]->size(x, y, dx[0], height);
            m_Objects[1]->size(m_Splitters[0]->right, y, dx[1] - dx[0], height);
            m_Objects[2]->size(m_Splitters[1]->right, y, cx - dx[1], height);
            break;
        }
        case ViewportManager::Configuration::ThreeHorizontal: {
            long cy = height;
            float fy[2] = { static_cast<float>(cy) * m_Splitters[0]->getRatio(),static_cast<float>(cy) * m_Splitters[1]->getRatio() };
            long dy[2] = { static_cast<long>(fy[0]),static_cast<long>(fy[1]) };

            m_Splitters[0]->size(x, y + dy[0], width, 0);
            m_Splitters[1]->size(x, y + dy[1] + 0, width, 0);

            m_Objects[0]->size(x, y, width, dy[0]);
            m_Objects[1]->size(x, m_Splitters[0]->bottom, width, dy[1] - dy[0]);
            m_Objects[2]->size(x, m_Splitters[1]->bottom, width, cy - dy[1]);
            break;
        }
        case ViewportManager::Configuration::FourEqual: {
            long cx = width;
            float fx = cx * m_Splitters[0]->getRatio();
            long dx = static_cast<long>(fx);
            long cy = height;
            float fy = cy * m_Splitters[1]->getRatio();
            long dy = static_cast<long>(fy);

            m_Splitters[0]->size(x + dx,y, 0, height);
            m_Splitters[1]->size(x, y + dy, width, 0);

            m_Objects[0]->size(x, y, dx, dy);
            m_Objects[1]->size(m_Splitters[0]->right, y, cx - dx, dy);
            m_Objects[2]->size(x, m_Splitters[1]->bottom, dx, cy - dy);
            m_Objects[3]->size(m_Splitters[0]->right, m_Splitters[1]->bottom, cx - dx, cy - dy);
            break;
        }
        case ViewportManager::Configuration::FourRight: {
            long cx = width;
            float fx = cx * m_Splitters[0]->getRatio();
            long dx = static_cast<long>(fx);
            long cy = height;
            float fy[2] = { cy * m_Splitters[1]->getRatio(),cy * m_Splitters[2]->getRatio() };
            long dy[2] = { static_cast<long>(fy[0]),static_cast<long>(fy[1]) };

            m_Splitters[0]->size(x + dx, y, 0, height);
            m_Splitters[1]->size(x + dx + 0, y + dy[0], width - dx - 0, 0);
            m_Splitters[2]->size(x + dx + 0, y + dy[1] + 0, width - dx - 0, 0);

            m_Objects[0]->size(x, y, dx, height);
            m_Objects[1]->size(m_Splitters[0]->right, y, cx - dx, dy[0]);
            m_Objects[2]->size(m_Splitters[0]->right, m_Splitters[1]->bottom, cx - dx, dy[1] - dy[0]);
            m_Objects[3]->size(m_Splitters[0]->right, m_Splitters[2]->bottom, cx - dx, cy - dy[1]);
            break;
        }
        case ViewportManager::Configuration::FourLeft: {
            long cx = width;
            float fx = cx * m_Splitters[0]->getRatio();
            long dx = static_cast<long>(fx);
            long cy = height;
            float fy[2] = { cy * m_Splitters[1]->getRatio(),cy * m_Splitters[2]->getRatio() };
            long dy[2] = { static_cast<long>(fy[0]),static_cast<long>(fy[1]) };

            m_Splitters[0]->size(x + dx, y, 0, height);
            m_Splitters[1]->size(x, y + dy[0], dx, 0);
            m_Splitters[2]->size(x, y + dy[1] + 0, dx, 0);

            m_Objects[0]->size(m_Splitters[0]->right, y, cx - dx, height);
            m_Objects[1]->size(x, y, dx, dy[0]);
            m_Objects[2]->size(x, m_Splitters[1]->bottom, dx, dy[1] - dy[0]);
            m_Objects[3]->size(x, m_Splitters[2]->bottom, dx, cy - dy[1]);
            break;
        }
        default: {
            break;
        }
    }
}
void ViewportManager::size(POINT position, SIZE dimensions) { size(position.x, position.y, dimensions.cx, dimensions.cy); }
void ViewportManager::size(long width, long height) { 0, 0, width, height; }
void ViewportManager::size(SIZE dimensions) { size({ 0,0 }, dimensions); }
