#include "DRW_space.h"

#include "CMD_commands.h"

#include "fault.h"
#include "log.h"

#include "glExtension.h"

#include <windowsx.h>

using namespace mycad;

Colour3<float> Space::s_c3fBackgroundColour{ 1.f, 1.f, 1.f/*, 1.f*/ };//White

Space::Space(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* uiviewport)
	: m_lpCommandManager(commandmanager),
	m_lpDataBlocks(datablocks),
	m_lpViewportManager(viewportmanager),
	m_lpUserInterface(uiviewport),
	m_rcDimensions({ 0,0,0,0 }),
	m_ptCursor({ 0,0 }),
	m_nBorderWidth(3),
	m_nVBOid_vertices(0),
	m_nVBOid_indices(0),
	m_lpText_Properties(0)
{
	log("Log::Space::Space(ViewportManager *viewportmanager, PViewport *ui, PViewport *paper)");

	m_lpActiveViewport = m_lpViewportManager->getViewports()[0];
}
Space::~Space()
{
	log("Log::Space::~Space()");
}

void Space::setBackgroundColour(Colour3<float> colour) { s_c3fBackgroundColour = colour; }

Viewport* Space::getActiveViewport() const { return m_lpActiveViewport; }

BOOL Space::setActiveViewport(ViewportManager* viewportmanager, POINT point)
{
	BOOL nSuccess = FALSE;
	for (MViewport* viewport : viewportmanager->getViewports()) {
		if (::PtInRect(viewport,point) == TRUE) {
			if (viewport->getViewportManager() == nullptr) {
				nSuccess = TRUE;
				if (nSuccess != 0) {
					if (viewport != m_lpActiveViewport) {
						m_lpActiveViewport = viewport;
						m_lpActiveViewport->mousemove(point);//Make sure the cursor is in the correct position.
						setProperties();
					}
				}
			}
			else {
				setActiveViewport(viewport->getViewportManager(), point);
			}
		}
	}
	return nSuccess;
}
BOOL Space::setActiveViewport(POINT point)
{
	BOOL nSuccess = FALSE;
	if (m_lpActiveViewport == nullptr) {//Is there an active viewport?
		nSuccess = setActiveViewport(m_lpViewportManager, point);//Make the viewport we're pointing at the active viewport.
	}
	else if (::PtInRect(m_lpActiveViewport, point) == FALSE) {//Yes, there is an active viewport, but we are not clicking in it.
		nSuccess = setActiveViewport(m_lpViewportManager, point);//Find the viewport we ARE pointing at and make it the active viewport.
	}
	return nSuccess;
}

void Space::setViewportConfiguration(ViewportManager::LPVIEWPORTDATA configdat)
{
	if (configdat->enContext == ViewportManager::Context::Display) {
		for (MViewport* lpViewport : m_lpViewportManager->getViewports()) {
			lpViewport->setViewportManager(nullptr);
		}
		m_lpViewportManager->setViewportConfiguration(configdat->enConfiguration);
		size(m_lpViewportManager, m_rcDimensions.left, m_rcDimensions.top, m_rcDimensions.width, m_rcDimensions.height);
		m_lpActiveViewport = m_lpViewportManager->getViewports()[0];
	}
	else if (configdat->enContext == ViewportManager::Context::CurrentViewport) {
		if (m_lpActiveViewport->getViewportManager() == nullptr) {
			m_lpActiveViewport->setViewportManager(new ViewportManager());
		}
		m_lpActiveViewport->getViewportManager()->setViewportConfiguration(configdat->enConfiguration);
		size(m_lpActiveViewport->getViewportManager(), m_lpActiveViewport->left, m_lpActiveViewport->top, m_lpActiveViewport->width, m_lpActiveViewport->height);
		m_lpActiveViewport = m_lpActiveViewport->getViewportManager()->getViewports()[0];
	}
}

void Space::setView(View* view)
{
	m_lpActiveViewport->getCamera()->setView(view);
	setProperties();
}
bool Space::setPresetView(const View::PresetView presetview)
{
	bool bResult = m_lpActiveViewport->getCamera()->setPresetView(presetview);
	if (bResult == true) {
		setProperties();
	}
	return bResult;
}
bool Space::setVisualStyle(const View::VisualStyle visualstyle)
{
	bool bResult = m_lpActiveViewport->getCamera()->setVisualStyle(visualstyle);
	if (bResult == true) {
		setProperties();
	}
	return bResult;
}

bool Space::setPerspective(const bool perspective)
{
	bool bResult = m_lpActiveViewport->getCamera()->setPerspective(perspective);
	if (bResult == true) {
		setProperties();
	}
	return bResult;
}
bool Space::getPerspective() { return m_lpActiveViewport->getCamera()->getPerspective(); }

BOOL Space::PtInRect(POINT point) { return ::PtInRect(&m_rcDimensions, point); }

