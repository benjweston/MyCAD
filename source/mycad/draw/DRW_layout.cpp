#include "DRW_layout.h"

#include "CMD_commands.h"

#include "fault.h"
#include "log.h"

#include <GL/gl.h>

#include <chrono>

using namespace mycad;

Layout::Layout(int index, const char* name, SIZE lsize, CommandManager* commandmanager, DataBlocks* datablocks)
	: m_nIndex(index),
	m_Size(lsize),
	m_lpCommandManager(commandmanager),
	m_lpDataBlocks(datablocks),
	m_lpActiveSpace(nullptr)
{
	log("Log::Layout::Layout(CommandManager *commandmanager, int width, int height)");

	::strcpy_s(m_szName, name);
}
Layout::~Layout()
{
	log("Log::Layout::~Layout()");
}

void Layout::setIndex(int index) { m_nIndex = index; }
int Layout::getIndex() { return m_nIndex; }

void Layout::setName(const char* lpszName) { ::strcpy_s(m_szName, lpszName); }
char* Layout::getName() { return m_szName; }

ViewportManager* Layout::Viewports() { return &m_ViewportManager; }

Space* Layout::getActiveSpace() const { return m_lpActiveSpace; }

BOOL Layout::setActiveViewport(POINT point)
{
	BOOL nSuccess = FALSE;
	nSuccess = m_lpActiveSpace->setActiveViewport(point);
	m_lpActiveSpace->size();
	return nSuccess;
}
Viewport* Layout::getActiveViewport() const { return m_lpActiveSpace->getActiveViewport(); }

void Layout::activate()
{
	//log("Log::Layout::activate()");
	for (std::list<Entity*>::const_iterator it = m_lpDataBlocks->m_mBlock.ent.begin(); it != m_lpDataBlocks->m_mBlock.ent.end(); ++it) {
		(*it)->redraw();
	}
}



Colour3<float> PaperLayout::s_c3fBackgroundColour{ 1.f, 1.f, 1.f };//White
Colour3<float> PaperLayout::s_c3fPaperColour{ 1.f, 1.f, 1.f };//White

PaperLayout::PaperLayout(int index, const char* name, SIZE lsize, CommandManager* commandmanager, DataBlocks* datablocks)
	: Layout(index, name, lsize, commandmanager, datablocks),
	m_nVBOid_vertices(0),
	m_nVBOid_indices(0),
	m_slPapersize({ 297, 210 }),
	rcPrintableAreaOffset({ 24,8,-24,-8 }),
	rcShadowOffset({ 8,-4,4,-8 }),
	rcViewportsOffset({ 30,20,-30,-20 }),
	m_rcPaper({ 0,0,0,0 }),
	m_rcShadow({ 0,0,0,0 }),
	m_rcMargin({ 0,0,0,0 }),
	m_rcViewports({ 0,0,0,0 })
{
	log("Log::PaperLayout::PaperLayout(int index %s)", name);

	setPaperSize();

	m_lpModelSpace = new MSpace(m_lpCommandManager, m_lpDataBlocks, &m_ViewportManager, &m_UserInterface, &m_PaperViewport, &m_rcPaper, &m_rcShadow, &m_rcMargin, &m_rcViewports);
	m_lpPaperSpace = new PSpace(m_lpCommandManager, m_lpDataBlocks, &m_ViewportManager, &m_UserInterface, &m_PaperViewport, &m_rcPaper, &m_rcShadow, &m_rcMargin, &m_rcViewports);

	m_lpActiveSpace = m_lpPaperSpace;
}
PaperLayout::~PaperLayout()
{
	log("Log::PaperLayout::~PaperLayout()");

	delete m_lpModelSpace;
	m_lpModelSpace = NULL;

	delete m_lpPaperSpace;
	m_lpPaperSpace = NULL;
}

Colour3<float> PaperLayout::getCrosshairColour() { return PaperSpace::getCrosshairColour(); }

void PaperLayout::setBackgroundColour(Colour3<float> colour) { s_c3fBackgroundColour = colour; }
void PaperLayout::setPaperColour(Colour3<float> colour) { s_c3fPaperColour = colour; }

Layout::Context PaperLayout::getContext() const { return Context::PaperLayout; }

