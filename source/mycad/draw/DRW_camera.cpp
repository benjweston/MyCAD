#include "DRW_camera.h"

#include "LIB_constants.h"
#include "LIB_conversions.h"

#include "fault.h"
#include "log.h"

#include <GL/glu.h>

using namespace mycad;

const double c_scaleFOV = 0.1;
const double c_scalePan = 1.0;
const double c_scaleRotate = 0.25;
const double c_scaleZoom = 0.005;

unsigned int Camera::s_ndGenerator{ 0 };

Camera::Camera(View* view, unsigned int id)
	: m_lpView(view),
	m_nId(id),
	m_nLeft(0),
	m_nRight(0),
	m_nBottom(0),
	m_nTop(0),
	m_m4dModel(Matrix4<double>(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0)),
	m_m4dView(Matrix4<double>(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0)),
	m_m4dModelView(Matrix4<double>(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0)),
	m_m4dProjection(Matrix4<double>(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0)),
	m_m4fModelView(Matrix4<float>(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f)),
	m_m4fProjection(Matrix4<float>(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f))
{
	log("Log::View::View(View *view, unsigned int id)");
	updateModelViewMatrix();
}
Camera::~Camera()
{
	log("Log::Camera::~Camera()");
}

unsigned int Camera::getID() const { return m_nId; }

double Camera::getDepth(int x, int y) const
{
	double objz{ 0 };
	glReadPixels((GLint)x, (GLint)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &objz);
	return objz;
}

Vector3<double> Camera::UnProject(const POINT point, const double z)
{
	double winx = (double)point.x;
	double winy = (double)point.y;
	double winz = (double)z;

	double modelview[16] = { m_m4dModelView[0],m_m4dModelView[1],m_m4dModelView[2],m_m4dModelView[3],
	m_m4dModelView[4],m_m4dModelView[5],m_m4dModelView[6],m_m4dModelView[7],
	m_m4dModelView[8],m_m4dModelView[9],m_m4dModelView[10],m_m4dModelView[11],
	m_m4dModelView[12],m_m4dModelView[13],m_m4dModelView[14],m_m4dModelView[15] };

	double projection[16] = { m_m4dProjection[0],m_m4dProjection[1],m_m4dProjection[2],m_m4dProjection[3],
	m_m4dProjection[4],m_m4dProjection[5],m_m4dProjection[6],m_m4dProjection[7],
	m_m4dProjection[8],m_m4dProjection[9],m_m4dProjection[10],m_m4dProjection[11],
	m_m4dProjection[12],m_m4dProjection[13],m_m4dProjection[14],m_m4dProjection[15] };

	int viewport[4] = { 0,0,m_lpView->m_szSize.cx << 1,m_lpView->m_szSize.cy << 1 };

	double objx{ 0.0 }, objy{ 0.0 }, objz{ 0.0 };

	gluUnProject(winx, winy, winz, modelview, projection, viewport, &objx, &objy, &objz);

	objx += static_cast<double>(m_lpView->m_szSize.cx * m_lpView->m_dMagnification);
	objy += static_cast<double>(m_lpView->m_szSize.cy * m_lpView->m_dMagnification);

	return Vector3<double>{ objx, objy, objz };
}
POINT Camera::Project(const double x, const double y) { return Project(x, y, 0.0); }
POINT Camera::Project(const double x, const double y, const double z)
{
	Vector4<double> object_coordinates = { x, y, z, 1.0 };

	Vector4<double> screen_coordinates = m_m4dProjection * m_m4dModelView * object_coordinates;
	Vector3<double> normalized_screen_coordinates = Vector3<double>(screen_coordinates.x, screen_coordinates.y, screen_coordinates.z) / screen_coordinates.w;

	return POINT{ static_cast<long>(normalized_screen_coordinates.x * m_lpView->m_szSize.cx),static_cast<long>(normalized_screen_coordinates.y * m_lpView->m_szSize.cy) };
}