void Space::drawActiveBorder(int left, int top, int right, int bottom, int width, Colour4<float> colour)
{
	RECT rc = { left,top,right,bottom };
	drawActiveBorder(&rc, width, colour);
}
void Space::drawActiveBorder(LPRECT lprc, int width, Colour4<float> colour)
{
	RECT outer = *lprc;
	RECT inner = {
		outer.left + width,
		outer.top + width,
		outer.right - width,
		outer.bottom - width,
	};

	Vector2<GLint>vertices[10] = {
		{(GLint)outer.left, (GLint)outer.top},
		{(GLint)inner.left, (GLint)inner.top},
		{(GLint)outer.left, (GLint)outer.bottom},
		{(GLint)inner.left, (GLint)inner.bottom},
		{(GLint)outer.right, (GLint)outer.bottom},
		{(GLint)inner.right, (GLint)inner.bottom},
		{(GLint)outer.right, (GLint)outer.top},
		{(GLint)inner.right, (GLint)inner.top},
		{(GLint)outer.left, (GLint)outer.top},
		{(GLint)inner.left, (GLint)inner.top}
	};
	Colour4<GLfloat>colours[10] = {
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a}
	};

	glGenBuffersARB(1, &m_nVBOid_vertices);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glPolygonMode(GL_FRONT, GL_FILL);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)(10));
	glPolygonMode(GL_FRONT, GL_LINES);//Restore polygon mode to GL_FRONT

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}

void Space::drawBorders(int left, int top, int right, int bottom, Colour4<float> colour)
{
	RECT rc = { left,top,right,bottom };
	drawBorders(&rc, colour);
}
void Space::drawBorders(LPRECT lprc, Colour4<float> colour)
{
	Vector2<GLint>vertices[4] = {
		Vector2<GLint>((GLint)lprc->left, (GLint)lprc->bottom),
		Vector2<GLint>((GLint)lprc->left, (GLint)lprc->top),
		Vector2<GLint>((GLint)lprc->right, (GLint)lprc->top),
		Vector2<GLint>((GLint)lprc->right, (GLint)lprc->bottom),
	};

	Colour4<GLfloat>colours[4] = {
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a},
		{(GLfloat)colour.r,(GLfloat)colour.g,(GLfloat)colour.b,(GLfloat)colour.a}
	};

	glGenBuffersARB(1, &m_nVBOid_vertices);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(4));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}

