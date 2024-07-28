#pragma once

#include <string>
#include <vector>
#include <windows.h>

namespace mycad {
	namespace GLFreeType {
		struct font_data {
			float m_fpTextHeight = 12.f;

			unsigned int m_nMaxPixelWidth = 0;
			unsigned int m_nMaxPixelHeight = 0;

			SIZE m_szSizes[256]{ 0 };

			std::vector<unsigned int> m_vecTextures;
			unsigned int m_nListbase = 0;

			unsigned int getTextWidth(const std::string& strText);
			unsigned int getTextHeight(const std::string&);
			SIZE getExtents(const std::string& strText);

			void init(const char* fname, unsigned int textheight);
			void clean();
		};
		void print(font_data const &font, POINT position, std::string const &text);
	}
}