void Camera::setView(View* view)
{
	//m_lpView = view;
	m_lpView = new View(*view);//Copy View class with copy constructor.



	updateModelMatrix();
	updateViewMatrix();
	updateModelViewMatrix();
	updateProjectionMatrix();
}
View* Camera::getView() { return m_lpView; }

void Camera::MagnificationToFOV()
{
	double dDistanceToObjectPlane = m_lpView->m_dNear - m_lpView->m_v3dCamera.z;
	double dHalfHeightOfObjectPlane = static_cast<double>(m_lpView->m_szSize.cy * m_lpView->m_dMagnification);
	double dTanFOV = dHalfHeightOfObjectPlane / dDistanceToObjectPlane;

	double dFOV = RAD2DEG<double>(atan(dTanFOV));

	double dDifference = abs(m_lpView->m_dFieldOfView - dFOV);
	if (dDifference < EPSILON<double>()) {
		return;
	}

	m_lpView->m_dFieldOfView = dFOV;
}
void Camera::FOVToMagnification()
{
	double dDistanceToObjectPlane = m_lpView->m_dNear - m_lpView->m_v3dCamera.z;
	double dMagnification = (dDistanceToObjectPlane * tan(DEG2RAD<double>(m_lpView->m_dFieldOfView))) / m_lpView->m_szSize.cy;

	double dDifference = abs(m_lpView->m_dMagnification - dMagnification);
	if (dDifference < EPSILON<double>()) {
		return;
	}

	m_lpView->m_dMagnification = dMagnification;
}

bool Camera::setPresetView(const View::PresetView presetview)
{
	if (m_lpView->m_enPresetView == presetview) {
		return false;
	}

	m_lpView->m_v3dTarget = { 0.0, 0.0, 0.0 };
	m_lpView->m_v3dCamera = { 0.0, 0.0, -502.0 };
	m_lpView->m_v3dDirection = { 0.0, 0.0, 0.0 };

	m_lpView->m_dMagnification = 1.0;
	MagnificationToFOV();

	//setProjection(Projection::Orthographic);
	switch (presetview) {
		case View::PresetView::Top: {
			m_lpView->m_v3dDirection = { 0.0, 0.0, 0.0 };
			break;
		}
		case View::PresetView::Bottom: {
			m_lpView->m_v3dDirection = { PI<double>(), 0.0, PI<double>() };
			break;
		}
		case View::PresetView::Left: {
			m_lpView->m_v3dDirection = { -PI<double>() / 2.0, 0.0, PI<double>() / 2.0 };
			break;
		}
		case View::PresetView::Right: {
			m_lpView->m_v3dDirection = { -PI<double>() / 2.0, 0.0, -PI<double>() / 2.0 };
			break;
		}
		case View::PresetView::Front: {
			m_lpView->m_v3dDirection = { -PI<double>() / 2.0, 0.0, 0.0 };
			break;
		}
		case View::PresetView::Back: {
			m_lpView->m_v3dDirection = { -PI<double>() / 2.0, 0.0, PI<double>() };
			break;
		}
		case View::PresetView::SWIsometric: {
			m_lpView->m_v3dDirection = { -PI<double>() / 4.0, 0.0, PI<double>() / 4.0 };
			break;
		}
		case View::PresetView::SEIsometric: {
			m_lpView->m_v3dDirection = { -PI<double>() / 4.0, 0.0, PI<double>() / 4.0 - PI<double>() / 2.0 };
			break;
		}
		case View::PresetView::NEIsometric: {
			m_lpView->m_v3dDirection = { -PI<double>() / 4.0, 0.0, PI<double>() / 4.0 - PI<double>() };
			break;
		}
		case View::PresetView::NWIsometric: {
			m_lpView->m_v3dDirection = { -PI<double>() / 4.0, 0.0, PI<double>() / 4.0 + PI<double>() / 2.0 };
			break;
		}
		default: {
			m_lpView->m_enPresetView = View::PresetView::Top;
			m_lpView->m_v3dDirection = { 0.0, 0.0, 0.0 };
		}
	}

	m_lpView->m_enPresetView = presetview;
	updateModelViewMatrix();

	return true;
}
View::PresetView Camera::getPresetView()
{
	if ((m_lpView->m_v3dDirection.x == 0.0) || (abs(m_lpView->m_v3dDirection.x) == PI<double>()/4.0) || (abs(m_lpView->m_v3dDirection.x) == PI<double>() / 2.0) || (abs(m_lpView->m_v3dDirection.x) == PI<double>() / 4.0 * 3.0)) {
		if ((m_lpView->m_v3dDirection.z == 0.0) || (abs(m_lpView->m_v3dDirection.z) == PI<double>() / 4.0) || (abs(m_lpView->m_v3dDirection.z) == PI<double>() / 2.0) || (abs(m_lpView->m_v3dDirection.z) == PI<double>() / 4.0 * 3.0)) {

		}
		else {
			m_lpView->m_enPresetView = View::PresetView::Current;
		}
	}
	else {
		m_lpView->m_enPresetView = View::PresetView::Current;
	}
	return m_lpView->m_enPresetView;
}

