#include "DRW_viewport.h"

#include "LIB_conversions.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>

using namespace mycad;

int Splitter::s_Thickness{ 5 };

Splitter::Splitter()
	: GLRECT(),
	m_Orientation(Orientation::Vertical),
	m_Ratio(0.5f)
{
	log("Log::Splitter::Splitter()");
}
Splitter::Splitter(Orientation orientation, float ratio)
	: GLRECT(),
	m_Orientation(orientation),
	m_Ratio(ratio)
{
	log("Log::Splitter::Splitter(float ratio)");
}
Splitter::~Splitter()
{
	log("Log::Splitter::~Splitter()");
}

void Splitter::setThickness(int thickness) { s_Thickness = thickness; }
int Splitter::getThickness() { return s_Thickness; }

void Splitter::setOrientation(Orientation orientation) { m_Orientation = orientation; }
Orientation Splitter::getOrientation() { return m_Orientation; }

void Splitter::setRatio(float ratio) { m_Ratio = ratio; }
float Splitter::getRatio() { return m_Ratio; }

void Splitter::size(const int x, const int y, const int cx, const int cy) { SetGLRect(x, y, cx, cy); }
void Splitter::size(const int cx, const int cy) { size(left, top, cx, cy); }



Viewport::Viewport()
	: GLRECT(),
	m_ptCursor({ 0,0 }),
	m_nUCS_DisplayState(0),
	m_nUCS_Visible(TRUE),
	m_nUCS_Origin(FALSE),
	m_bGrid_Visible(true),
	m_bGridXY_Visible(true),
	m_bGridYZ_Visible(true),
	m_bGridZX_Visible(true)
{
	log("Log::Viewport::Viewport()");

	View* lpView = new View();
	m_lpUICamera = new Camera(lpView);
	m_lpUICamera->setNear(1.0);
	m_lpUICamera->setFar(0.0);
	m_lpUICamera->setPosition(0.0, 0.0, 0.0);
}
Viewport::~Viewport()
{
	log("Log::Viewport::~Viewport()");
}

POINT Viewport::getCursor() const { return m_ptCursor; }
POINT Viewport::getCursor(POINT position) const
{
	POINT ptCursor = position;

	ptCursor.x -= width / 2;
	ptCursor.y -= height / 2;

	ptCursor.x -= left;
	ptCursor.y -= top;

	return ptCursor;
}

BOOL Viewport::setUCSVisible(BOOL visible)
{
	m_nUCS_Visible = visible;

	m_nUCS_DisplayState = m_nUCS_Visible + 2 * m_nUCS_Origin;

	return m_nUCS_Visible;
}
BOOL Viewport::getUCSVisible() { return m_nUCS_Visible; }

BOOL Viewport::setUCSOrigin(BOOL origin)
{
	m_nUCS_Origin = origin;

	m_nUCS_DisplayState = m_nUCS_Visible + 2 * m_nUCS_Origin;

	return m_nUCS_Origin;
}
BOOL Viewport::getUCSOrigin() { return m_nUCS_Origin; }

int Viewport::setUCSDisplayState(int state)//Used by SETVAR(UCSICON, val); 0 = Off, left/bottom corner, 1 = On, left/bottom corner, 2 = Off, origin, 3 = On, origin
{
	m_nUCS_DisplayState = state;

	if ((m_nUCS_DisplayState == 0) || (m_nUCS_DisplayState == 2)) {
		m_nUCS_Visible = 0;
	}
	else if ((m_nUCS_DisplayState == 1) || (m_nUCS_DisplayState == 3)) {
		m_nUCS_Visible = 1;
	}

	if ((m_nUCS_DisplayState == 0) || (m_nUCS_DisplayState == 1)) {
		m_nUCS_Origin = 0;
	}
	else if ((m_nUCS_DisplayState == 2) || (m_nUCS_DisplayState == 3)) {
		m_nUCS_Origin = 1;
	}

	return m_nUCS_DisplayState;
}
int Viewport::getUCSDisplayState() { return m_nUCS_DisplayState; }

bool Viewport::setGridVisible(bool visible)
{
	m_bGrid_Visible = visible;
	return m_bGrid_Visible;
}
bool Viewport::getGridVisible() { return m_bGrid_Visible; }

