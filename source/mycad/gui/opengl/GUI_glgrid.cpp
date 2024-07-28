#include "GUI_glgrid.h"

#include "log.h"

#include <GL/gl.h>
#include "glExtension.h"

#include <windowsx.h>

using namespace mycad;

Colour4<float> GLGrid::s_c4fGridColourMajor{ 0.28f, 0.31f, 0.41f, .7f };
Colour4<float> GLGrid::s_c4fGridColourMinor{ 0.19f, 0.21f, 0.28f, .7f };
bool GLGrid::s_bTintForXYZ(true);

GLGrid::GLGrid()
	: m_c4fGridColourAxisX({ .9f, .3f, .3f, .7f }),
	m_c4fGridColourAxisY({ .3f, .9f, .3f, .7f }),
	m_c4fGridColourAxisZ({ .3f, .3f, .9f, .7f }),
	m_nVBOid(0)
{
	log("Log::GLGrid::GLGrid()");
}

GLGrid::~GLGrid()
{
	log("Log::GLGrid::~GLGrid()");

	//glDeleteBuffersARB(1, &m_nVBOid);
	m_nVBOid = 0;

	//m_vecColours.erase(m_vecColours.begin(), m_vecColours.end());
	//m_vecColours.clear();
}

void GLGrid::setGridColourMajor(Colour4<float> colour) { s_c4fGridColourMajor = colour; }
void GLGrid::setGridColourMinor(Colour4<float> colour) { s_c4fGridColourMinor = colour; }
void GLGrid::setTintForXYZ(bool state) { s_bTintForXYZ = state; }

int GLGrid::getMinimum(SIZE size) {
	int minimum = 0;
	size.cx < size.cy ? minimum = size.cx : minimum = size.cy;
	return minimum;
}

float GLGrid::getRoundSize(float size, int scaleFactor) {
	//float intpart{};
	float round = size / scaleFactor;
	//float fractpart = modf(round, &intpart);
	int divisions = (int)round;

	float roundedsize = (float)(divisions * scaleFactor);

	return roundedsize;
}

void GLGrid::drawXAxis(float size) {
	Vector3<float>vertices[2] = {
		Vector3<float>(0, 0, 0),
		Vector3<float>((float)size, 0, 0),
	};

	Colour4<float>colours[2] = {
		m_c4fGridColourAxisX,
		m_c4fGridColourAxisX,
	};

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(2));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void GLGrid::drawYAxis(float size) {
	Vector3<float>vertices[2] = {
		Vector3<float>(0, 0, 0),
		Vector3<float>(0, (float)size, 0),
	};

	Colour4<float>colours[2] = {
		m_c4fGridColourAxisY,
		m_c4fGridColourAxisY,
	};

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(2));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void GLGrid::drawZAxis(float size) {
	Vector3<float>vertices[2] = {
		Vector3<float>(0, 0, 0),
		Vector3<float>(0, 0, (float)size)
	};

	Colour4<float>colours[2] = {
		m_c4fGridColourAxisZ,
		m_c4fGridColourAxisZ
	};

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(2));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}

void GLGrid::drawAxes(SIZE viewportsize, double magnification, bool xy, bool yz, bool zx) {
	int extents = getMinimum(viewportsize);
	//extents /= 2;
	extents = extents >> 1;
	extents = (int)(extents * magnification);

	float size = (float)extents;
	bool bScaleFactor = false;
	int iScaleFactor = 1;
	while (!bScaleFactor) {
		iScaleFactor *= 10;
		if (extents < iScaleFactor) {
			size = getRoundSize((float)extents, iScaleFactor / 10);
			bScaleFactor = true;
		}
	}
	size *= 1.1f;

	glLineWidth(2);

	if ((xy) || (zx)) {
		drawXAxis(size);
	}

	if ((yz) || (xy)) {
		drawYAxis(size);
	}

	if ((zx) || (yz)) {
		drawZAxis(size);
	}

	glLineWidth(1);
}

