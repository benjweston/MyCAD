#pragma once

#include "LIB_colours.h"
#include "LIB_matrices.h"//Includes vectors.h in header

#include <windows.h>

namespace mycad {
	/**
	* Declaration of GLControl class.
	*/
	class GLControl {
	public:
		GLControl();
		virtual ~GLControl();

		virtual void draw(POINT position, Colour3<float> colour);

	protected:
		virtual Vector3<float> getWorldCoordinates(Matrix4<float> rotationmatrix, POINT position, float z, Vector3<float>offset);
		virtual Vector2<float> getProjectedCoordinates(SIZE viewport, bool perspective, float fov, Vector3<float> coordinates);

	};
}
