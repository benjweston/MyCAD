#include "GUI_glucsicon.h"

#include "LIB_conversions.h"

#include "glUtil.h"

#include "log.h"

#include <GL/gl.h>
#include "glExtension.h"

#define OPENGL_IMMEDIATE_MODE

using namespace mycad;

GLUCSIcon::Style GLUCSIcon::s_enStyle(GLUCSIcon::Style::Style3D);//Default style in case of error reading xml file.
unsigned int GLUCSIcon::s_nLineWidth(1);//Default linewidth in case of error reading xml file.
unsigned int GLUCSIcon::s_nSize(50);//Default size in case of error reading xml file.

GLUCSIcon::GLUCSIcon()
	: m_nBorder(),
	m_nOffset(),
	m_rcBoundingbox({ 0,0,0,0 }),
	m_nVBOid{ 0,0 }
{
	log("Log::GLUCSIcon::GLUCSIcon()");

	m_nUCSIconSize[0] = s_nSize * 2;
	m_nUCSIconSize[1] = s_nSize;

	m_nPickboxSize[0] = m_nUCSIconSize[0] / 10;
	m_nPickboxSize[1] = m_nUCSIconSize[1] / 10;

	m_nTextwidth = m_nTextheight = m_nUCSIconSize[0] / 7;

	m_nBorder[0] = 2;
	m_nBorder[1] = 6;

	m_nOffset[0] = 15;
	m_nOffset[1] = 60;
}
GLUCSIcon::~GLUCSIcon()
{
	log("Log::GLUCSIcon::~GLUCSIcon()");

	glDeleteBuffersARB(2, m_nVBOid);
}

void GLUCSIcon::setStyle(GLUCSIcon::Style style) { s_enStyle = style; }
GLUCSIcon::Style GLUCSIcon::getStyle() { return s_enStyle; }

void GLUCSIcon::setLineWidth(unsigned int linewidth) { s_nLineWidth = linewidth; }
unsigned int GLUCSIcon::getLineWidth() { return s_nLineWidth; }

void GLUCSIcon::setSize(unsigned int ucsiconsize){ s_nSize = ucsiconsize; }
unsigned int GLUCSIcon::getSize() { return s_nSize; }

int GLUCSIcon::getOffset(int index) { return m_nOffset[index]; }
RECT GLUCSIcon::getBoundingbox() { return m_rcBoundingbox; }

void GLUCSIcon::X(Vector2<int> position, Colour3<float> colour) {
	int vertices[] = {
		position.x, position.y,
		position.x + m_nTextwidth, position.y + m_nTextheight,
		position.x + m_nTextwidth, position.y,
		position.x, position.y + m_nTextheight
	};

	float colours[] = {
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b
	};

	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), &vertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), &colours[0]);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid[0]);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(4));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void GLUCSIcon::Y(Vector2<int> position, Colour3<float> colour) {
	int vertices[] = {
		position.x + m_nTextwidth / 2, position.y,
		position.x + m_nTextwidth / 2, position.y + m_nTextheight / 2,
		position.x + m_nTextwidth / 2, position.y + m_nTextheight / 2,
		position.x, position.y + m_nTextheight,
		position.x + m_nTextwidth / 2, position.y + m_nTextheight / 2,
		position.x + m_nTextwidth, position.y + m_nTextheight
	};

	float colours[] = {
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b
	};

	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), &vertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), &colours[0]);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid[0]);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(6));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void GLUCSIcon::Z(Vector2<int> position, Colour3<float> colour) {
	int vertices[] = {
		position.x, position.y,
		position.x + m_nTextwidth, position.y,
		position.x, position.y,
		position.x + m_nTextwidth, position.y + m_nTextheight,
		position.x, position.y + m_nTextheight,
		position.x + m_nTextwidth, position.y + m_nTextheight
	};

	float colours[] = {
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b,
		colour.r, colour.g, colour.b
	};

	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), &vertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), &colours[0]);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid[0]);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(6));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}