bool Viewport::setGridXYVisible(bool visible)
{
	m_bGridXY_Visible = visible;
	return m_bGridXY_Visible;
}
bool Viewport::getGridXYVisible() { return m_bGridXY_Visible; }
bool Viewport::setGridYZVisible(bool visible)
{
	m_bGridYZ_Visible = visible;
	return m_bGridYZ_Visible;
}
bool Viewport::getGridYZVisible() { return m_bGridYZ_Visible; }
bool Viewport::setGridZXVisible(bool visible)
{
	m_bGridZX_Visible = visible;
	return m_bGridZX_Visible;
}
bool Viewport::getGridZXVisible() { return m_bGridZX_Visible; }

void Viewport::mousemove(POINT position)
{
	m_ptCursor = position;

	m_ptCursor.x -= width / 2;
	m_ptCursor.y -= height / 2;

	m_ptCursor.x -= left;
	m_ptCursor.y -= top;
}
void Viewport::size(const int x, const int y, const int cx, const int cy) { SetGLRect(x, y, cx, cy); }
void Viewport::size(const int cx, const int cy) { size(left, top, cx, cy); }



UIViewport::UIViewport()
{
	log("Log::UIViewport::UIViewport()");

	View* lpView = new View();
	m_lpCamera = new Camera(lpView);
	m_lpCamera->setNear(1.0);
	m_lpCamera->setFar(0.0);
	m_lpCamera->setPosition(0.0, 0.0, 0.0);
}
UIViewport::~UIViewport()
{
	log("Log::UIViewport::~UIViewport()");
}

Camera* UIViewport::getUICamera() const { return m_lpUICamera; }
Camera* UIViewport::getCamera() const { return m_lpCamera; }

void UIViewport::size(const int x, const int y, const int cx, const int cy)
{
	SetGLRect(x, y, cx, cy);
	m_lpCamera->size(0, 0, cx, cy);
}
void UIViewport::size(const int cx, const int cy) { size(left, top, cx, cy); }



PViewport::PViewport()
{
	log("Log::PViewport::PViewport()");

	View* lpView = new View();
	m_lpCamera = new Camera(lpView);
	m_lpCamera->setNear(1.0);
	m_lpCamera->setFar(0.0);
	m_lpCamera->setPosition(0.0, 0.0, 0.0);
}
PViewport::~PViewport()
{
	log("Log::PViewport::~PViewport()");

	delete m_lpCamera;
	m_lpCamera = NULL;
}

Camera* PViewport::getUICamera() const { return m_lpUICamera; }
Camera* PViewport::getCamera() const { return m_lpCamera; }

void PViewport::move(const double x, const double y, const double z) { m_lpCamera->move(x, y, z); }
void PViewport::zoom(const double z) { m_lpCamera->zoom(z); }

void PViewport::size(const int x, const int y, const int cx, const int cy)
{
	SetGLRect(x, y, cx, cy);

	int nWidth = cx / 2;
	int nHeight = cy / 2;

	m_lpUICamera->size(-nWidth, -nHeight, nWidth, nHeight);
	m_lpCamera->size(-nWidth, -nHeight, nWidth, nHeight);
}
void PViewport::size(const int cx, const int cy) { size(left, top, cx, cy); }



MViewport::MViewport()
	: m_lpViewportManager(0)
{
	log("Log::MViewport::MViewport()");
	View* lpView = new View();
	m_lpCamera = new Camera(lpView);
}
MViewport::~MViewport()
{
	log("Log::MViewport::~MViewport()");

	delete m_lpViewportManager;
	m_lpViewportManager = NULL;
}

Camera* MViewport::getUICamera() const { return m_lpUICamera; }
Camera* MViewport::getCamera() const { return m_lpCamera; }

void MViewport::setViewportManager(ViewportManager* viewportmanager) { m_lpViewportManager = viewportmanager; }
ViewportManager* MViewport::getViewportManager() const { return m_lpViewportManager; }

void MViewport::move(const double x, const double y, const double z) { getCamera()->move(x, y, z); }
void MViewport::rotate(double x, double y, double z) { getCamera()->rotate(x, y, z); }
void MViewport::zoom(const double z) {
	bool bPerspective = getCamera()->getPerspective();
	if (bPerspective == false) {
		getCamera()->zoom(z);
	}
	else {
		getCamera()->fov(z);
	}
}

void MViewport::size(const int x, const int y, const int cx, const int cy)
{
	SetGLRect(x, y, cx, cy);

	int nWidth = cx / 2;
	int nHeight = cy / 2;

	m_lpUICamera->size(-nWidth, -nHeight, nWidth, nHeight);
	m_lpCamera->size(-nWidth, -nHeight, nWidth, nHeight);
}
void MViewport::size(const int cx, const int cy) { size(left, top, cx, cy); }
