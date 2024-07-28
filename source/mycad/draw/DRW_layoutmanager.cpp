#include "CMD_commandmanager.h"

#include "DRW_layoutmanager.h"
#include "DRW_layout.h"

#include "fault.h"
#include "log.h"

#include <cassert>

using namespace mycad;

LayoutManager::LayoutManager(CommandManager* commandmanager, DataBlocks* datablocks, SIZE size)
    : m_Size(size),
    TILEMODE(1),
    m_lpCommandManager(commandmanager),
    m_lpDataBlocks(datablocks),
    m_lpModelLayout(nullptr)
{
    log("Log::LayoutManager::LayoutManager()");

    m_lpModelLayout = new ModelLayout(0, "Model", m_Size, m_lpCommandManager, m_lpDataBlocks);//Create Model layout and add to vector,...
    m_Objects.push_back(m_lpModelLayout);

    m_lpActiveObject = new PaperLayout(1, "Layout1", m_Size, m_lpCommandManager, m_lpDataBlocks);//...create Paper layout 1 and add to vector,...
    m_Objects.push_back(m_lpActiveObject);

    PaperLayout* paperLayout2 = new PaperLayout(2, "Layout2", m_Size, m_lpCommandManager, m_lpDataBlocks);//...create Paper layout 2 and add to vector,...
    m_Objects.push_back(paperLayout2);
}

bool LayoutManager::setTilemode(int tilemode)
{
    if (getActiveObject() == nullptr) {
        return false;
    }

    if (TILEMODE == tilemode) {
        return false;
    }

    TILEMODE = tilemode;
    getActiveObject()->size(m_Size);//Resize in case size has changed since layout changed.

    return true;
}
int LayoutManager::getTilemode() { return TILEMODE; }

BOOL LayoutManager::addObject(int index, char* name)
{
    BOOL nSuccess = FALSE;
    PaperLayout* layout = new PaperLayout(index, name, m_Size, m_lpCommandManager, m_lpDataBlocks);
    if (layout) {
        if (!getObjectFound(layout)) {
            m_Objects.push_back(layout);
            nSuccess = TRUE;
        }
    }
    return nSuccess;
}

Layout* LayoutManager::setActiveLayout(unsigned int index)//Sets active layout by index, returns active layout.
{
    assert(index < m_Objects.size());
    if (index == 0) {
        TILEMODE = 1;
        m_lpModelLayout->activate();//Redo entities so that black is white for Model layouts/white is black for Paper layouts.
        m_lpModelLayout->size(m_Size);//Resize before paint event.
        return m_lpModelLayout;
    }
    else {
        Layout* layout = m_Objects[index];
        //if (layout != m_lpActiveLayout) {
            if (getObjectFound(layout)) {
                m_lpActiveObject = layout;
                TILEMODE = 0;
                m_lpActiveObject->activate();//Redo entities so that black is white for Model layouts/white is black for Paper layouts.
                m_lpActiveObject->size(m_Size);//Resize before paint event.
                return m_lpActiveObject;
            }
        //}
    }
    return nullptr;
}
Layout* LayoutManager::setActiveLayout(const char* name)//Sets active layout by name, returns active layout.
{
    for (unsigned int i = 0; i < m_Objects.size(); ++i) {
        if (::strcmp(m_Objects[i]->getName(), name) == 0) {
            return setActiveLayout(i);
        }
    }
    return nullptr;
}
Layout* LayoutManager::getActiveObject() const//Returns active layout. TODO - solve error
{
    if (m_lpActiveObject == nullptr) {
        return nullptr;
    }

    Layout* lpLayout = m_lpActiveObject;

    if (TILEMODE == 1) {//???????
        lpLayout = m_lpModelLayout;
    }
    else {
        lpLayout = m_lpActiveObject;
    }

    return lpLayout;
}

//Layout* LayoutManager::Item(char* name)//Returns layout by name.
//{
//    for (unsigned int i = 0; i < m_Objects.size(); ++i) {
//        if (::strcmp(m_Objects[i]->getName(), name) == 0) {
//            return m_Objects[i];
//        }
//    }
//    return nullptr;
//}

Space* LayoutManager::getActiveSpace()
{
    if (getActiveObject() == nullptr) {
        return nullptr;
    }
    return getActiveObject()->getActiveSpace();
}

void LayoutManager::size(int width, int height)
{
    m_Size.cx = width;
    m_Size.cy = height;

    if (getActiveObject() == nullptr) {
        return;
    }

    getActiveObject()->size(m_Size);
}