void GLUCSIcon::draw(POINT position, Colour3<float> colour)
{
	UNREFERENCED_PARAMETER(position);
	UNREFERENCED_PARAMETER(colour);
}
void GLUCSIcon::draw(SIZE viewport, float fov, Matrix4<float> rotationmatrix, bool perspective, POINT position, Colour3<float> colour, bool dynamic)
{
	UNREFERENCED_PARAMETER(viewport);
	UNREFERENCED_PARAMETER(fov);
	UNREFERENCED_PARAMETER(rotationmatrix);
	UNREFERENCED_PARAMETER(perspective);
	UNREFERENCED_PARAMETER(position);
	UNREFERENCED_PARAMETER(colour);
	UNREFERENCED_PARAMETER(dynamic);
}



Colour3<float> ModelspaceUCSIcon::s_c3fColour({ 0.f, 0.f, 0.f });//Black
bool ModelspaceUCSIcon::s_bApplySingleColour(true);

ModelspaceUCSIcon::ModelspaceUCSIcon()
	: GLUCSIcon(),
	c4fColourX({ .9f, .3f, .3f }),
	c4fColourY({ .3f, .9f, .3f }),
	c4fColourZ({ .3f, .3f, .9f })
{

}

void ModelspaceUCSIcon::setColour(Colour3<float> colour) { s_c3fColour = colour; }
Colour3<float> ModelspaceUCSIcon::getColour() { return s_c3fColour; }

void ModelspaceUCSIcon::setApplySingleColour(bool applysinglecolour) { s_bApplySingleColour = applysinglecolour; }
bool ModelspaceUCSIcon::getApplySingleColour() { return s_bApplySingleColour; }

void ModelspaceUCSIcon::pickbox(SIZE viewport, float fov, Matrix4<float> rotationmatrix, bool perspective, POINT position, Colour3<float> colour)
{
	Vector3<float> world_coordinates[4] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{(float)-m_nPickboxSize[1], (float)-m_nPickboxSize[1], 0.f}),
		getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{(float)-m_nPickboxSize[1], (float)m_nPickboxSize[1], 0.f}),
		getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{(float)m_nPickboxSize[1], (float)m_nPickboxSize[1], 0.f}),
		getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{(float)m_nPickboxSize[1], (float)-m_nPickboxSize[1], 0.f})
	};

	Vector2<float> vertices[4] = {
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates[0]),
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates[1]),
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates[2]),
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates[3])
	};

	Colour3<float> colours[4] = {
		colour,
		colour,
		colour,
		colour
	};

#ifdef OPENGL_IMMEDIATE_MODE
	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid[0]);

	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(4));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else

#endif
}
void ModelspaceUCSIcon::crosshairs(SIZE viewport, float fov, Matrix4<float> rotationmatrix, bool perspective, POINT position, Colour3<float> colour)
{
	Vector3<float> world_coordinates_x[2] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{(float)m_nUCSIconSize[1], 0.f, 0.f}),
		getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, 0.f, 0.f}) };
	Vector3<float> world_coordinates_y[2] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, (float)m_nUCSIconSize[1], 0.f}),
		getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, 0.f, 0.f}) };
	Vector3<float> world_coordinates_z[2] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f,0.f,(float)m_nUCSIconSize[1]}),
		getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, 0.f, 0.f}) };

	Vector2<float> screen_coordinates_x[2] = { getProjectedCoordinates(viewport, perspective, fov, world_coordinates_x[0]),
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates_x[1]) };
	Vector2<float> screen_coordinates_y[2] = { getProjectedCoordinates(viewport, perspective, fov, world_coordinates_y[0]),
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates_y[1]) };
	Vector2<float> screen_coordinates_z[2] = { getProjectedCoordinates(viewport, perspective, fov, world_coordinates_z[0]),
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates_z[1]) };

	Vector2<float> vertices[6] = {
		screen_coordinates_x[0],
		screen_coordinates_x[1],
		screen_coordinates_y[0],
		screen_coordinates_y[1],
		screen_coordinates_z[0],
		screen_coordinates_z[1]
	};

	Colour3<float> colours[6] = {
		colour,
		colour,
		colour,
		colour,
		colour,
		colour
	};