const char* Camera::viewToString(View::PresetView presetview) { return m_lpView->viewToString(presetview); }
View::PresetView Camera::stringToView(const char* presetviewName) { return m_lpView->stringToView(presetviewName); }

bool Camera::setVisualStyle(const View::VisualStyle visualstyle) {
	if (m_lpView->m_enVisualStyle == visualstyle) {
		return false;
	}

	m_lpView->m_enVisualStyle = visualstyle;

	return true;
}
View::VisualStyle Camera::getVisualStyle() const { return m_lpView->m_enVisualStyle; }

const char* Camera::visualstyleToString(View::VisualStyle visualstyle) { return m_lpView->visualstyleToString(visualstyle); }
View::VisualStyle Camera::stringToVisualstyle(const char* visualstyleName) { return m_lpView->stringToVisualstyle(visualstyleName); }

bool Camera::setPerspective(const bool perspective)
{
	if (m_lpView->m_bPerspective == perspective) {
		return false;
	}

	if (m_lpView->m_bPerspective == false) {
		FOVToMagnification();
	}
	else if (m_lpView->m_bPerspective == true) {
		MagnificationToFOV();
	}

	m_lpView->m_bPerspective = perspective;
	updateProjectionMatrix();

	return true;

}
bool Camera::getPerspective() const { return m_lpView->m_bPerspective; }

void Camera::setDirection(double x, double y, double z) { m_lpView->m_v3dDirection = { x, y, z }; }

void Camera::setNear(const double n) { m_lpView->m_dNear = n; }
double Camera::getNear() const { return m_lpView->m_dNear; }

void Camera::setFar(const double f) { m_lpView->m_dFar = f; }
double Camera::getFar() const {return m_lpView->m_dFar;}

void Camera::setClipping(const bool clipping) { m_lpView->m_bClipping = clipping; }
bool Camera::getClipping() const { return m_lpView->m_bClipping; }

void Camera::setMagnification(const double magnification) { m_lpView->m_dMagnification = magnification; }
double Camera::getMagnification() const { return m_lpView->m_dMagnification; }

void Camera::setFOV(const double fov) { m_lpView->m_dFieldOfView = fov; }
double Camera::getFOV() const { return m_lpView->m_dFieldOfView; }

void Camera::setPosition(const double x, const double y, const double z)
{
	Vector3<double> dv = { x,y,z };
	m_lpView->m_v3dCamera = dv * m_lpView->m_dMagnification;
	updateModelViewMatrix();
}
Vector3<double> Camera::getPosition() const { return m_lpView->m_v3dCamera; }

