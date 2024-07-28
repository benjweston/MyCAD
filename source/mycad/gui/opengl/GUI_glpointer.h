#pragma once

#include "GUI_glcontrol.h"

#include <vector>

namespace mycad {
	/**
	* Declaration of GLPointer class.
	*/
	class GLPointer : GLControl {
	public:
		GLPointer();
		~GLPointer();

		static void setCrosshairSize(int size);
		static void setPicboxSize(int size);

		void drawCommand(POINT position, Colour3<float> colour, bool tintforxyz);
		void draw(POINT position, Colour3<float> colour, bool tintforxyz);
		void draw(SIZE viewport, bool perspective, float fov, Matrix4<float> rotationmatrix, POINT position, Colour3<float> colour, bool tintforxyz);

	private:
		enum { MAX_LOADSTRING = 256 };

		static int s_nCrosshairSize;
		static int s_nPickboxSize;

		Colour3<float> c4fColourX;
		Colour3<float> c4fColourY;
		Colour3<float> c4fColourZ;

		unsigned int m_nVBOid[2];

		Vector2<float> getIntersectionCoordinates(POINT position, Vector2<float> coordinates);

		void pickbox(POINT position, Colour3<float> colour);
		void crosshairs(POINT position, Colour3<float> colour, bool tintforxyz);
		void crosshairs(SIZE viewport, bool perspective, float fov, Matrix4<float> rotationmatrix, POINT position, Colour3<float> colour, bool tintforxyz);

	};
}