#ifdef OPENGL_IMMEDIATE_MODE
	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid[0]);

	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glLineWidth(static_cast<float>(s_nLineWidth));//Set linewidth to user defined value.

	glDrawArrays(GL_LINES, 0, (GLsizei)(6));

	glLineWidth(1.f);//Return linewidth to default value(1.f).

	glBindBufferARB(GL_ARRAY_BUFFER, 0);

	X(Vector2<int>((int)(screen_coordinates_x[0].x + m_nBorder[1]), (int)(screen_coordinates_x[0].y + m_nBorder[1])), colour);
	Y(Vector2<int>((int)(screen_coordinates_y[0].x + m_nBorder[1]), (int)(screen_coordinates_y[0].y + m_nBorder[1])), colour);
	Z(Vector2<int>((int)(screen_coordinates_z[0].x + m_nBorder[1]), (int)(screen_coordinates_z[0].y + m_nBorder[1])), colour);
#else

#endif
}
void ModelspaceUCSIcon::draw(SIZE viewport, float fov, Matrix4<float> rotationmatrix, bool perspective, POINT position, Colour3<float> colour, bool dynamic)
{
	m_nUCSIconSize[0] = s_nSize * 2;
	m_nUCSIconSize[1] = s_nSize;

	m_nPickboxSize[0] = m_nUCSIconSize[0] / 10;
	m_nPickboxSize[1] = m_nUCSIconSize[1] / 10;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	if (!dynamic) {
		//pickbox(viewport, fov, rotationmatrix, perspective, position, colour);
		crosshairs(viewport, fov, rotationmatrix, perspective, position, colour);
	}
	else {
		Vector3<float> world_coordinates_x[2] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{(float)m_nUCSIconSize[1], 0.f, 0.f}),
			getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, 0.f, 0.f}) };
		Vector3<float> world_coordinates_y[2] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, (float)m_nUCSIconSize[1], 0.f}),
			getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, 0.f, 0.f}) };
		Vector3<float> world_coordinates_z[2] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f,0.f,(float)m_nUCSIconSize[1]}),
			getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, 0.f, 0.f}) };

		Vector2<float> screen_coordinates_x[2] = { getProjectedCoordinates(viewport, perspective, fov, world_coordinates_x[0]),
			getProjectedCoordinates(viewport, perspective, fov, world_coordinates_x[1]) };
		Vector2<float> screen_coordinates_y[2] = { getProjectedCoordinates(viewport, perspective, fov, world_coordinates_y[0]),
			getProjectedCoordinates(viewport, perspective, fov, world_coordinates_y[1]) };
		Vector2<float> screen_coordinates_z[2] = { getProjectedCoordinates(viewport, perspective, fov, world_coordinates_z[0]),
			getProjectedCoordinates(viewport, perspective, fov, world_coordinates_z[1]) };

		Vector2<float> distance[3];
		distance[0] = { screen_coordinates_x[1].x - screen_coordinates_x[0].x,screen_coordinates_x[1].y - screen_coordinates_x[0].y };
		distance[1] = { screen_coordinates_y[1].x - screen_coordinates_y[0].x,screen_coordinates_y[1].y - screen_coordinates_y[0].y };
		distance[2] = { screen_coordinates_z[1].x - screen_coordinates_z[0].x,screen_coordinates_z[1].y - screen_coordinates_z[0].y };

		POINT ptPosition{ 0,0 };

		if ((long)distance[0].x > ptPosition.x) {
			ptPosition.x = (long)distance[0].x;
		}
		if ((long)distance[1].x > ptPosition.x) {
			ptPosition.x = (long)distance[1].x;
		}
		if ((long)distance[2].x > ptPosition.x) {
			ptPosition.x = (long)distance[2].x;
		}

		if ((long)distance[0].y > ptPosition.y) {
			ptPosition.y = (long)distance[0].y;
		}
		if ((long)distance[1].y > ptPosition.y) {
			ptPosition.y = (long)distance[1].y;
		}
		if ((long)distance[2].y > ptPosition.y) {
			ptPosition.y = (long)distance[2].y;
		}

		ptPosition.x += position.x;
		ptPosition.y += position.y;

		//pickbox(viewport, fov, rotationmatrix, perspective, ptPosition, colour);
		crosshairs(viewport, fov, rotationmatrix, perspective, ptPosition, colour);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}