void Camera::setDistance(const double distance)
{
	if (distance == 0.0) return;

	Vector3<double> dv = { 0.0,0.0,distance };
	dv *= c_scalePan;
	dv *= m_lpView->m_dMagnification;

	m_lpView->m_v3dCamera += dv;

	updateModelViewMatrix();
}
double Camera::getDistance() const { return m_lpView->m_v3dCamera.z - m_lpView->m_dNear; }

Vector3<double> Camera::getLeftAxis() const { return Vector3<double>(-m_m4dView[0], -m_m4dView[4], -m_m4dView[8]); }
Vector3<double> Camera::getUpAxis() const { return Vector3<double>(m_m4dView[1], m_m4dView[5], m_m4dView[9]); }
Vector3<double> Camera::getForwardAxis() const { return Vector3<double>(-m_m4dView[2], -m_m4dView[6], -m_m4dView[10]); }

const Matrix4<double>& Camera::getModelMatrixd() { return m_m4dModel; }
const Matrix4<double>& Camera::getViewMatrixd() { return m_m4dView; }
const Matrix4<double>& Camera::getModelViewMatrixd() { return m_m4dModelView; }
const Matrix4<double>& Camera::getProjectionMatrixd() { return m_m4dProjection; }

const Matrix4<float>& Camera::getModelViewMatrixf() { return m_m4fModelView; }
const Matrix4<float>& Camera::getProjectionMatrixf() { return m_m4fProjection; }

const Matrix4<float>& Camera::getModelViewRotationMatrixf()
{
	m_m4dModelViewRotation = m_m4fModelView;
	m_m4dModelViewRotation.setColumn(3, Vector4<float>{0.f, 0.f, 0.f, 1.f});
	return m_m4dModelViewRotation;
}

const double* Camera::getModelMatrixElementsd() { return m_m4dModel.get(); }
const double* Camera::getViewMatrixElementsd() { return m_m4dView.get(); }
const double* Camera::getModelViewMatrixElementsd() { return m_m4dModelView.get(); }
const double* Camera::getProjectionMatrixElementsd() { return m_m4dProjection.get(); }

const float* Camera::getModelViewMatrixElementsf() { return m_m4fModelView.get(); }
const float* Camera::getProjectionMatrixElementsf() { return m_m4fProjection.get(); }

bool Camera::isViewOrthogonal()//TODO - make this function work for multiples of pi and pi/2
{
	if ((m_lpView->m_v3dDirection.x == 0.0) && (m_lpView->m_v3dDirection.z == 0.0)) {
		return true;
	}

	if ((abs(m_lpView->m_v3dDirection.x) == PI<double>()) && (abs(m_lpView->m_v3dDirection.z) == PI<double>())) {
		return true;
	}

	if (abs(m_lpView->m_v3dDirection.x) == PI<double>() / 2.0) {
		if ((abs(m_lpView->m_v3dDirection.z) == 0.0) || (abs(m_lpView->m_v3dDirection.z) == PI<double>() / 2.0) || (abs(m_lpView->m_v3dDirection.z) == PI<double>())) {
			return true;
		}
	}

	return false;
}