void Space::mousemove(POINT position)
{
	m_ptCursor = position;
	m_lpActiveViewport->mousemove(position);
}
void Space::paint(ViewportManager* viewportmanager)//Recursive function that iterates through the viewports tree painting viewports, returns if the viewports viewportmanager is null.
{
	for (MViewport* lpViewport : viewportmanager->getViewports()) {//Draw viewport borders...
		if (lpViewport->getViewportManager() == nullptr) {//Draw viewport...
			glViewport(m_lpUserInterface->left, m_lpUserInterface->top, m_lpUserInterface->width, m_lpUserInterface->height);

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(m_lpUserInterface->getCamera()->getProjectionMatrixElementsf());
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(m_lpUserInterface->getCamera()->getModelViewMatrixElementsf());//Could use glLoadIdentity() here, but using the cameras modelview matrix allows us to scale the UI

			drawBorders(lpViewport);

			glViewport(lpViewport->left, lpViewport->top, lpViewport->width, lpViewport->height);

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(lpViewport->getCamera()->getProjectionMatrixElementsf());
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(lpViewport->getCamera()->getModelViewMatrixElementsf());

			//m_Grid.draw({ lpViewport->width,lpViewport->height }, 1.0, true, true, true, GLGrid::Style::Axislines_And_Gridlines);//Draw grid...
			if (lpViewport->getGridVisible()) {
				bool xy = lpViewport->getGridXYVisible();
				bool yz = lpViewport->getGridYZVisible();
				bool zx = lpViewport->getGridZXVisible();
				m_Grid.draw(lpViewport->extents, lpViewport->getCamera()->getMagnification(), xy, yz, zx, GLGrid::Style::Axislines_And_Gridlines);
			}

			drawEntities();//Draw entities...
		}
		else {//...or step into viewportmanagers viewport collection
			paint(lpViewport->getViewportManager());
		}
	}
}
void Space::paint() { paint(m_lpViewportManager); }
void Space::size(ViewportManager* viewportmanager, int x, int y, int width, int height)
{
	switch (viewportmanager->getViewportConfiguration()) {
		case ViewportManager::Configuration::One: {
			viewportmanager->getViewports()[0]->size(x, y, width, height);
			break;
		}
		case ViewportManager::Configuration::TwoVertical: {
			int cx = width;
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;

			viewportmanager->getSplitters()[0]->size(x + dx, y, 0, width);

			viewportmanager->getViewports()[0]->size(x, y, dx, height);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, height);
			break;
		}
		case ViewportManager::Configuration::TwoHorizontal: {
			int cy = height;
			float fy = cy * viewportmanager->getSplitters()[0]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x, y + dy, width, 0);

			viewportmanager->getViewports()[0]->size(x, y, width, dy);
			viewportmanager->getViewports()[1]->size(x, viewportmanager->getSplitters()[0]->bottom, width, cy - dy);
			break;
		}
		case ViewportManager::Configuration::ThreeRight: {
			int cx = width;
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height;
			float fy = cy * viewportmanager->getSplitters()[1]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x + dx, y, 0, height);
			viewportmanager->getSplitters()[1]->size(x, y + dy, dx, 0);

			viewportmanager->getViewports()[0]->size(x, y, dx, dy);
			viewportmanager->getViewports()[1]->size(x, viewportmanager->getSplitters()[1]->bottom, dx, cy - dy);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, height);
			break;
		}
		case ViewportManager::Configuration::ThreeLeft: {
			int cx = width;
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height;
			float fy = cy * viewportmanager->getSplitters()[1]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x + dx, y, 0, height);
			viewportmanager->getSplitters()[1]->size(x + dx + 0, y + dy, cx - dx, 0);

			viewportmanager->getViewports()[0]->size(x, y, dx, height);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, dy);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[0]->right, viewportmanager->getSplitters()[1]->bottom, cx - dx, cy - dy);
			break;
		}
		case ViewportManager::Configuration::ThreeAbove: {
			int cx = width;
			float fx = cx * viewportmanager->getSplitters()[1]->getRatio();
			int dx = (int)fx;
			int cy = height;
			float fy = cy * viewportmanager->getSplitters()[0]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x, y + dy, width, 0);
			viewportmanager->getSplitters()[1]->size(x + dx, y, 0, dy);

			viewportmanager->getViewports()[0]->size(x, y, dx, dy);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[1]->right, y, cx - dx, dy);
			viewportmanager->getViewports()[2]->size(x, viewportmanager->getSplitters()[0]->bottom, width, cy - dy);
			break;
		}
		case ViewportManager::Configuration::ThreeBelow: {
			int cx = width;
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height;
			float fy = cy * viewportmanager->getSplitters()[1]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x, y + dy, width, 0);
			viewportmanager->getSplitters()[1]->size(x + dx, y + dy + 0, 0, height - dy - 0);

			viewportmanager->getViewports()[0]->size(x, y, width, cy - dy);
			viewportmanager->getViewports()[1]->size(x, viewportmanager->getSplitters()[0]->bottom, dx, cy - dy);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[1]->right, viewportmanager->getSplitters()[0]->bottom, cx - dx, cy - dy);
			break;
		}
		case ViewportManager::Configuration::ThreeVertical: {
			int cx = width;
			float fx[2] = { (float)cx * viewportmanager->getSplitters()[0]->getRatio(),(float)cx * viewportmanager->getSplitters()[1]->getRatio() };
			int dx[2] = { (int)fx[0],(int)fx[1] };

			viewportmanager->getSplitters()[0]->size(x + dx[0], y, 0, height);
			viewportmanager->getSplitters()[1]->size(x + dx[1] + 0, y, 0, height);

			viewportmanager->getViewports()[0]->size(x, y, dx[0], height);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, dx[1] - dx[0], height);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[1]->right, y, cx - dx[1], height);
			break;
		}
		case ViewportManager::Configuration::ThreeHorizontal: {
			int cy = height;
			float fy[2] = { (float)cy * viewportmanager->getSplitters()[0]->getRatio(),(float)cy * viewportmanager->getSplitters()[1]->getRatio() };
			int dy[2] = { (int)fy[0],(int)fy[1] };

			viewportmanager->getSplitters()[0]->size(x, y + dy[0], width, 0);
			viewportmanager->getSplitters()[1]->size(x, y + dy[1] + 0, width, 0);

			viewportmanager->getViewports()[0]->size(x, y, width, dy[0]);
			viewportmanager->getViewports()[1]->size(x, viewportmanager->getSplitters()[0]->bottom, width, dy[1] - dy[0]);
			viewportmanager->getViewports()[2]->size(x, viewportmanager->getSplitters()[1]->bottom, width, cy - dy[1]);
			break;
		}
		case ViewportManager::Configuration::FourEqual: {
			int cx = width;
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height;
			float fy = cy * viewportmanager->getSplitters()[1]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x + dx, y, 0, height);
			viewportmanager->getSplitters()[1]->size(x, y + dy, width, 0);

			viewportmanager->getViewports()[0]->size(x, y, dx, dy);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, dy);
			viewportmanager->getViewports()[2]->size(x, viewportmanager->getSplitters()[1]->bottom, dx, cy - dy);
			viewportmanager->getViewports()[3]->size(viewportmanager->getSplitters()[0]->right, viewportmanager->getSplitters()[1]->bottom, cx - dx, cy - dy);
			break;
		}
		case ViewportManager::Configuration::FourRight: {
			int cx = width;
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height;
			float fy[2] = { cy * viewportmanager->getSplitters()[1]->getRatio(),cy * viewportmanager->getSplitters()[2]->getRatio() };
			int dy[2] = { (int)fy[0],(int)fy[1] };

			viewportmanager->getSplitters()[0]->size(x + dx, y, 0, height);
			viewportmanager->getSplitters()[1]->size(x + dx + 0, y + dy[0], width - dx - 0, 0);
			viewportmanager->getSplitters()[2]->size(x + dx + 0, y + dy[1] + 0, width - dx - 0, 0);

			viewportmanager->getViewports()[0]->size(x, y, dx, height);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, dy[0]);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[0]->right, viewportmanager->getSplitters()[1]->bottom, cx - dx, dy[1] - dy[0]);
			viewportmanager->getViewports()[3]->size(viewportmanager->getSplitters()[0]->right, viewportmanager->getSplitters()[2]->bottom, cx - dx, cy - dy[1]);
			break;
		}
		case ViewportManager::Configuration::FourLeft: {
			int cx = width;
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height;
			float fy[2] = { cy * viewportmanager->getSplitters()[1]->getRatio(),cy * viewportmanager->getSplitters()[2]->getRatio() };
			int dy[2] = { (int)fy[0],(int)fy[1] };

			viewportmanager->getSplitters()[0]->size(x + dx, y, 0, height);
			viewportmanager->getSplitters()[1]->size(x, y + dy[0], dx, 0);
			viewportmanager->getSplitters()[2]->size(x, y + dy[1] + 0, dx, 0);

			viewportmanager->getViewports()[0]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, height);
			viewportmanager->getViewports()[1]->size(x, y, dx, dy[0]);
			viewportmanager->getViewports()[2]->size(x, viewportmanager->getSplitters()[1]->bottom, dx, dy[1] - dy[0]);
			viewportmanager->getViewports()[3]->size(x, viewportmanager->getSplitters()[2]->bottom, dx, cy - dy[1]);
			break;
		}
		default: {
			break;
		}
	}

	for (MViewport* lpViewport : viewportmanager->getViewports()) {
		if (lpViewport->getViewportManager() != nullptr) {
			size(lpViewport->getViewportManager(), lpViewport->left, lpViewport->top, lpViewport->width, lpViewport->height);
		}
	}
}
void Space::size(int x, int y, int width, int height)
{
	m_rcDimensions.SetGLRect(x, y, width, height);
	size(m_lpViewportManager, x, y, width, height);
}
void Space::size(SIZE lsize) { size(m_rcDimensions.left, m_rcDimensions.top, lsize.cx, lsize.cy); }