Colour3<float> LayoutTabUCSIcon::s_c3fColour({ 0.f, 0.f, 1.f });//Blue

LayoutTabUCSIcon::LayoutTabUCSIcon()
	: GLUCSIcon()
{

}

void LayoutTabUCSIcon::setColour(Colour3<float> colour) { s_c3fColour = colour; }
Colour3<float> LayoutTabUCSIcon::getColour() { return s_c3fColour; }

void LayoutTabUCSIcon::pickbox(POINT position, Colour3<float> colour)
{
	Vector2<int> vertices[] = {
		{(int)position.x, (int)position.y},
		{(int)position.x, (int)(position.y + m_nPickboxSize[0])},
		{(int)(position.x + m_nPickboxSize[0]), (int)(position.y + m_nPickboxSize[0])},
		{(int)(position.x + m_nPickboxSize[0]), (int)position.y}
	};

	Colour3<float> colours[] = {
		colour,
		colour,
		colour,
		colour
	};

#ifdef OPENGL_IMMEDIATE_MODE
	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid[0]);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(4));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	m_nVBOid[0] = createVertexBuffer<int>(vertices, 0, 2, GL_INT);
	if (m_nVBOid[0] == 0) {
		return;
	}

	m_nVBOid[1] = createVertexBuffer<float>(colours, 1, 3, GL_FLOAT);
	if (m_nVBOid[1] == 0) {
		return;
	}

	//if (m_glslReady) {
	//	glUseProgram(m_shaderProgram);
	//}

	glVertexPointer(2, GL_INT, 0, NULL);
	glColorPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(4));

	//if (m_glslReady) {
	//	glUseProgram(0);
	//}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}
void LayoutTabUCSIcon::crosshairs(POINT position, Colour3<float> colour)
{
	Vector2<int> vertices[3] = {
		{(int)position.x, (int)position.y},
		{(int)position.x, (int)(position.y + m_nUCSIconSize[0] * cosf(DEG2RAD<float>(30)))},
		{(int)(position.x + m_nUCSIconSize[0] * sinf(DEG2RAD<float>(30))), position.y},
	};

	Colour3<float> colours[3] = {
		colour,
		colour,
		colour
	};

#ifdef OPENGL_IMMEDIATE_MODE
	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid[0]);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(3));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);

	X(Vector2<int>((int)(vertices[0].x + m_nBorder[0] + 20), (int)(vertices[0].y + m_nBorder[0])), colour);
	Y(Vector2<int>((int)(vertices[1].x + m_nBorder[0]), (int)(vertices[1].y + m_nBorder[0] - 50)), colour);
#else

#endif
}
void LayoutTabUCSIcon::draw(POINT position, Colour3<float> colour)
{
	m_nUCSIconSize[0] = s_nSize * 2;
	m_nUCSIconSize[1] = s_nSize;

	m_nPickboxSize[0] = m_nUCSIconSize[0] / 10;
	m_nPickboxSize[1] = m_nUCSIconSize[1] / 10;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//pickbox(position, colour);
	crosshairs(position, colour);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
