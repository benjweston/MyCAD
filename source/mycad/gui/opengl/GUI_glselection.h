#pragma once

#include "GUI_glcontrol.h"

#include <vector>
#include <windows.h>

namespace mycad {
	/**
	* Declaration of GLSelection class.
	*/
	class GLSelection : public GLControl {
	public:
		GLSelection();
		GLSelection(POINT position);
		virtual ~GLSelection();

		static void setCrossingSelectionColour(Colour4<float> colour);
		static void setWindowSelectionColour(Colour4<float> colour);

		virtual void draw(POINT position, Colour3<float> colour);

	protected:
		static Colour4<float> s_c4fCrossingSelectionColour;
		static Colour4<float> s_c4fWindowSelectionColour;

		bool m_bTestDirection = true;
		int m_nDirection;//0 = CCW, 1 = CW
		int m_nSpace;
		int m_nSquare;

		POINT m_ptStartPosition;
		Colour4<float> m_c4fColour;

		unsigned int m_nVBOid_vertices;
		unsigned int m_nVBOid_indices;

		virtual void drawSelectionIcon(POINT position, Colour3<float> colour);

	};


	/**
	* Declaration of GLWindow class.
	*/
	class GLWindow : public GLSelection {
	public:
		GLWindow();
		GLWindow(POINT position);

		void draw(POINT position, Colour3<float> colour) override;

	private:


	};



	/**
	* Declaration of GLLasso class.
	*/
	class GLLasso : public GLSelection {
	public:
		GLLasso();
		GLLasso(POINT position, Colour3<float> colour);
		~GLLasso();

		void draw(POINT position, Colour3<float> colour) override;

	private:
		std::vector<Vector2<int>> m_v2nVertices;
		std::vector<Colour3<float>> m_c3fColours;
		RECT m_rcBoundingRect;

		bool createPixelatedPath(std::vector<Vector2<int>> path, Colour4<float> colour, std::vector<Vector2<int>>* vertices, std::vector<Colour4<float>>* colours);
		bool createLineVerticesList(std::vector<Vector2<int>> path, Colour4<float> colour, std::vector<Vector2<int>>* vertices, std::vector<Colour4<float>>* colours);
		void resizeBoundingBox(Vector2<int> position);
		void removeDuplicateVertices(Vector2<int> position, Colour3<float> colour);
		void drawFill();
		void drawPath(POINT position, Colour3<float> colour);

	};
}