void Space::initProperties()
{
	const char* pchFont = "C:/Windows/Fonts/Tahoma.ttf";
	const int nTextHeight = 12;
	//const Colour4<float> c3fColour = { 1.f,1.f,1.f,1.f };

	m_lpText_Properties = new GLText(pchFont, nTextHeight);
	//m_lpText_Properties->setTextColour(c3fColour);

	setProperties();

	m_lpText_Properties->setPosition({ -m_lpActiveViewport->width / 2 + 3 + m_nBorderWidth, m_lpActiveViewport->height / 2 - m_lpText_Properties->getSize().cy - 3 - m_nBorderWidth });
}
void Space::setProperties()
{
	if (m_lpText_Properties) {
		char buf[256] = "[+][";

		::strcat_s(buf, 64, m_lpActiveViewport->getCamera()->viewToString(m_lpActiveViewport->getCamera()->getPresetView()));
		::strcat_s(buf, 256, "][");

		if (m_lpActiveViewport->getCamera()->getPerspective() == false) {
			::strcat_s(buf, 64, "Orthographic");
		}
		else {
			::strcat_s(buf, 64, "Perspective");
		}
		::strcat_s(buf, 256, "][");

		::strcat_s(buf, 64, m_lpActiveViewport->getCamera()->visualstyleToString(m_lpActiveViewport->getCamera()->getVisualStyle()));
		::strcat_s(buf, 256, "]");

		m_lpText_Properties->setText(buf);
	}
}

void Space::drawGrid(SIZE viewport, bool xy, bool yz, bool zx) { m_Grid.draw(viewport, 1.0, xy, yz, zx, GLGrid::Style::Axislines_And_Gridlines); }

void Space::drawEntity(Entity* ent)
{
	if (ent->getEntity() == nullptr) {
		return;
	}
	Colour3<float> c3fColour = Entity::IndexToVector3fColour(ent->getEntity()->color);
	//if (ent->getEntity()->color == 255) {//If colour is white...
	//	c3fColour = Entity::IndexToVector3fColour(7);//Then change colour to black.
	//}
	if (c3fColour == Colour3<float>{ 1.f, 1.f, 1.f }) {//If colour is white...
		ent->paint(Colour3<float>{ 0.f, 0.f, 0.f });//...set colour to black.
	}
	else {
		ent->paint(c3fColour);
	}
}
void Space::drawEntities()
{
	//glDisable(GL_BLEND);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	for (std::list<Entity*>::const_iterator it = m_lpDataBlocks->m_mBlock.ent.begin(); it != m_lpDataBlocks->m_mBlock.ent.end(); ++it) {
		drawEntity(*it);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//glEnable(GL_BLEND);
}