void GLGrid::drawXYPlane(float size, int major, int minor) {
	float majorspacing = (float)(size / major);
	float minorspacing = (float)(majorspacing / minor);

	std::vector<Vector3<float>> vertlist;

	vertlist.push_back(Vector3<float>(0, 0, 0));
	vertlist.push_back(Vector3<float>(0, -size, 0));

	for (int i = 1; i <= major; i++) {
		float majorposition = i * majorspacing;
		vertlist.push_back(Vector3<float>(-size, majorposition, 0.f));
		vertlist.push_back(Vector3<float>(size, majorposition, 0.f));
		vertlist.push_back(Vector3<float>(-size, -majorposition, 0.f));
		vertlist.push_back(Vector3<float>(size, -majorposition, 0.f));

		vertlist.push_back(Vector3<float>(majorposition, -size, 0.f));
		vertlist.push_back(Vector3<float>(majorposition, size, 0.f));
		vertlist.push_back(Vector3<float>(-majorposition, -size, 0.f));
		vertlist.push_back(Vector3<float>(-majorposition, size, 0.f));

		for (int j = 1; j < minor; j++) {
			majorposition = (i - 1) * majorspacing;
			float minorposition = majorposition + j * minorspacing;
			vertlist.push_back(Vector3<float>(-size, minorposition, 0.f));
			vertlist.push_back(Vector3<float>(size, minorposition, 0.f));
			vertlist.push_back(Vector3<float>(-size, -minorposition, 0.f));
			vertlist.push_back(Vector3<float>(size, -minorposition, 0.f));

			vertlist.push_back(Vector3<float>(minorposition, -size, 0.f));
			vertlist.push_back(Vector3<float>(minorposition, size, 0.f));
			vertlist.push_back(Vector3<float>(-minorposition, -size, 0.f));
			vertlist.push_back(Vector3<float>(-minorposition, size, 0.f));
		}
	}

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, vertlist.size() * sizeof(Vector3<float>) + m_vecColours.size() * sizeof(Vector4<float>), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, vertlist.size() * sizeof(Vector3<float>), &vertlist[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER, vertlist.size() * sizeof(Vector3<float>), m_vecColours.size() * sizeof(Vector4<float>), &m_vecColours[0]);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(vertlist.size() * sizeof(Vector3<float>)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(vertlist.size()));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void GLGrid::drawYZPlane(float size, int major, int minor) {
	float majorspacing = (float)(size / major);
	float minorspacing = (float)(majorspacing / minor);

	std::vector<Vector3<float>> vertlist;

	vertlist.push_back(Vector3<float>(0, 0, 0));
	vertlist.push_back(Vector3<float>(0, 0, -size));

	for (int i = 1; i <= major; i++) {
		float majorposition = i * majorspacing;
		vertlist.push_back(Vector3<float>(0.f, -size, majorposition));
		vertlist.push_back(Vector3<float>(0.f, size, majorposition));
		vertlist.push_back(Vector3<float>(0.f, -size, -majorposition));
		vertlist.push_back(Vector3<float>(0.f, size, -majorposition));

		vertlist.push_back(Vector3<float>(0.f, majorposition, -size));
		vertlist.push_back(Vector3<float>(0.f, majorposition, size));
		vertlist.push_back(Vector3<float>(0.f, -majorposition, -size));
		vertlist.push_back(Vector3<float>(0.f, -majorposition, size));
		for (int j = 1; j < minor; j++) {
			majorposition = (i - 1) * majorspacing;
			float minorposition = majorposition + j * minorspacing;
			vertlist.push_back(Vector3<float>(0.f, -size, minorposition));
			vertlist.push_back(Vector3<float>(0.f, size, minorposition));
			vertlist.push_back(Vector3<float>(0.f, -size, -minorposition));
			vertlist.push_back(Vector3<float>(0.f, size, -minorposition));

			vertlist.push_back(Vector3<float>(0.f, minorposition, -size));
			vertlist.push_back(Vector3<float>(0.f, minorposition, size));
			vertlist.push_back(Vector3<float>(0.f, -minorposition, -size));
			vertlist.push_back(Vector3<float>(0.f, -minorposition, size));
		}
	}

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, vertlist.size() * sizeof(Vector3<float>) + m_vecColours.size() * sizeof(Vector4<float>), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, vertlist.size() * sizeof(Vector3<float>), &vertlist[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER, vertlist.size() * sizeof(Vector3<float>), m_vecColours.size() * sizeof(Vector4<float>), &m_vecColours[0]);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(vertlist.size() * sizeof(Vector3<float>)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(vertlist.size()));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void GLGrid::drawZXPlane(float size, int major, int minor) {
	float majorspacing = (float)(size / major);
	float minorspacing = (float)(majorspacing / minor);

	std::vector<Vector3<float>> vertlist;

	vertlist.push_back(Vector3<float>(0, 0, 0));
	vertlist.push_back(Vector3<float>(-size, 0, 0));

	for (int i = 1; i <= major; i++) {
		float majorposition = i * majorspacing;
		vertlist.push_back(Vector3<float>(-size, 0.f, majorposition));
		vertlist.push_back(Vector3<float>(size, 0.f, majorposition));
		vertlist.push_back(Vector3<float>(-size, 0.f, -majorposition));
		vertlist.push_back(Vector3<float>(size, 0.f, -majorposition));

		vertlist.push_back(Vector3<float>(majorposition, 0.f, -size));
		vertlist.push_back(Vector3<float>(majorposition, 0.f, size));
		vertlist.push_back(Vector3<float>(-majorposition, 0.f, -size));
		vertlist.push_back(Vector3<float>(-majorposition, 0.f, size));
		for (int j = 1; j < minor; j++) {
			majorposition = (i - 1) * majorspacing;
			float minorposition = majorposition + j * minorspacing;
			vertlist.push_back(Vector3<float>(-size, 0.f, minorposition));
			vertlist.push_back(Vector3<float>(size, 0.f, minorposition));
			vertlist.push_back(Vector3<float>(-size, 0.f, -minorposition));
			vertlist.push_back(Vector3<float>(size, 0.f, -minorposition));

			vertlist.push_back(Vector3<float>(minorposition, 0.f, -size));
			vertlist.push_back(Vector3<float>(minorposition, 0.f, size));
			vertlist.push_back(Vector3<float>(-minorposition, 0.f, -size));
			vertlist.push_back(Vector3<float>(-minorposition, 0.f, size));
		}
	}

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, vertlist.size() * sizeof(Vector3<float>) + m_vecColours.size() * sizeof(Vector4<float>), 0, GL_STATIC_DRAW);
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, vertlist.size() * sizeof(Vector3<float>), &vertlist[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER, vertlist.size() * sizeof(Vector3<float>), m_vecColours.size() * sizeof(Vector4<float>), &m_vecColours[0]);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(vertlist.size() * sizeof(Vector3<float>)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(vertlist.size()));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}

void GLGrid::drawGrid(SIZE viewportsize, double magnification, bool xy, bool yz, bool zx) {
	int extents = getMinimum(viewportsize);
	//extents /= 2;
	extents = extents >> 1;
	extents = (int)(extents * magnification);

	float size = (float)extents;
	bool bScaleFactor = false;
	int iScaleFactor = 1;
	while (!bScaleFactor) {
		iScaleFactor *= 10;
		if (extents < iScaleFactor) {
			size = getRoundSize((float)extents, iScaleFactor / 10);
			bScaleFactor = true;
		}
	}

	int major = 2;
	//float round, intpart;
	//round = extents / 100;
	//modf(round, &intpart);
	//int major = (int)round;
	int minor = 5;

	m_vecColours.clear();
	
	m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));
	m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));
	
	for (int i = 1; i <= major; i++) {
		m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));
		m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));
		m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));
		m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));

		m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));
		m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));
		m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));
		m_vecColours.push_back(Vector4<float>(s_c4fGridColourMajor.r, s_c4fGridColourMajor.g, s_c4fGridColourMajor.b, s_c4fGridColourMajor.a));
		for (int j = 1; j < minor; j++) {
			m_vecColours.push_back(Vector4<float>(s_c4fGridColourMinor.r, s_c4fGridColourMinor.g, s_c4fGridColourMinor.b, s_c4fGridColourMinor.a));
			m_vecColours.push_back(Vector4<float>(s_c4fGridColourMinor.r, s_c4fGridColourMinor.g, s_c4fGridColourMinor.b, s_c4fGridColourMinor.a));
			m_vecColours.push_back(Vector4<float>(s_c4fGridColourMinor.r, s_c4fGridColourMinor.g, s_c4fGridColourMinor.b, s_c4fGridColourMinor.a));
			m_vecColours.push_back(Vector4<float>(s_c4fGridColourMinor.r, s_c4fGridColourMinor.g, s_c4fGridColourMinor.b, s_c4fGridColourMinor.a));

			m_vecColours.push_back(Vector4<float>(s_c4fGridColourMinor.r, s_c4fGridColourMinor.g, s_c4fGridColourMinor.b, s_c4fGridColourMinor.a));
			m_vecColours.push_back(Vector4<float>(s_c4fGridColourMinor.r, s_c4fGridColourMinor.g, s_c4fGridColourMinor.b, s_c4fGridColourMinor.a));
			m_vecColours.push_back(Vector4<float>(s_c4fGridColourMinor.r, s_c4fGridColourMinor.g, s_c4fGridColourMinor.b, s_c4fGridColourMinor.a));
			m_vecColours.push_back(Vector4<float>(s_c4fGridColourMinor.r, s_c4fGridColourMinor.g, s_c4fGridColourMinor.b, s_c4fGridColourMinor.a));
		}
	}

	if (xy) {
		drawXYPlane(size, major, minor);//Draw XY plane.
	}

	if (yz) {
		drawYZPlane(size, major, minor);//Draw YZ plane.
	}

	if (zx) {
		drawZXPlane(size, major, minor);//Draw ZX plane.
	}
}

void GLGrid::draw(SIZE viewportsize, double magnification, bool xy, bool yz, bool zx, Style style) {
	//magnification = 1.0;
	//viewportsize = {500,350};
	//if ((style == Style::Axislines) || (style == Style::Axislines_And_Gridlines)) {
	//	drawAxes(viewportsize, magnification, xy, yz, zx);
	//}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	if ((style == Style::Axislines_And_Gridlines) || (style == Style::Gridlines)) {
		drawGrid(viewportsize, magnification, xy, yz, zx);
	}
	if ((style == Style::Axislines_And_Gridlines) || (style == Style::Axislines)) {
		drawAxes(viewportsize, magnification, xy, yz, zx);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
