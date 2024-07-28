#pragma once

#include "LIB_vectors.h"
#include "LIB_colours.h"

#include "glfreetype.h"

#include <windows.h>

namespace mycad {
	/**
	* Declaration of GLText class.
	*/
	class GLText {
	public:
		GLText();
		GLText(const char* fname, const unsigned int textheight);
		~GLText();

		void setTextColour(Colour4<float> colour);
		Colour4<float> getTextColour() const;

		void setTextHeight(unsigned int textheight);
		unsigned int getTextHeight() const;

		void setText(const char *text);
		char* getText();

		void setSize(SIZE size);
		SIZE getSize() const;

		void setPosition(POINT position);
		POINT getPosition() const;

		void draw();
		void draw(POINT position);

	protected:
		enum { MAX_STRING = 256 };

		char m_chFont[MAX_STRING];
		unsigned int m_u32TextHeight;
		SIZE m_iMinimumSize;

		char chText[MAX_STRING];
		GLFreeType::font_data our_font;

		SIZE m_slText;
		SIZE m_slSize;

		POINT m_ptPosition;

		Colour4<float> m_c4fTextColour;

	};
}
