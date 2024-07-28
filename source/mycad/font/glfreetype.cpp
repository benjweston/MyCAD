#include "glfreetype.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include FT_FREETYPE_H

#include <GL/gl.h>
#include "glExtension.h"

#include "log.h"

#include <iostream>
#include <sstream>

namespace mycad {
	namespace GLFreeType {
		FT_BitmapGlyph generateBitmapForFace(FT_Face face, char ch) {
			if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT)) {
				throw std::runtime_error("FT_Load_Glyph() failed");
			}

			FT_Glyph glyph;
			if (FT_Get_Glyph(face->glyph, &glyph)) {
				throw std::runtime_error("FT_Get_Glyph() failed");
			}

			FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);

			return (FT_BitmapGlyph)glyph;
		}

		inline void storeTextureData(const int width, const int height, FT_Bitmap & bitmap, std::vector<GLubyte> & expanded_data) {
			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {
					expanded_data[2 * (i + j * width)] = 255;
					expanded_data[2 * (i + j * width) + 1] = (i >= static_cast<int>(bitmap.width) || j >= static_cast<int>(bitmap.rows)) ? static_cast<GLubyte>(0) : bitmap.buffer[i + static_cast<int>(bitmap.width) * j];
				}
			}
		}

		unsigned int font_data::getTextWidth(const std::string& strText) {
			unsigned int nPixelLength{ 0 };
			for (unsigned int i = 0; i < strText.size(); i++) {
				nPixelLength += m_szSizes[static_cast<unsigned int>(strText[i])].cx;
			}
			return nPixelLength;
		}
		unsigned int font_data::getTextHeight(const std::string&) { return m_nMaxPixelHeight; }
		SIZE font_data::getExtents(const std::string& strText) {
			SIZE m_szExtents{ 0,0 };
			m_szExtents.cx = getTextWidth(strText);
			m_szExtents.cy = getTextHeight(strText);
			return m_szExtents;
		}

		inline int next_p2(int a) {
			int rval = 1;
			while (rval < a) rval <<= 1;//Multiply rval by 2 (rval *= 2)
			return rval;
		}
		void make_dlist(FT_Face face, char ch, unsigned int listbase, GLuint* tex_base, unsigned int* puiMaxPixelWidth, unsigned int* puiMaxPixelHeight, SIZE* pSizes) {
			FT_BitmapGlyph bitmap_glyph = generateBitmapForFace(face, ch);
			FT_Bitmap& bitmap = bitmap_glyph->bitmap;

			int nWidth = next_p2(static_cast<int>(bitmap.width));
			int nHeight = next_p2(static_cast<int>(bitmap.rows));

			std::vector<GLubyte> expanded_data(2 * nWidth * nHeight, 0);

			storeTextureData(nWidth, nHeight, bitmap, expanded_data);

			glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)&expanded_data.front());

			expanded_data.clear();

			//glBindTexture(GL_TEXTURE_2D, 0);

			glNewList(listbase + ch, GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
			glPushMatrix();
			glTranslatef(static_cast<float>(bitmap_glyph->left), static_cast<float>(bitmap_glyph->top - (bitmap.rows * 0.9)), 0.f);
			//glTranslatef(0.f, static_cast<float>(bitmap_glyph->top - (bitmap.rows * 0.9)), 0.f);

			float x = static_cast<float>(bitmap.width) / static_cast<float>(nWidth);
			float y = static_cast<float>(bitmap.rows) / static_cast<float>(nHeight);

			pSizes[ch].cx = static_cast<LONG>(bitmap.width);
			pSizes[ch].cy = static_cast<LONG>(bitmap.rows);
			if (bitmap.width > *puiMaxPixelWidth) {
				*puiMaxPixelWidth = bitmap.width;
			}
			if (bitmap.rows > *puiMaxPixelHeight) {
				*puiMaxPixelHeight = bitmap.rows;
			}

			glBegin(GL_QUADS);
			glTexCoord2f(0.f, 0.f); glVertex2i(0, static_cast<int>(bitmap.rows));
			glTexCoord2f(0.f, y); glVertex2i(0, 0);
			glTexCoord2f(x, y); glVertex2i(static_cast<int>(bitmap.width), 0);
			glTexCoord2f(x, 0.f); glVertex2i(static_cast<int>(bitmap.width), static_cast<int>(bitmap.rows));
			glEnd();

			//glBindTexture(GL_TEXTURE_2D, 0);

			glPopMatrix();
			glTranslatef(static_cast<float>(face->glyph->advance.x >> 6), 0.f, 0.f);

			pSizes[ch].cx = static_cast<LONG>(face->glyph->advance.x >> 6);
			glBitmap(0, 0, 0.f, 0.f, static_cast<float>(pSizes[ch].cx), 0.f, NULL);

			glEndList();
		}

		void font_data::init(const char* fname, unsigned int textheight) {
			m_vecTextures.resize(128);

			m_fpTextHeight = static_cast<float>(textheight);

			FT_Library library;
			if (FT_Init_FreeType(&library)) {
				throw std::runtime_error("FT_Init_FreeType() failed");
			}

			FT_Face face;
			if (FT_New_Face(library, fname, 0, &face)) {
				throw std::runtime_error("FT_New_Face() failed (there is probably a problem with your font file)");
			}

			FT_Set_Char_Size(face, textheight << 6, textheight << 6, 96, 96);
			FT_Set_Pixel_Sizes(face, 0, textheight);

			FT_Select_Charmap(face, FT_ENCODING_MS_SYMBOL);//FT_ENCODING_UNICODE

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			m_nListbase = glGenLists(128);
			glGenTextures(128, &m_vecTextures.front());

			for (unsigned char i = 0; i < 128; i++) {
				make_dlist(face, i, m_nListbase, &m_vecTextures.front(), &m_nMaxPixelWidth, &m_nMaxPixelHeight, m_szSizes);
			}

			FT_Done_Face(face);
			FT_Done_FreeType(library);
		}
		void font_data::clean() {
			glDeleteLists(m_nListbase, 128);
			glDeleteTextures(128, &m_vecTextures.front());
			//glBindTexture(GL_TEXTURE_2D, 0);
			m_vecTextures.clear();
		}

		void print(const font_data &font, POINT position, std::string const & text) {
			unsigned int nFont = font.m_nListbase;
			float fHeight = font.m_fpTextHeight / .63f;

			std::stringstream ss(text);
			std::string to;
			std::vector<std::string> lines;
			while (std::getline(ss, to, '\n')) {
				lines.push_back(to);
			}

			glEnable(GL_TEXTURE_2D);

			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_FILL);

			glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);

			glListBase(nFont);

			glMatrixMode(GL_MODELVIEW);
			float modelview_matrix[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

			float rpos[4];
			float len;
			for (int i = 0; i < (int)lines.size(); i++) {
				glPushMatrix();
				glLoadIdentity();
				glTranslatef(static_cast<float>(position.x), static_cast<float>(position.y - fHeight * i), 0.f);
				glMultMatrixf(modelview_matrix);
				glCallLists(static_cast<GLsizei>(lines[i].length()), GL_UNSIGNED_BYTE, (GLvoid*)lines[i].c_str());
				glGetFloatv(GL_CURRENT_RASTER_POSITION, rpos);//needs the glBitmap commands in make_dlist().
				glPopMatrix();
				len = rpos[0];
			}
			glPopAttrib();

			glDisable(GL_TEXTURE_2D);
		}
	}
}