Colour3<float> PaperSpace::s_c3fCrosshairColour{ 0.f, 0.f, 0.f };//Black
bool PaperSpace::s_bTintForXYZ{ false };

PaperSpace::PaperSpace(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* uiviewport, PViewport* paperviewport, RECT* paper, RECT* shadow, RECT* margin, RECT* viewports)
	: Space(commandmanager, datablocks, viewportmanager, uiviewport),
	m_lpPaperViewport(paperviewport),
	m_lprcPaper(paper),
	m_lprcShadow(shadow),
	m_lprcMargin(margin),
	m_lprcViewports(viewports)
{
	log("Log::PaperSpace::PaperSpace()");
}
PaperSpace::~PaperSpace()
{
	log("Log::PaperSpace::~PaperSpace()");
}

void PaperSpace::setCrosshairColour(Colour3<float> colour, bool tintforxyz)
{
	s_c3fCrosshairColour = colour;
	s_bTintForXYZ = tintforxyz;
}
Colour3<float> PaperSpace::getCrosshairColour() { return s_c3fCrosshairColour; }

void PaperSpace::paint(ViewportManager* viewportmanager) { Space::paint(viewportmanager); }
void PaperSpace::size(SIZE lsize)
{
	POINT ptLeftTop = m_lpPaperViewport->getCamera()->Project((double)m_lprcViewports->left, (double)m_lprcViewports->top);
	POINT ptRightBottom = m_lpPaperViewport->getCamera()->Project((double)m_lprcViewports->right, (double)m_lprcViewports->bottom);

	//Move screen coordinates by half the screen size...
	POINT szLayout = { 2 * ptLeftTop.x + lsize.cx,2 * ptLeftTop.y + lsize.cy };
	POINT ptPosition = { szLayout.x / 2,szLayout.y / 2 };
	SIZE szSize = { ptRightBottom.x - ptLeftTop.x,ptRightBottom.y - ptLeftTop.y };

	//Send coordinates to space.
	Space::size(ptPosition.x, ptPosition.y, szSize.cx, szSize.cy);

	ptLeftTop = m_lpPaperViewport->getCamera()->Project((double)m_lprcPaper->left, (double)m_lprcPaper->top);
	ptRightBottom = m_lpPaperViewport->getCamera()->Project((double)m_lprcPaper->right, (double)m_lprcPaper->bottom);

	szLayout = { 2 * ptLeftTop.x + lsize.cx,2 * ptLeftTop.y + lsize.cy };
	ptPosition = { szLayout.x / 2,szLayout.y / 2 };
	szSize = { ptRightBottom.x - ptLeftTop.x,ptRightBottom.y - ptLeftTop.y };

	m_Paper.size(ptPosition.x, ptPosition.y, szSize.cx, szSize.cy);
}

void PaperSpace::drawShadow()
{
	Vector2<GLint>vertices[4] = {
		Vector2<GLint>((GLint)m_lprcShadow->left, (GLint)m_lprcShadow->bottom),
		Vector2<GLint>((GLint)m_lprcShadow->left, (GLint)m_lprcShadow->top),
		Vector2<GLint>((GLint)m_lprcShadow->right, (GLint)m_lprcShadow->bottom),
		Vector2<GLint>((GLint)m_lprcShadow->right, (GLint)m_lprcShadow->top),
	};

	Colour4<GLfloat> c4fColour = { 0.f, 0.f, 0.f, 1.f };
	Colour4<GLfloat>colours[4] = {
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a},
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a},
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a},
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a}
	};

	glGenBuffersARB(1, &m_nVBOid_vertices);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glPolygonMode(GL_FRONT, GL_FILL);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)(4));
	glPolygonMode(GL_FRONT, GL_LINES);//Restore polygon mode to GL_FRONT

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void PaperSpace::drawPaper()
{
	Vector2<GLint>vertices[4] = {
		Vector2<GLint>((GLint)m_lprcPaper->left, (GLint)m_lprcPaper->top),
		Vector2<GLint>((GLint)m_lprcPaper->right, (GLint)m_lprcPaper->top),
		Vector2<GLint>((GLint)m_lprcPaper->right, (GLint)m_lprcPaper->bottom),
		Vector2<GLint>((GLint)m_lprcPaper->left, (GLint)m_lprcPaper->bottom)
	};

	Colour4<GLfloat> c4fColour = { 1.f, 1.f, 1.f, 1.f };
	Colour4<GLfloat>colours[4] = {
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a},
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a},
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a},
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a}
	};

	unsigned int indices[4] = { 0, 1, 3, 2 };

	glGenBuffersARB(1, &m_nVBOid_vertices);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colours), colours);

	glGenBuffersARB(1, &m_nVBOid_indices);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nVBOid_indices);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nVBOid_indices);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glPolygonMode(GL_FRONT, GL_FILL);
	glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)(4), GL_UNSIGNED_INT, (GLvoid*)(0));
	glPolygonMode(GL_FRONT, GL_LINES);//Restore polygon mode to GL_FRONT

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void PaperSpace::drawMargin()
{
	Vector2<GLint>vertices[4] = {
		Vector2<GLint>((GLint)m_lprcMargin->left, (GLint)m_lprcMargin->bottom),
		Vector2<GLint>((GLint)m_lprcMargin->left, (GLint)m_lprcMargin->top),
		Vector2<GLint>((GLint)m_lprcMargin->right, (GLint)m_lprcMargin->top),
		Vector2<GLint>((GLint)m_lprcMargin->right, (GLint)m_lprcMargin->bottom)
	};

	Colour4<GLfloat> c4fColour = { .5f, .5f, .5f, 1.f };
	Colour4<GLfloat>colours[4] = {
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a},
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a},
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a},
		{(GLfloat)c4fColour.r,(GLfloat)c4fColour.g,(GLfloat)c4fColour.b,(GLfloat)c4fColour.a}
	};

	glGenBuffersARB(1, &m_nVBOid_vertices);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glLineStipple(2, 0xCCCC);
	glEnable(GL_LINE_STIPPLE);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(4));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_LINE_STIPPLE);
}



