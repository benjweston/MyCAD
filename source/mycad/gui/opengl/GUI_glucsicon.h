#pragma once

#include "GUI_glcontrol.h"

namespace mycad {
	/**
	* Declaration of GLUCSIcon class.
	*/
	class GLUCSIcon : public GLControl {
	public:
		GLUCSIcon();
		virtual ~GLUCSIcon();

		enum class Style : int {
			Style2D,
			Style3D
		};

		static void setStyle(Style style);
		static Style getStyle();

		static void setLineWidth(unsigned int linewidth);
		static unsigned int getLineWidth();

		static void setSize(unsigned int ucsiconsize);
		static unsigned int getSize();

		int getOffset(int index);
		RECT getBoundingbox();

		virtual void draw(POINT position, Colour3<float> colour);
		virtual void draw(SIZE viewport, float fov, Matrix4<float> rotationmatrix, bool perspective, POINT position, Colour3<float> colour, bool dynamic = false);

	protected:
		int m_nBorder[2];

		int m_nUCSIconSize[2];
		int m_nPickboxSize[2];

		int m_nTextwidth;
		int m_nTextheight;

		int m_nOffset[2];
		RECT m_rcBoundingbox;

		unsigned int m_nVBOid[2];

		void X(Vector2<int> position, Colour3<float> colour);
		void Y(Vector2<int> position, Colour3<float> colour);
		void Z(Vector2<int> position, Colour3<float> colour);

		static unsigned int s_nSize;
		static unsigned int s_nLineWidth;
		static Style s_enStyle;

	};



	class ModelspaceUCSIcon : public GLUCSIcon {
	public:
		ModelspaceUCSIcon();

		static void setColour(Colour3<float> colour);
		static Colour3<float> getColour();

		static void setApplySingleColour(bool applysinglecolour);
		static bool getApplySingleColour();

		void draw(SIZE viewport, float fov, Matrix4<float> rotationmatrix, bool perspective, POINT position, Colour3<float> colour, bool dynamic = false) override;

	private:
		static Colour3<float> s_c3fColour;
		static bool s_bApplySingleColour;

		Colour3<float> c4fColourX;
		Colour3<float> c4fColourY;
		Colour3<float> c4fColourZ;

		void crosshairs(SIZE viewport, float fov, Matrix4<float> rotationmatrix, bool perspective, POINT position, Colour3<float> colour);
		void pickbox(SIZE viewport, float fov, Matrix4<float> rotationmatrix, bool perspective, POINT position, Colour3<float> colour);

	};



	class LayoutTabUCSIcon : public GLUCSIcon {
	public:
		LayoutTabUCSIcon();

		static void setColour(Colour3<float> colour);
		static Colour3<float> getColour();

		void draw(POINT position, Colour3<float> colour) override;

	private:
		static Colour3<float> s_c3fColour;

		void crosshairs(POINT position, Colour3<float> colour);
		void pickbox(POINT position, Colour3<float> colour);

	};
}