void Camera::fov(double dv)
{
	dv *= c_scaleFOV;

	if (abs(dv) < EPSILON<double>()) return;

	m_lpView->m_dFieldOfView += dv;

	//if (m_dFieldOfView < 0.0) {
	//	m_dFieldOfView = 0.0;
	//}
	//if (m_dFieldOfView > 90.0) {
	//	m_dFieldOfView = 90.0;
	//}

	updateProjectionMatrix();
}
void Camera::move(const double x, const double y, const double z)
{
	Vector3<double> dv = { x,y,z };
	dv *= c_scalePan;
	dv *= m_lpView->m_dMagnification;

	if (dv.length() < EPSILON<double>()) return;

	m_lpView->m_v3dCamera += dv;

	updateModelViewMatrix();
}
void Camera::rotate(double x, double y, double z)
{
	Vector3<double> dv = { x,y,z };
	dv *= DEG2RAD<double>(c_scaleRotate);

	if (dv.length() < EPSILON<double>()) return;

	m_lpView->m_v3dDirection.x += dv.x;
	m_lpView->m_v3dDirection.y += dv.y;
	m_lpView->m_v3dDirection.z += dv.z;

	//Clamp m_v3dDirection rotation angles to 0<=val<=2*pi
	//m_v3dDirection.x = m_v3dDirection.x - TAU<double>() * std::floor(m_v3dDirection.x / TAU<double>());
	//m_v3dDirection.y = m_v3dDirection.y - TAU<double>() * std::floor(m_v3dDirection.y / TAU<double>());
	//m_v3dDirection.z = m_v3dDirection.z - TAU<double>() * std::floor(m_v3dDirection.z / TAU<double>());

	//m_enPresetView = View::Current;
	updateModelViewMatrix();
}
void Camera::zoom(const double z)
{
	double dv = z;
	dv *= c_scaleZoom;

	if (abs(dv) < EPSILON<double>()) return;

	m_lpView->m_dMagnification += m_lpView->m_dMagnification * dv;

	if (m_lpView->m_dMagnification < 0.0) {
		m_lpView->m_dMagnification = 0.0;
	}

	updateProjectionMatrix();
}
void Camera::zoom(const int x, const int y, const double z)
{
	UNREFERENCED_PARAMETER(x);
	UNREFERENCED_PARAMETER(y);

	double dv = z;
	dv *= c_scaleZoom;

	if (abs(dv) < EPSILON<double>()) return;

	m_lpView->m_dMagnification += m_lpView->m_dMagnification * dv;

	if (m_lpView->m_dMagnification < 0.0) {
		m_lpView->m_dMagnification = 0.0;
	}

	//double dStep = 0.25;
	//double dSign = -abs(z) / z;
	//POINT ptOrigin = Project((double)position.x, (double)position.y);
	//SIZE ptOffset = { ptOrigin.x - position.x,ptOrigin.y - position.y };
	//
	//ptOffset.cx *= (long)m_dbMagnification;
	//ptOffset.cy *= (long)m_dbMagnification;
	//
	//Vector2<double> v2dOffset = { (double)(dSign * ptOffset.cx),(double)(dSign * ptOffset.cy) };
	//
	//move(dStep*v2dOffset.x, dStep*v2dOffset.y);

	updateProjectionMatrix();
}

void Camera::size(const int left, const int bottom, const int right, const int top)
{
	//log("Log::Camera::size() { %d, %d }, { %d, %d }", left, bottom, right, top);

	m_nLeft = left;
	m_nRight = right;
	m_nBottom = bottom;
	m_nTop = top;

	int width = m_nRight - m_nLeft;
	int height = m_nTop - m_nBottom;

	width = width / 2;
	height = height / 2;

	m_lpView->m_szSize = { width, height };

	updateProjectionMatrix();
}

Matrix4<double> Camera::setOrthoFrustum(double l, double r, double b, double t, double n, double f)
{
	Matrix4<double> matrix;
	matrix[0] = 2 / (r - l);
	matrix[5] = 2 / (t - b);
	matrix[10] = -2 / (f - n);
	matrix[12] = -(r + l) / (r - l);
	matrix[13] = -(t + b) / (t - b);
	matrix[14] = -(f + n) / (f - n);
	return matrix;
}
Matrix4<double> Camera::setFrustum(double l, double r, double b, double t, double n, double f)
{
	Matrix4<double> matrix;
	matrix[0] = 2 * n / (r - l);
	matrix[5] = 2 * n / (t - b);
	matrix[8] = (r + l) / (r - l);
	matrix[9] = (t + b) / (t - b);
	matrix[10] = -(f + n) / (f - n);
	matrix[11] = -1;
	matrix[14] = -(2 * f * n) / (f - n);
	return matrix;
}
Matrix4<double> Camera::setFrustum(double fovY, double aspectRatio, double front, double back)
{
	//double tangent = tan(DEG2RAD<double>(fovY / 2));
	double tangent = tan(DEG2RAD<double>(fovY));
	double py = front * tangent;
	double px = py * aspectRatio;

	return setFrustum(-px, px, -py, py, front, back);
}