Colour3<float> PSpace::s_c3fUCSIconColour{ 0.f, 0.f, 1.f };//Blue

PSpace::PSpace(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* uiviewport, PViewport* paperviewport, RECT* paper, RECT* shadow, RECT* margin, RECT* viewports)
	: PaperSpace(commandmanager, datablocks, viewportmanager, uiviewport, paperviewport, paper, shadow, margin, viewports)
{
	log("Log::PSpace::PSpace()");
}
PSpace::~PSpace()
{
	log("Log::PSpace::~PSpace()");
}

void PSpace::setUCSIconColour(Colour3<float> colour) { s_c3fUCSIconColour = colour; }

Space::Context PSpace::getContext() const { return Context::PaperSpace; }

Viewport* PSpace::getActiveViewport() const { return m_lpPaperViewport; }

void PSpace::move(const double x, const double y, const double z) {
	m_lpPaperViewport->move(x, y, z);
	size(m_lpPaperViewport->extents);
}
void PSpace::rotate(double x, double y, double z) { UNREFERENCED_PARAMETER(x); UNREFERENCED_PARAMETER(y); UNREFERENCED_PARAMETER(z); }
void PSpace::zoom(const double z) {
	m_lpPaperViewport->zoom(z);
	size(m_lpPaperViewport->extents);
}

void PSpace::mousemove(POINT position)
{
	m_ptCursor = position;
	m_lpPaperViewport->mousemove(position);
	m_Paper.mousemove(position);
}
void PSpace::paint(ViewportManager* viewportmanager) { PaperSpace::paint(viewportmanager); }
void PSpace::paint()
{
	glViewport(m_lpPaperViewport->left, m_lpPaperViewport->top, m_lpPaperViewport->width, m_lpPaperViewport->height);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_lpPaperViewport->getCamera()->getProjectionMatrixElementsf());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_lpPaperViewport->getCamera()->getModelViewMatrixElementsf());

	drawShadow();
	drawPaper();
	drawMargin();

	paint(m_lpViewportManager);//Draw viewports and borders.

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_lpPaperViewport->getUICamera()->getProjectionMatrixElementsf());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_lpPaperViewport->getUICamera()->getModelViewMatrixElementsf());

	drawUCS();
	drawCommand();
	//drawPointer(s_c3fCrosshairColour);//Draw underlay pointer.

	//New AutoCAD (Grey background pointer on paperspace background)...
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	drawPointer({ 1.f - s_c3fCrosshairColour.r, 1.f - s_c3fCrosshairColour.g, 1.f - s_c3fCrosshairColour.b });//Draw underlay pointer.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//Return glBlendFunc to default values.
}

