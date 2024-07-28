#include "GUI_glcontrol.h"

#include "LIB_conversions.h"

#include "log.h"

#include <GL/gl.h>
#include "glExtension.h"

using namespace mycad;

GLControl::GLControl()
{
	log("Log::GLControl::GLControl()");
}
GLControl::~GLControl()
{
	log("Log::GLControl::~GLControl()");
}

//Transforms screen coordinates to world coordinates...
Vector3<float> GLControl::getWorldCoordinates(Matrix4<float> rotationmatrix, POINT position, float z, Vector3<float>offset) {
	Vector4<float> fPosition = Vector4<float>{ (float)position.x, (float)position.y, z, 1.f };//Create local position vector,...
	Vector4<float> fOffset = { static_cast<float>(offset.x), static_cast<float>(offset.y), static_cast<float>(offset.z), 1.f };//...create local offset vector,...
	fPosition += rotationmatrix * fOffset;//...apply the projection transform to the offset vector, add the offset vector to the position vector...

	return Vector3<float>{fPosition.x, fPosition.y, fPosition.z};//...and return the { x, y, z } components of the position vector.
}
//Transforms world coordinates to screen coordinates...
Vector2<float> GLControl::getProjectedCoordinates(SIZE viewport, bool perspective, float fov, Vector3<float> coordinates) {
	float tangent = tanf(DEG2RAD<float>(fov));
	float distance = (float)((viewport.cy >> 1) / tangent);

	float x{ 0.0 };
	float y{ 0.0 };

	if (perspective == false) {
		x = coordinates.x;
		y = coordinates.y;
	}
	else {
		x = distance * (coordinates.x / (distance - coordinates.z));
		y = distance * (coordinates.y / (distance - coordinates.z));
	}

	return Vector2<float>(x, y);
}

void GLControl::draw(const POINT position, Colour3<float> colour) { UNREFERENCED_PARAMETER(position); UNREFERENCED_PARAMETER(colour); }