Space::Context PaperLayout::setActiveSpace(POINT point)
{
	if (m_lpActiveSpace->getContext() == Space::Context::PaperSpace) {
		if (m_lpPaperSpace->PtInRect(point) == TRUE) {//If the mouse is in the model space rectangle...
			setActiveSpace(Space::Context::ModelSpace);//...make the model space active;...
			m_lpActiveSpace->setActiveViewport(point);
		}
	}
	else if (m_lpActiveSpace->getContext() == Space::Context::ModelSpace) {
		if (m_lpPaperSpace->PtInRect(point) == FALSE) {//...If the mouse is NOT in the model space rectangle...
			setActiveSpace(Space::Context::PaperSpace);//...make the paper space active.
		}
	}
	m_lpActiveSpace->getActiveViewport()->mousemove(point);//Make sure the cursor is in the correct position.
	return m_lpActiveSpace->getContext();
}
Space::Context PaperLayout::setActiveSpace(Space::Context context) {
	if (m_lpActiveSpace->getContext() != context) {
		if (context == Space::Context::PaperSpace) {
			m_lpActiveSpace = m_lpPaperSpace;
		}
		else if (context == Space::Context::ModelSpace) {
			m_lpActiveSpace = m_lpModelSpace;
		}
		m_lpActiveSpace->size(m_Size);
	}
	return m_lpActiveSpace->getContext();
}

void PaperLayout::setPaperSize() {
	m_rcPaper = { -m_slPapersize.cx / 2,-m_slPapersize.cy / 2,m_slPapersize.cx / 2,m_slPapersize.cy / 2 };

	//Calculate printable area...
	m_rcMargin.left = m_rcPaper.left + rcPrintableAreaOffset.left;
	m_rcMargin.top = m_rcPaper.top + rcPrintableAreaOffset.top;
	m_rcMargin.right = m_rcPaper.right + rcPrintableAreaOffset.right;
	m_rcMargin.bottom = m_rcPaper.bottom + rcPrintableAreaOffset.bottom;

	//Calculate shadow...
	m_rcShadow.left = m_rcPaper.left + rcShadowOffset.left;
	m_rcShadow.top = m_rcPaper.top + rcShadowOffset.top;
	m_rcShadow.right = m_rcPaper.right + rcShadowOffset.right;
	m_rcShadow.bottom = m_rcPaper.bottom + rcShadowOffset.bottom;

	//Calculate viewports area...
	m_rcViewports.left = m_rcPaper.left + rcViewportsOffset.left;
	m_rcViewports.top = m_rcPaper.top + rcViewportsOffset.top;
	m_rcViewports.right = m_rcPaper.right + rcViewportsOffset.right;
	m_rcViewports.bottom = m_rcPaper.bottom + rcViewportsOffset.bottom;

	//Calculate initial magnification value...
	const int dSpacer = 30;
	double dy = (double)m_Size.cy - dSpacer;
	double py = (double)m_slPapersize.cy;
	double magnification = py / dy;

	m_PaperViewport.getCamera()->setMagnification(magnification);
}
void PaperLayout::setPaperSize(SIZE papersize) {
	m_slPapersize = papersize;
	setPaperSize();
}
SIZE PaperLayout::getPaperSize() const { return m_slPapersize; }

void PaperLayout::move(const double x, const double y, const double z) { m_lpActiveSpace->move(x, y, z); }
void PaperLayout::rotate(double x, double y, double z) { m_lpActiveSpace->rotate(x, y, z); }
void PaperLayout::zoom(const double z) { m_lpActiveSpace->zoom(z); }

void PaperLayout::mousemove(POINT position) { m_lpActiveSpace->mousemove(position); }
void PaperLayout::paint()
{
	glClearColor(s_c3fBackgroundColour.r, s_c3fBackgroundColour.g, s_c3fBackgroundColour.b, 1.f);//Paint background...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glDisable(GL_LIGHTING);

	m_lpActiveSpace->paint();//Paint active space...

	glEnable(GL_LIGHTING);
}
void PaperLayout::size(SIZE lsize)
{
	m_Size = lsize;

	m_UserInterface.size(m_Size.cx, m_Size.cy);//UI viewports have the camera origin at left,bottom so that the Windows and OpenGL coordinate systems are the same.
	m_PaperViewport.size(m_Size.cx, m_Size.cy);
	m_lpActiveSpace->size(m_Size);
}