void PSpace::drawPointer(Colour3<float> colour)
{
	if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) == nullptr) {
		if (m_lpCommandManager->getActiveObject(CommandContext::COMMAND) == nullptr) {
			m_Pointer.draw(m_lpPaperViewport->getCursor(), colour, s_bTintForXYZ);
		}
		else
		{
			Command* command = m_lpCommandManager->getActiveObject(CommandContext::COMMAND);
			if (command->getID() == IDC_SELECT) {
				m_Pointer.drawCommand(m_lpPaperViewport->getCursor(), colour, s_bTintForXYZ);
			}
		}
	}
}
void PSpace::drawUCS()
{
	if (m_lpActiveViewport->getUCSVisible() == TRUE) {
		if (getActiveViewport()->getUCSOrigin() == TRUE) {
			glViewport(m_Paper.left, m_Paper.top, m_Paper.width, m_Paper.height);

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(m_Paper.getUICamera()->getProjectionMatrixElementsf());
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(m_Paper.getUICamera()->getModelViewMatrixElementsf());

			POINT position = { m_UCSIcon.getOffset(0) - m_Paper.width / 2, m_UCSIcon.getOffset(0) - m_Paper.height / 2 };

			//RECT rcBoundingbox = m_UCSIcon.getBoundingbox();
			//if ((m_lpActiveViewport->getUCSOrigin() != 0) &&
			//	(((position.x > (-slHalfViewport.cx / 2 - rcBoundingbox.left)) && (position.y > (-slHalfViewport.cy / 2 - rcBoundingbox.top))) &&
			//		((position.x < (slHalfViewport.cx / 2 - rcBoundingbox.right)) && (position.y < (slHalfViewport.cy / 2 - rcBoundingbox.bottom))))) {
			//	//position.x += 10;
			//	//position.y += 10;
			//}
			//else {
			//	//position = { (long)(10 - slHalfViewport.cx / 2), (long)(10 - slHalfViewport.cy / 2) };
			//}

			m_UCSIcon.draw(position, s_c3fUCSIconColour);
		}
		else {
			glViewport(m_lpPaperViewport->left, m_lpPaperViewport->top, m_lpPaperViewport->width, m_lpPaperViewport->height);

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(m_lpPaperViewport->getUICamera()->getProjectionMatrixElementsf());
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(m_lpPaperViewport->getUICamera()->getModelViewMatrixElementsf());

			POINT position = { m_UCSIcon.getOffset(0) - m_lpPaperViewport->width / 2, m_UCSIcon.getOffset(0) - m_lpPaperViewport->height / 2 };
			m_UCSIcon.draw(position, s_c3fUCSIconColour);
		}
	}
}
void PSpace::drawCommand()
{
	if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) == nullptr) {
		if (m_lpCommandManager->getActiveObject(CommandContext::COMMAND) == nullptr) {
			return;
		}
		else {
			//m_lpCommandManager->getActiveObject(FALSE)->draw(m_lpPaperViewport->getCursor(), s_c3fCrosshairColour);
			//m_lpCommandManager->getActiveObject(FALSE)->draw(
			//	m_lpPaperViewport->getCursor(),
			//	{ 1.f - s_c3fCrosshairColour.r, 1.f - s_c3fCrosshairColour.g, 1.f - s_c3fCrosshairColour.b }
			//);
			m_lpCommandManager->getActiveObject(CommandContext::COMMAND)->wm_paint();
		}
	}
	else {

	}
}



Colour3<float> MSpace::s_c3fUCSIconColour{ 0.f, 0.f, 0.f };//Black

MSpace::MSpace(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* uiviewport, PViewport* paperviewport, RECT* paper, RECT* shadow, RECT* margin, RECT* viewports)
	: PaperSpace(commandmanager, datablocks, viewportmanager, uiviewport, paperviewport, paper, shadow, margin, viewports)
{
	log("Log::MSpace::MSpace()");
	initProperties();
}
MSpace::~MSpace()
{
	log("Log::MSpace::~MSpace()");
}

void MSpace::setUCSIconColour(Colour3<float> colour) { s_c3fUCSIconColour = colour; }

Space::Context MSpace::getContext() const { return Context::ModelSpace; }

void MSpace::move(const double x, const double y, const double z) { m_lpActiveViewport->move(x, y, z); }
void MSpace::rotate(double x, double y, double z)
{
	m_lpActiveViewport->rotate(x, y, z);
	setProperties();
}
void MSpace::zoom(const double z) { m_lpActiveViewport->zoom(z); }

void MSpace::paint(ViewportManager* viewportmanager) { PaperSpace::paint(viewportmanager); }
void MSpace::paint()
{
	glViewport(m_lpPaperViewport->left, m_lpPaperViewport->top, m_lpPaperViewport->width, m_lpPaperViewport->height);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_lpPaperViewport->getCamera()->getProjectionMatrixElementsf());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_lpPaperViewport->getCamera()->getModelViewMatrixElementsf());

	drawShadow();
	drawPaper();
	drawMargin();

	paint(m_lpViewportManager);//Draw viewports and borders.

	glViewport(m_lpUserInterface->left, m_lpUserInterface->top, m_lpUserInterface->width, m_lpUserInterface->height);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_lpUserInterface->getCamera()->getProjectionMatrixElementsf());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_lpUserInterface->getCamera()->getModelViewMatrixElementsf());//Could use glLoadIdentity() here, but using the cameras modelview matrix allows us to scale the UI

	drawActiveBorder(//Draw active viewport border...
		m_lpActiveViewport->left - m_nBorderWidth / 2 - 1,
		m_lpActiveViewport->top - m_nBorderWidth / 2,
		m_lpActiveViewport->right + m_nBorderWidth / 2,
		m_lpActiveViewport->bottom + m_nBorderWidth / 2 + 1, m_nBorderWidth
	);

	glViewport(m_lpActiveViewport->left, m_lpActiveViewport->top, m_lpActiveViewport->width, m_lpActiveViewport->height);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_lpActiveViewport->getUICamera()->getProjectionMatrixElementsf());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_lpActiveViewport->getUICamera()->getModelViewMatrixElementsf());

	m_lpText_Properties->draw();

	drawUCS();
	drawCommand();
	drawPointer();
}
void MSpace::size(SIZE lsize)
{
	PaperSpace::size(lsize);
	size();
}
void MSpace::size() {
	int x = -m_lpActiveViewport->width / 2 + 3 + m_nBorderWidth;
	int y = m_lpActiveViewport->height / 2 - m_lpText_Properties->getSize().cy - 3 - m_nBorderWidth;
	m_lpText_Properties->setPosition({ x,y });
}

