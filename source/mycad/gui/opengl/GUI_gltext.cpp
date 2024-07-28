#include "GUI_gltext.h"

#include "log.h"

#include <GL/gl.h>
#include "glExtension.h"

#include <strsafe.h>

using namespace mycad;

GLText::GLText()
	: m_u32TextHeight(14),
	m_iMinimumSize({ 6, 12 }),
	chText{},
	m_slText({ 0,0 }),
	m_slSize({ 0,0 }),
	m_ptPosition({ 0,0 }),
	m_c4fTextColour({ 0.f,0.f,0.f,1.f })
{
	log("Log::GLText::GLText()");

	::strcpy_s(m_chFont, "C:/Windows/Fonts/Arial.ttf");
	our_font.init(m_chFont, 12);
}
GLText::GLText(const char* fname, const unsigned int textheight)
	: m_u32TextHeight(14),
	m_iMinimumSize({ 6, 12 }),
	chText{},
	m_slText({ 0,0 }),
	m_slSize({ 0,0 }),
	m_ptPosition({ 0,0 }),
	m_c4fTextColour({ 0.f,0.f,0.f,1.f })
{
	log("Log::GLText::GLText(const char* fname, const unsigned int textheight)");

	::strcpy_s(m_chFont, fname);
	m_iMinimumSize.cy = textheight;
	our_font.init(fname, textheight);
}
GLText::~GLText()
{
	log("Log::GLText::~GLText()");

	our_font.clean();
}

void GLText::setTextColour(Colour4<float> colour) {
	m_c4fTextColour = colour;
}
Colour4<float> GLText::getTextColour() const {
	return m_c4fTextColour;
}

void GLText::setTextHeight(unsigned int textheight) {
	m_u32TextHeight = textheight;
	m_iMinimumSize.cy = (int)textheight;
	our_font.init(m_chFont, textheight);
}
unsigned int GLText::getTextHeight() const {
	return m_u32TextHeight;
}

void GLText::setText(const char *text) {
	::strcpy_s(chText, text);
	m_slSize = m_slText = our_font.getExtents(chText);
	if (m_slSize.cx < m_iMinimumSize.cx) {
		m_slSize.cx = m_iMinimumSize.cx;
	}
	if (m_slSize.cy < (int)m_u32TextHeight) {
		m_slSize.cy = (int)m_u32TextHeight;
	}
}
char* GLText::getText() {
	return chText;
}

void GLText::setSize(SIZE size) {
	m_slSize = size;
}
SIZE GLText::getSize() const {
	return m_slSize;
}

void GLText::setPosition(POINT position) { m_ptPosition = position; }
POINT GLText::getPosition() const { return m_ptPosition; }

void GLText::draw() {
	glColor4f((GLfloat)m_c4fTextColour.r, (GLfloat)m_c4fTextColour.g, (GLfloat)m_c4fTextColour.b, (GLfloat)m_c4fTextColour.a);
	GLFreeType::print(our_font, m_ptPosition, chText);
}

void GLText::draw(POINT position) {
	m_ptPosition = position;
	draw();
}