static std::chrono::steady_clock::time_point start{ std::chrono::steady_clock::now() };

Colour3<float> ModelSpace::s_c3fBackgroundColour{ 0.f, 0.f, 0.f };//Black
Colour3<float> ModelSpace::s_c3fBorderColour{ 0.f, 0.5f, 1.f };//Blue
Colour3<float> ModelSpace::s_c3fCrosshairColour[2]{ { 1.f, 1.f, 1.f },{ 1.f, 1.f, 1.f } };//White
bool ModelSpace::s_bTintForXYZ[2]{ true,true };//Index [0] is for parallel projection in model space, [1] is for perspective projection in model space.
Colour3<float> ModelSpace::s_c3fUCSIconColour{ 1.f, 1.f, 1.f };//White

ModelSpace::ModelSpace(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* ui)
	: Space(commandmanager, datablocks, viewportmanager, ui),
	m_nFPSCounter(0),
	m_nFPS(0)
{
	log("Log::ModelSpace::ModelSpace()");
	m_nBorderWidth = 5;
	initProperties();
	m_lpText_Properties->setTextColour({ 1.f, 1.f, 1.f, 1.f });
	initFPS();
	initFrameLength();
}

void ModelSpace::setBackgroundColour(Colour3<float> colour) { s_c3fBackgroundColour = colour; }
void ModelSpace::setBorderColour(Colour3<float> colour) { s_c3fBorderColour = colour; }

void ModelSpace::setCrosshairColour(Colour3<float> colour, bool tintforxyz, int index)
{
	s_c3fCrosshairColour[index] = colour;
	s_bTintForXYZ[index] = tintforxyz;
}
Colour3<float> ModelSpace::getCrosshairColour() { return s_c3fCrosshairColour[0]; }

void ModelSpace::setUCSIconColour(Colour3<float> colour) { s_c3fUCSIconColour = colour; }

Space::Context ModelSpace::getContext() const { return Context::ModelSpace; }

void ModelSpace::move(const double x, const double y, const double z) { m_lpActiveViewport->move(x, y, z); }
void ModelSpace::rotate(double x, double y, double z)
{
	m_lpActiveViewport->rotate(x, y, z);
	setProperties();
}
void ModelSpace::zoom(const double z) { m_lpActiveViewport->zoom(z); }

