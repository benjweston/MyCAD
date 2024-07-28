#include "DRW_view.h"

#include "LIB_constants.h"
#include "LIB_conversions.h"

#include "fault.h"
#include "log.h"

using namespace mycad;

View::View() : View("Current") {}
View::View(const View& view)
	: m_bClipping(view.m_bClipping),
	m_bPerspective(view.m_bPerspective),
	m_dFar(view.m_dFar),
	m_dFieldOfView(view.m_dFieldOfView),
	m_dFieldOfViewMinimum(view.m_dFieldOfViewMinimum),
	m_dMagnification(view.m_dMagnification),
	m_dMagnificationMinimum(view.m_dMagnificationMinimum),
	m_dNear(view.m_dNear),
	m_enPresetView(view.m_enPresetView),
	m_enVisualStyle(view.m_enVisualStyle),
	m_szSize(view.m_szSize),
	m_v3dCamera(view.m_v3dCamera),
	m_v3dDirection(view.m_v3dDirection),
	m_v3dTarget(view.m_v3dTarget)
{
	strcpy_s(m_szName, view.m_szName);
}
View::View(const char* name, PresetView presetview)
	: m_szSize({ 0,0 }),
	m_enPresetView(presetview),
	m_enVisualStyle(VisualStyle::Wireframe2D),
	m_bPerspective(false),
	m_v3dTarget(Vector3<double>(0.0, 0.0, 0.0)),
	m_v3dCamera(Vector3<double>(0.0, 0.0, -500.0)),
	m_v3dDirection(Vector3<double>(0.0, 0.0, 0.0)),
	m_dNear(10.0),
	m_dFar(2500.0),
	m_dMagnification(1.0),
	m_dMagnificationMinimum(100.0),
	m_dFieldOfView(30.0),
	m_dFieldOfViewMinimum(0.01),
	m_bClipping(false)
{
	::strcpy_s(m_szName, name);
	switch (presetview) {
		case View::PresetView::Top: {
			m_v3dDirection = { 0.0, 0.0, 0.0 };
			break;
		}
		case View::PresetView::Bottom: {
			m_v3dDirection = { PI<double>(), 0.0, PI<double>() };
			break;
		}
		case View::PresetView::Left: {
			m_v3dDirection = { -PI<double>() / 2.0, 0.0, PI<double>() / 2.0 };
			break;
		}
		case View::PresetView::Right: {
			m_v3dDirection = { -PI<double>() / 2.0, 0.0, -PI<double>() / 2.0 };
			break;
		}
		case View::PresetView::Front: {
			m_v3dDirection = { -PI<double>() / 2.0, 0.0, 0.0 };
			break;
		}
		case View::PresetView::Back: {
			m_v3dDirection = { -PI<double>() / 2.0, 0.0, PI<double>() };
			break;
		}
		case View::PresetView::SWIsometric: {
			m_v3dDirection = { -PI<double>() / 4.0, 0.0, PI<double>() / 4.0 };
			break;
		}
		case View::PresetView::SEIsometric: {
			m_v3dDirection = { -PI<double>() / 4.0, 0.0, PI<double>() / 4.0 - PI<double>() / 2.0 };
			break;
		}
		case View::PresetView::NEIsometric: {
			m_v3dDirection = { -PI<double>() / 4.0, 0.0, PI<double>() / 4.0 - PI<double>() };
			break;
		}
		case View::PresetView::NWIsometric: {
			m_v3dDirection = { -PI<double>() / 4.0, 0.0, PI<double>() / 4.0 + PI<double>() / 2.0 };
			break;
		}
		default: {
			m_enPresetView = View::PresetView::Top;
			m_v3dDirection = { 0.0, 0.0, 0.0 };
		}
	}
}
View::~View()
{
	log("Log::View::~View()");
}

void View::setName(const char* lpszName) { ::strcpy_s(m_szName, lpszName); }
char* View::getName() { return m_szName; }

View::PresetView View::getPresetView()
{
	if ((m_v3dDirection.x == 0.0) || (abs(m_v3dDirection.x) == PI<double>() / 4.0) || (abs(m_v3dDirection.x) == PI<double>() / 2.0) || (abs(m_v3dDirection.x) == PI<double>() / 4.0 * 3.0)) {
		if ((m_v3dDirection.z == 0.0) || (abs(m_v3dDirection.z) == PI<double>() / 4.0) || (abs(m_v3dDirection.z) == PI<double>() / 2.0) || (abs(m_v3dDirection.z) == PI<double>() / 4.0 * 3.0)) {

		}
		else {
			m_enPresetView = PresetView::Current;
		}
	}
	else {
		m_enPresetView = PresetView::Current;
	}

	return m_enPresetView;
}

const char* View::viewToString(PresetView view)
{
	switch (view) {
#define X(ID, VALUE) case PresetView::ID: return #ID;
		PresetViews
#undef X
	};

	return nullptr;
}
View::PresetView View::stringToView(const char* viewName)
{
#define X(ID, VALUE) if(::strcmp(viewName, #ID) == 0) return PresetView::ID;
	PresetViews
#undef X

		return PresetView::INVALID;
}

View::VisualStyle View::getVisualStyle() const { return m_enVisualStyle; }

const char* View::visualstyleToString(VisualStyle visualstyle)
{
	switch (visualstyle) {
#define X(ID, VALUE) case VisualStyle::ID: return #ID;
		VisualStyles
#undef X
	};

	return nullptr;
}
View::VisualStyle View::stringToVisualstyle(const char* visualstyleName)
{
#define X(ID, VALUE) if(::strcmp(visualstyleName, #ID) == 0) return VisualStyle::ID;
	VisualStyles
#undef X

		return VisualStyle::INVALID;
}

bool View::getPerspective() const { return m_bPerspective; }

double View::getFieldOfView() const { return m_dFieldOfView; }

double View::getNear() const { return m_dNear; }

double View::getFar() const { return m_dFar; }

Vector3<double> View::getTarget() const { return m_v3dTarget; }

Vector3<double> View::getCamera() const { return m_v3dCamera; }

bool View::getClipping() const { return m_bClipping; }

void View::size(int width, int height)
{
	m_szSize.cx = width;
	m_szSize.cy = height;
}
