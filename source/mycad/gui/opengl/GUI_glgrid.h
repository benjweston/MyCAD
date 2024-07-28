#pragma once

#include "LIB_colours.h"
#include "LIB_vectors.h"

#include <windows.h>

namespace mycad {
	/**
	* Declaration of GLGrid class.
	*/
	class GLGrid {
	public:
		GLGrid();
		~GLGrid();

		enum class Style : int {
			Axislines,
			Gridlines,
			Axislines_And_Gridlines
		};

		static void setGridColourMajor(Colour4<float> colour);
		static void setGridColourMinor(Colour4<float> colour);
		static void setTintForXYZ(bool state);

		void draw(SIZE viewportsize, double magnification, bool xy, bool yz, bool zx, Style style);

	private:
		static Colour4<float> s_c4fGridColourMajor;
		static Colour4<float> s_c4fGridColourMinor;
		static bool s_bTintForXYZ;

		Colour4<float> m_c4fGridColourAxisX;
		Colour4<float> m_c4fGridColourAxisY;
		Colour4<float> m_c4fGridColourAxisZ;

		std::vector<Vector4<float>> m_vecColours;

		unsigned int m_nVBOid;

		int getMinimum(SIZE size);
		float getRoundSize(float size, int scaleFactor);

		void drawXAxis(float size);
		void drawYAxis(float size);
		void drawZAxis(float size);

		void drawXYPlane(float size, int major, int minor);
		void drawYZPlane(float size, int major, int minor);
		void drawZXPlane(float size, int major, int minor);

		void drawAxes(SIZE viewportsize, double magnification, bool xy, bool yz, bool zx);
		void drawGrid(SIZE viewportsize, double magnification, bool xy, bool yz, bool zx);

	};
}