void ModelSpace::paint(ViewportManager* viewportmanager)//Recursive function that iterates through the viewports tree painting viewports, returns if the viewports viewportmanager is null.
{
	for (MViewport* lpViewport : viewportmanager->getViewports()) {//Draw viewport...
		if (lpViewport->getViewportManager() == nullptr) {
			//Paint viewport background colour...
			glViewport(lpViewport->left, lpViewport->top, lpViewport->width, lpViewport->height);
			glScissor(lpViewport->left, lpViewport->top, lpViewport->width, lpViewport->height);
			glClearColor(s_c3fBackgroundColour.r, s_c3fBackgroundColour.g, s_c3fBackgroundColour.b, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			//Draw grid...
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(lpViewport->getCamera()->getProjectionMatrixElementsf());
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(lpViewport->getCamera()->getModelViewMatrixElementsf());

			//m_Grid.draw({ lpViewport->width,lpViewport->height }, 1.0, true, true, true, Grid::Style::Axislines_And_Gridlines);
			if (lpViewport->getGridVisible()) {
				bool xy = lpViewport->getGridXYVisible();
				bool yz = lpViewport->getGridYZVisible();
				bool zx = lpViewport->getGridZXVisible();
				m_Grid.draw(lpViewport->extents, lpViewport->getCamera()->getMagnification(), xy, yz, zx, GLGrid::Style::Axislines_And_Gridlines);
			}

			//Draw entities...
			drawEntities();
		}
		else {//...or step into viewportmanagers viewport collection
			paint(lpViewport->getViewportManager());
		}
	}
}
void ModelSpace::paint()
{
	glEnable(GL_SCISSOR_TEST);

	paint(m_lpViewportManager);

	glViewport(m_lpUserInterface->left, m_lpUserInterface->top, m_lpUserInterface->width, m_lpUserInterface->height);
	glScissor(m_lpActiveViewport->left, m_lpActiveViewport->top, m_lpActiveViewport->right, m_lpActiveViewport->bottom);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_lpUserInterface->getCamera()->getProjectionMatrixElementsf());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_lpUserInterface->getCamera()->getModelViewMatrixElementsf());//Could use glLoadIdentity() here, but using the cameras modelview matrix allows us to scale the UI

	if (m_lpViewportManager->getViewportConfiguration() != ViewportManager::Configuration::One) {////Draw active viewport border (In Model Layout don't draw border for single viewport).
		drawActiveBorder(m_lpActiveViewport, m_nBorderWidth, { s_c3fBorderColour.r,s_c3fBorderColour.g,s_c3fBorderColour.b,0.7f });
	}

	glDisable(GL_SCISSOR_TEST);

	glViewport(m_lpActiveViewport->left, m_lpActiveViewport->top, m_lpActiveViewport->width, m_lpActiveViewport->height);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_lpActiveViewport->getUICamera()->getProjectionMatrixElementsf());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_lpActiveViewport->getUICamera()->getModelViewMatrixElementsf());

	m_lpText_Properties->draw();
	m_lpText_FPS->draw();

	setFrameLength();
	m_lpText_FrameLength->draw();

	drawUCS();
	drawCommand();
	drawPointer();

	m_nFPSCounter++;
}
void ModelSpace::size(ViewportManager* viewportmanager, int x, int y, int width, int height)
{
	switch (viewportmanager->getViewportConfiguration()) {
		case ViewportManager::Configuration::One: {
			viewportmanager->getViewports()[0]->size(x, y, width, height);
			break;
		}
		case ViewportManager::Configuration::TwoVertical: {
			int cx = width - Splitter::getThickness();
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;

			viewportmanager->getSplitters()[0]->size(x + dx, y, Splitter::getThickness(), width);

			viewportmanager->getViewports()[0]->size(x, y, dx, height);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, height);
			break;
		}
		case ViewportManager::Configuration::TwoHorizontal: {
			int cy = height - Splitter::getThickness();
			float fy = cy * viewportmanager->getSplitters()[0]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x, y + dy, width, Splitter::getThickness());

			viewportmanager->getViewports()[0]->size(x, y, width, dy);
			viewportmanager->getViewports()[1]->size(x, viewportmanager->getSplitters()[0]->bottom, width, cy - dy);
			break;
		}
		case ViewportManager::Configuration::ThreeRight: {
			int cx = width - Splitter::getThickness();
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height - Splitter::getThickness();
			float fy = cy * viewportmanager->getSplitters()[1]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x + dx, y, Splitter::getThickness(), height);
			viewportmanager->getSplitters()[1]->size(x, y + dy, dx, Splitter::getThickness());

			viewportmanager->getViewports()[0]->size(x, y, dx, dy);
			viewportmanager->getViewports()[1]->size(x, viewportmanager->getSplitters()[1]->bottom, dx, cy - dy);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, height);
			break;
		}
		case ViewportManager::Configuration::ThreeLeft: {
			int cx = width - Splitter::getThickness();
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height - Splitter::getThickness();
			float fy = cy * viewportmanager->getSplitters()[1]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x + dx, y, Splitter::getThickness(), height);
			viewportmanager->getSplitters()[1]->size(x + dx + Splitter::getThickness(), y + dy, cx - dx, Splitter::getThickness());

			viewportmanager->getViewports()[0]->size(x, y, dx, height);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, dy);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[0]->right, viewportmanager->getSplitters()[1]->bottom, cx - dx, cy - dy);
			break;
		}
		case ViewportManager::Configuration::ThreeAbove: {
			int cx = width - Splitter::getThickness();
			float fx = cx * viewportmanager->getSplitters()[1]->getRatio();
			int dx = (int)fx;
			int cy = height - Splitter::getThickness();
			float fy = cy * viewportmanager->getSplitters()[0]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x, y + dy, width, Splitter::getThickness());
			viewportmanager->getSplitters()[1]->size(x + dx, y, Splitter::getThickness(), dy);

			viewportmanager->getViewports()[0]->size(x, y, dx, dy);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[1]->right, y, cx - dx, dy);
			viewportmanager->getViewports()[2]->size(x, viewportmanager->getSplitters()[0]->bottom, width, cy - dy);
			break;
		}
		case ViewportManager::Configuration::ThreeBelow: {
			int cx = width - Splitter::getThickness();
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height - Splitter::getThickness();
			float fy = cy * viewportmanager->getSplitters()[1]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x, y + dy, width, Splitter::getThickness());
			viewportmanager->getSplitters()[1]->size(x + dx, y + dy + Splitter::getThickness(), Splitter::getThickness(), height - dy - Splitter::getThickness());

			viewportmanager->getViewports()[0]->size(x, y, width, cy - dy);
			viewportmanager->getViewports()[1]->size(x, viewportmanager->getSplitters()[0]->bottom, dx, cy - dy);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[1]->right, viewportmanager->getSplitters()[0]->bottom, cx - dx, cy - dy);
			break;
		}
		case ViewportManager::Configuration::ThreeVertical: {
			int cx = width - (2 * Splitter::getThickness());
			float fx[2] = { (float)cx * viewportmanager->getSplitters()[0]->getRatio(),(float)cx * viewportmanager->getSplitters()[1]->getRatio() };
			int dx[2] = { (int)fx[0],(int)fx[1] };

			viewportmanager->getSplitters()[0]->size(x + dx[0], y, Splitter::getThickness(), height);
			viewportmanager->getSplitters()[1]->size(x + dx[1] + Splitter::getThickness(), y, Splitter::getThickness(), height);

			viewportmanager->getViewports()[0]->size(x, y, dx[0], height);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, dx[1] - dx[0], height);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[1]->right, y, cx - dx[1], height);
			break;
		}
		case ViewportManager::Configuration::ThreeHorizontal: {
			int cy = height - (2 * Splitter::getThickness());
			float fy[2] = { (float)cy * viewportmanager->getSplitters()[0]->getRatio(),(float)cy * viewportmanager->getSplitters()[1]->getRatio() };
			int dy[2] = { (int)fy[0],(int)fy[1] };

			viewportmanager->getSplitters()[0]->size(x, y + dy[0], width, Splitter::getThickness());
			viewportmanager->getSplitters()[1]->size(x, y + dy[1] + Splitter::getThickness(), width, Splitter::getThickness());

			viewportmanager->getViewports()[0]->size(x, y, width, dy[0]);
			viewportmanager->getViewports()[1]->size(x, viewportmanager->getSplitters()[0]->bottom, width, dy[1] - dy[0]);
			viewportmanager->getViewports()[2]->size(x, viewportmanager->getSplitters()[1]->bottom, width, cy - dy[1]);
			break;
		}
		case ViewportManager::Configuration::FourEqual: {
			int cx = width - Splitter::getThickness();
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height - Splitter::getThickness();
			float fy = cy * viewportmanager->getSplitters()[1]->getRatio();
			int dy = (int)fy;

			viewportmanager->getSplitters()[0]->size(x + dx, y, Splitter::getThickness(), height);
			viewportmanager->getSplitters()[1]->size(x, y + dy, width, Splitter::getThickness());

			viewportmanager->getViewports()[0]->size(x, y, dx, dy);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, dy);
			viewportmanager->getViewports()[2]->size(x, viewportmanager->getSplitters()[1]->bottom, dx, cy - dy);
			viewportmanager->getViewports()[3]->size(viewportmanager->getSplitters()[0]->right, viewportmanager->getSplitters()[1]->bottom, cx - dx, cy - dy);
			break;
		}
		case ViewportManager::Configuration::FourRight: {
			int cx = width - Splitter::getThickness();
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height - (2 * Splitter::getThickness());
			float fy[2] = { cy * viewportmanager->getSplitters()[1]->getRatio(),cy * viewportmanager->getSplitters()[2]->getRatio() };
			int dy[2] = { (int)fy[0],(int)fy[1] };

			viewportmanager->getSplitters()[0]->size(x + dx, y, Splitter::getThickness(), height);
			viewportmanager->getSplitters()[1]->size(x + dx + Splitter::getThickness(), y + dy[0], width - dx - Splitter::getThickness(), Splitter::getThickness());
			viewportmanager->getSplitters()[2]->size(x + dx + Splitter::getThickness(), y + dy[1] + Splitter::getThickness(), width - dx - Splitter::getThickness(), Splitter::getThickness());

			viewportmanager->getViewports()[0]->size(x, y, dx, height);
			viewportmanager->getViewports()[1]->size(viewportmanager->getSplitters()[0]->right, y, cx - dx, dy[0]);
			viewportmanager->getViewports()[2]->size(viewportmanager->getSplitters()[0]->right, viewportmanager->getSplitters()[1]->bottom, cx - dx, dy[1] - dy[0]);
			viewportmanager->getViewports()[3]->size(viewportmanager->getSplitters()[0]->right, viewportmanager->getSplitters()[2]->bottom, cx - dx, cy - dy[1]);
			break;
		}
		case ViewportManager::Configuration::FourLeft: {
			int cx = width - Splitter::getThickness();
			float fx = cx * viewportmanager->getSplitters()[0]->getRatio();
			int dx = (int)fx;
			int cy = height - (2 * Splitter::getThickness());
			float fy[2] = { cy * viewportmanager->getSplitters()[1]->getRatio(),cy * viewportmanager->getSplitters()[2]->getRatio() };
			int dy[2] = { (int)fy[0],(int)fy[1] };

			viewportmanager->getSplitters()[0]->size(x + dx, y, Splitter::getThickness(), height);
			viewportmanager->getSplitters()[1]->size(x, y + dy[0], dx, Splitter::getThickness());
			viewportmanager->getSplitters()[2]->size(x, y + dy[1] + Splitter::getThickness(), dx, Splitter::getThickness());

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

	//This is the recursive part - we call this function for each viewport in the viewportmanager,
	//sending a pointer to the viewports viewportmanager, left, top, width and height as the arguments.
	//The halting condition is the value of the viewportmanager pointer being null.
	//Maybe suitable for multithreading?
	for (MViewport* lpViewport : viewportmanager->getViewports()) {
		if (lpViewport->getViewportManager() != nullptr) {
			size(lpViewport->getViewportManager(), lpViewport->left, lpViewport->top, lpViewport->width, lpViewport->height);
		}
	}
}
void ModelSpace::size(int x, int y, int width, int height)
{
	m_rcDimensions.SetGLRect(x, y, width, height);//Set GLRectangle properties...
	size(m_lpViewportManager, x, y, width, height);//The entry point for the recursive size(...) function that loops through the layouts tree and sets the position/size properties...
	//size();//Set the position/size of the FPS label.
}
void ModelSpace::size(SIZE lsize)
{
	size(m_rcDimensions.left, m_rcDimensions.top, lsize.cx, lsize.cy);
	size();
}
void ModelSpace::size()
{
	if (m_lpViewportManager->getViewportConfiguration() != ViewportManager::Configuration::One) {
		m_lpText_Properties->setPosition({ -m_lpActiveViewport->width / 2 + 3 + m_nBorderWidth, m_lpActiveViewport->height / 2 - m_lpText_Properties->getSize().cy - 3 - m_nBorderWidth });
		m_lpText_FPS->setPosition({ m_lpActiveViewport->width / 2 - m_lpText_FPS->getSize().cx - 3 - m_nBorderWidth, m_lpActiveViewport->height / 2 - m_lpText_FPS->getSize().cy - 3 - m_nBorderWidth });
		m_lpText_FrameLength->setPosition({ m_lpActiveViewport->width / 2 - m_lpText_FrameLength->getSize().cx - 3 - m_nBorderWidth, m_lpActiveViewport->height / 2 - m_lpText_FPS->getSize().cy - 3 - m_lpText_FrameLength->getSize().cy - 3 - m_nBorderWidth });
	}
	else {
		m_lpText_Properties->setPosition({ -m_lpActiveViewport->width / 2 + 3, m_lpActiveViewport->height / 2 - m_lpText_Properties->getSize().cy - 3 });
		m_lpText_FPS->setPosition({ m_lpActiveViewport->width / 2 - m_lpText_FPS->getSize().cx - 3, m_lpActiveViewport->height / 2 - m_lpText_FPS->getSize().cy - 3 });
		m_lpText_FrameLength->setPosition({ m_lpActiveViewport->width / 2 - m_lpText_FrameLength->getSize().cx - 3, m_lpActiveViewport->height / 2 - m_lpText_FPS->getSize().cy - 3 - m_lpText_FrameLength->getSize().cy - 3 });
	}
}
void ModelSpace::timer()
{
	m_nFPS = m_nFPSCounter;
	m_nFPSCounter = 0;

	char chFps[8];
	::sprintf_s(chFps, "%d", m_nFPS/* - 1*/);//Frame counter minus one - timer calls paint() once per second so we need to take this off the counter to get the true FPS. 

	char buf[16] = "FPS[";
	::strcat_s(buf, 16, chFps);
	::strcat_s(buf, 16, "]");

	m_lpText_FPS->setText(buf);

	char buf2[24] = "Frame length = ";
	::strcat_s(buf2, 24, m_chFrameLength);
	::strcat_s(buf2, 24, "ms");
	m_lpText_FrameLength->setText(buf2);

	size();
}