void Camera::updateModelMatrix() { m_m4dModel.identity(); }
void Camera::updateViewMatrix()
{
	//Rotation.
	//Vector3d<double> Xx = { 1.0,0.0,0.0 };
	Vector3<double> Xy = { 0.0,cos(m_lpView->m_v3dDirection.x),sin(m_lpView->m_v3dDirection.x) };
	Vector3<double> Xz = { 0.0,-sin(m_lpView->m_v3dDirection.x),cos(m_lpView->m_v3dDirection.x) };

	Matrix4<double> Rx;
	//Rx.setColumn(0, Xx);
	Rx.setColumn(1, Xy);
	Rx.setColumn(2, Xz);

	Vector3<double> Yx = { cos(m_lpView->m_v3dDirection.y),0.0,-sin(m_lpView->m_v3dDirection.y) };
	//Vector3d<double> Yy = { 0.0,1.0,0.0 };
	Vector3<double> Yz = { sin(m_lpView->m_v3dDirection.y),0.0,cos(m_lpView->m_v3dDirection.y) };

	Matrix4<double> Ry;
	Ry.setColumn(0, Yx);
	//Ry.setColumn(1, Yy);
	Ry.setColumn(2, Yz);

	Vector3<double> Zx = { cos(m_lpView->m_v3dDirection.z),sin(m_lpView->m_v3dDirection.z),0.0 };
	Vector3<double> Zy = { -sin(m_lpView->m_v3dDirection.z),cos(m_lpView->m_v3dDirection.z),0.0 };
	//Vector3d<double> Zz = { 0.0,0.0,1.0 };

	Matrix4<double> Rz;
	Rz.setColumn(0, Zx);
	Rz.setColumn(1, Zy);
	//Rz.setColumn(2, Zz);

	Matrix4<double> rotation = Rx * Ry * Rz;

	//Translation.
	//Vector3<double> translation = { m_v3dCamera.x,m_v3dCamera.y,0.0 };
	Vector3<double> translation = { m_lpView->m_v3dCamera.x,m_lpView->m_v3dCamera.y,m_lpView->m_v3dCamera.z };

	//Construct matrix.
	m_m4dView = rotation;//The first three columns are from the rotation matrix.
	m_m4dView.setColumn(3, translation);//The fourth column is from the translation vector.
}
void Camera::updateModelViewMatrix()
{
	updateViewMatrix();
	updateModelMatrix();
	m_m4dModelView = m_m4dView * m_m4dModel;

	for (unsigned int i = 0; i < 16; i++) {
		m_m4fModelView[i] = static_cast<float>(m_m4dModelView[i]);
	}
}
void Camera::updateProjectionMatrix()
{
	if (m_lpView->m_bPerspective == false) {
		m_m4dProjection = setOrthoFrustum(
			static_cast<double>(m_nLeft * m_lpView->m_dMagnification),
			static_cast<double>(m_nRight * m_lpView->m_dMagnification),
			static_cast<double>(m_nBottom * m_lpView->m_dMagnification),
			static_cast<double>(m_nTop * m_lpView->m_dMagnification),
			m_lpView->m_dNear,
			m_lpView->m_dFar
		);
	}
	else if (m_lpView->m_bPerspective == true) {
		double aspectRatio = static_cast<double>(m_lpView->m_szSize.cx) / m_lpView->m_szSize.cy;
		m_m4dProjection = setFrustum(m_lpView->m_dFieldOfView, aspectRatio, m_lpView->m_dNear, m_lpView->m_dFar);
	}

	for (unsigned int i = 0; i < 16; i++) {
		m_m4fProjection[i] = static_cast<float>(m_m4dProjection[i]);
	}
}