void MSpace::drawPointer()
{
	//if (DynamicInput::s_nVisible == TRUE) {
	//	if (m_lpCommandManager->getDynamicInput()->getValid() == TRUE) {
	//		if (::IsWindowVisible(m_lpCommandManager->getDynamicInput()->getHWND()) == TRUE) {
	//			return;
	//		}
	//	}
	//}

	if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) == nullptr) {
		if (m_lpCommandManager->getActiveObject(CommandContext::COMMAND) == nullptr) {
			if (m_lpActiveViewport->getCamera()->isViewOrthogonal()) {
				m_Pointer.draw(m_lpActiveViewport->getCursor(), s_c3fCrosshairColour, s_bTintForXYZ);
			}
			else {
				bool bPerspective = m_lpActiveViewport->getCamera()->getPerspective();
				float fFOV = (float)m_lpActiveViewport->getCamera()->getFOV();
				Matrix4<float>m4fModelView = m_lpActiveViewport->getCamera()->getModelViewRotationMatrixf();
				m_Pointer.draw(m_lpActiveViewport->extents, bPerspective, fFOV, m4fModelView, m_lpActiveViewport->getCursor(), s_c3fCrosshairColour, s_bTintForXYZ);
			}
		}
		else {
			Command* command = m_lpCommandManager->getActiveObject(CommandContext::COMMAND);
			if (command->getID() == IDC_SELECT) {
				m_Pointer.drawCommand(m_lpActiveViewport->getCursor(), s_c3fCrosshairColour, s_bTintForXYZ);
			}
		}
	}
}
void MSpace::drawUCS()
{
	if (m_lpActiveViewport->getUCSVisible() == TRUE) {
		SIZE slHalfViewport = m_lpActiveViewport->extents;
		float fFOV = (float)m_lpActiveViewport->getCamera()->getFOV();
		Matrix4<float>m_m4fModelView = m_lpActiveViewport->getCamera()->getModelViewMatrixf();
		m_m4fModelView.setColumn(3, Vector4<float>{0.f, 0.f, 0.f, 1.f});
		bool bPerspective = m_lpActiveViewport->getCamera()->getPerspective();
		RECT rcBoundingbox = m_UCSIcon.getBoundingbox();
		if (getActiveViewport()->getUCSOrigin() == TRUE) {
			POINT position = m_lpActiveViewport->getCamera()->Project(0.0, 0.0);
			if ((m_lpActiveViewport->getUCSOrigin() == TRUE) &&
				(((position.x > (-slHalfViewport.cx / 2 - rcBoundingbox.left)) && (position.y > (-slHalfViewport.cy / 2 - rcBoundingbox.top))) &&
					((position.x < (slHalfViewport.cx / 2 - rcBoundingbox.right)) && (position.y < (slHalfViewport.cy / 2 - rcBoundingbox.bottom))))) {
				//position.x += 10;
				//position.y += 10;
			}
			else {
				position = { (long)(10 - slHalfViewport.cx / 2), (long)(10 - slHalfViewport.cy / 2) };
			}
			m_UCSIcon.draw(slHalfViewport, fFOV, m_m4fModelView, bPerspective, position, s_c3fUCSIconColour);
		}
		else {
			if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) == nullptr) {
				POINT position = { m_nBorderWidth + m_UCSIcon.getOffset(0) - slHalfViewport.cx / 2, m_nBorderWidth + m_UCSIcon.getOffset(0) - slHalfViewport.cy / 2 };
				m_UCSIcon.draw(slHalfViewport, fFOV, m_m4fModelView, bPerspective, position, s_c3fUCSIconColour, true);
			}
			else {
				POINT position = { m_nBorderWidth + m_UCSIcon.getOffset(1) - slHalfViewport.cx / 2, m_nBorderWidth + m_UCSIcon.getOffset(1) - slHalfViewport.cy / 2 };
				m_UCSIcon.draw(slHalfViewport, fFOV, m_m4fModelView, bPerspective, position, s_c3fUCSIconColour);
			}
		}
	}
}
void MSpace::drawCommand()
{
	if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) == nullptr) {
		if (m_lpCommandManager->getActiveObject(CommandContext::COMMAND) == nullptr) {
			return;
		}
		else {
			//m_lpCommandManager->getActiveObject(FALSE)->draw(m_lpActiveViewport->getCursor(), s_c3fCrosshairColour);
			m_lpCommandManager->getActiveObject(CommandContext::COMMAND)->wm_paint();
		}
	}
	else {

	}
}