void ModelSpace::drawEntity(Entity* ent)
{
	if (ent->getEntity() == nullptr) {
		return;
	}
	Colour3<float> c3fColour = Entity::IndexToVector3fColour(ent->getEntity()->color);
	//if ((ent->getEntity()->color == 0) || (ent->getEntity()->color == 7) || (ent->getEntity()->color == 256)) {//If colour is black...
	//	c3fColour = Entity::IndexToVector3fColour(255);//Then change colour to white.
	//}
	if ((c3fColour.r == 0.0) && (c3fColour.g == 0.0) && (c3fColour.b == 0.0)) {//If colour is black...
		ent->paint({ 1.f, 1.f, 1.f });//...set colour to white.
	}
	else {
		ent->paint(c3fColour);
	}
}
void ModelSpace::drawEntities()
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

void ModelSpace::initFPS()
{
	const char* pchFont = "C:/Windows/Fonts/Tahoma.ttf";
	const int nTextHeight = 12;
	const Colour4<float> c3fColour = { 1.f,1.f,0.f,1.f };

	m_lpText_FPS = new GLText(pchFont, nTextHeight);
	m_lpText_FPS->setTextColour(c3fColour);

	char chFps[8];
	::sprintf_s(chFps, "%d", m_nFPS);//Frame counter minus one - timer calls paint() once per second so we need to take this off the counter to get the true FPS. 

	char buf[16] = "FPS[";
	::strcat_s(buf, 16, chFps);
	::strcat_s(buf, 16, "]");

	m_lpText_FPS->setText(buf);
	m_lpText_FPS->setPosition({ m_lpActiveViewport->right - m_lpText_FPS->getSize().cx, m_lpActiveViewport->bottom - m_lpText_FPS->getSize().cy });
}
void ModelSpace::initFrameLength()
{
	const char* pchFont = "C:/Windows/Fonts/Tahoma.ttf";
	const int nTextHeight = 12;
	const Colour4<float> c3fColour = { 1.f,1.f,0.f,1.f };

	m_lpText_FrameLength = new GLText(pchFont, nTextHeight);
	m_lpText_FrameLength->setTextColour(c3fColour);

	char buf[17] = "This frame = 0ms";

	m_lpText_FrameLength->setText(buf);
	m_lpText_FrameLength->setPosition({ m_lpActiveViewport->right - m_lpText_FrameLength->getSize().cx, m_lpActiveViewport->bottom - m_lpText_FPS->getSize().cy - 2 - m_lpText_FrameLength->getSize().cy });
}
void ModelSpace::setFrameLength()
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	::sprintf_s(m_chFrameLength, "%lld", std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count());//Frame counter minus one - timer calls paint() once per second so we need to take this off the counter to get the true FPS. 

	//char buf[24] = "Frame length = ";
	//::strcat_s(buf, 24, m_chFrameLength);
	//::strcat_s(buf, 24, "ms");

	//m_lpText_FrameLength->setText(buf);

	start = now;
}

void ModelSpace::drawPointer()
{
	if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) == nullptr) {
		if (m_lpCommandManager->getActiveObject(CommandContext::COMMAND) == nullptr) {
			if (m_lpActiveViewport->getCamera()->isViewOrthogonal()) {
				m_Pointer.draw(m_lpActiveViewport->getCursor(), s_c3fCrosshairColour[0], s_bTintForXYZ[0]);
			}
			else {
				bool bPerspective = m_lpActiveViewport->getCamera()->getPerspective();
				float fFOV = (float)m_lpActiveViewport->getCamera()->getFOV();
				Matrix4<float>m4fModelView = m_lpActiveViewport->getCamera()->getModelViewRotationMatrixf();
				m_Pointer.draw(m_lpActiveViewport->extents, bPerspective, fFOV, m4fModelView, m_lpActiveViewport->getCursor(), s_c3fCrosshairColour[1], s_bTintForXYZ[1]);
			}
		}
		else {
			Command* command = m_lpCommandManager->getActiveObject(CommandContext::COMMAND);
			if (command->getID() == IDC_SELECT) {
				m_Pointer.drawCommand(m_lpActiveViewport->getCursor(), s_c3fCrosshairColour[0], s_bTintForXYZ[0]);
			}
		}
	}
}
void ModelSpace::drawUCS()
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
				(((position.x > (-slHalfViewport.cx - rcBoundingbox.left)) && (position.y > (-slHalfViewport.cy - rcBoundingbox.top))) &&
					((position.x < (slHalfViewport.cx - rcBoundingbox.right)) && (position.y < (slHalfViewport.cy - rcBoundingbox.bottom))))) {
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
void ModelSpace::drawCommand()
{
	if (m_lpCommandManager->getActiveObject(CommandContext::VIEW) == nullptr) {
		if (m_lpCommandManager->getActiveObject(CommandContext::COMMAND) == nullptr) {
			return;
		}
		else {
			//m_lpCommandManager->getActiveObject(FALSE)->draw(m_lpActiveViewport->getCursor(), s_c3fCrosshairColour[0]);
			m_lpCommandManager->getActiveObject(CommandContext::COMMAND)->wm_paint();
		}
	}
	else {

	}
}



Colour3<float> ModelLayout::s_c3fBackgroundColour{ 1.f, 1.f, 1.f };//White

ModelLayout::ModelLayout(int index, const char* name, SIZE lsize, CommandManager* commandmanager, DataBlocks* datablocks)
	: Layout(index, name, lsize, commandmanager, datablocks)//There is only one model layout, index is always 0
{
	log("Log::ModelLayout::ModelLayout()");
	m_lpModelSpace = new ModelSpace(m_lpCommandManager, m_lpDataBlocks, &m_ViewportManager, &m_UserInterface);
	m_lpActiveSpace = m_lpModelSpace;
}
ModelLayout::~ModelLayout()
{
	log("Log::ModelLayout::~ModelLayout()");

	delete m_lpModelSpace;
	m_lpModelSpace = NULL;
}

void ModelLayout::setBackgroundColour(Colour3<float> colour) { s_c3fBackgroundColour = colour; }

Colour3<float> ModelLayout::getCrosshairColour() { return ModelSpace::getCrosshairColour(); }

Layout::Context ModelLayout::getContext() const { return Context::ModelLayout; }

Space::Context ModelLayout::setActiveSpace(POINT point) { UNREFERENCED_PARAMETER(point); return Space::Context::ModelSpace; }
Space::Context ModelLayout::setActiveSpace(Space::Context context) { UNREFERENCED_PARAMETER(context); return Space::Context::ModelSpace; }

void ModelLayout::move(const double x, const double y, const double z) { m_lpActiveSpace->move(x, y, z); }
void ModelLayout::rotate(double x, double y, double z) { m_lpActiveSpace->rotate(x, y, z); }
void ModelLayout::zoom(const double z) { m_lpActiveSpace->zoom(z); }

void ModelLayout::mousemove(POINT position) { m_lpActiveSpace->mousemove(position); }
void ModelLayout::paint()
{
	glClearColor(s_c3fBackgroundColour.r, s_c3fBackgroundColour.g, s_c3fBackgroundColour.b, 1.f);//Paint splitter background colour...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glDisable(GL_LIGHTING);

	m_lpActiveSpace->paint();

	glEnable(GL_LIGHTING);
}
void ModelLayout::size(SIZE lsize)
{
	m_Size = lsize;

	m_UserInterface.size(m_Size.cx, m_Size.cy);//UI viewports have the camera origin at left,bottom so that the Windows and OpenGL coordinate systems are the same.
	m_lpActiveSpace->size(m_Size);
}
void ModelLayout::timer() { m_lpActiveSpace->timer(); }
