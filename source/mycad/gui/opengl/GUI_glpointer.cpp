#include "GUI_glpointer.h"

#include "LIB_conversions.h"

#include "glUtil.h"

#include "log.h"

#include <GL/gl.h>

#define OPENGL_IMMEDIATE_MODE

using namespace mycad;

int GLPointer::s_nCrosshairSize(50);
int GLPointer::s_nPickboxSize(3);

GLPointer::GLPointer()
	: c4fColourX({ .9f, .3f, .3f }),
	c4fColourY({ .3f, .9f, .3f }),
	c4fColourZ({ .3f, .3f, .9f }),
	m_nVBOid{ 0,0 }
{
	log("Log::GLPointer::GLPointer()");
}
GLPointer::~GLPointer()
{
	log("Log::GLPointer::~GLPointer()");
	glDeleteBuffersARB(2, m_nVBOid);
}

void GLPointer::setCrosshairSize(int size) { s_nCrosshairSize = size; }
void GLPointer::setPicboxSize(int size) { s_nPickboxSize = size; }

void GLPointer::drawCommand(POINT position, Colour3<float> colour, bool tintforxyz) {
	Vector2<int> vertices[4] = {
		{(int)(position.x - s_nCrosshairSize + 1), (int)position.y},
		{(int)(position.x + s_nCrosshairSize - 1), (int)position.y },
		{(int)position.x, (int)(position.y - s_nCrosshairSize + 1)},
		{(int)position.x, (int)(position.y + s_nCrosshairSize - 1)}
	};

	Colour3<float> colours[4];
	if (tintforxyz == false) {
		for (unsigned int i = 0; i < 4; i++) {
			colours[i] = colour;
		}
	}
	else if (tintforxyz == true) {
		for (unsigned int i = 0; i < 2; i++) {
			colours[i] = c4fColourX;
			colours[i + 2] = c4fColourY;
		}
	}

	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(4));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void GLPointer::draw(POINT position, Colour3<float> colour, bool tintforxyz)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	crosshairs(position, colour, tintforxyz);
	pickbox(position, colour);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
void GLPointer::draw(SIZE viewport, bool perspective, float fov, Matrix4<float> rotationmatrix, POINT position, Colour3<float> colour, bool tintforxyz)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	crosshairs(viewport, perspective, fov, rotationmatrix, position, colour, tintforxyz);
	pickbox(position, colour);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

Vector2<float> GLPointer::getIntersectionCoordinates(POINT position, Vector2<float> coordinates)
{
	float dx{ coordinates.x - position.x };
	float dy{ coordinates.y - position.y };

	int sx{ 0 };
	int sy{ 0 };

	float cx{ 0 };
	float cy{ 0 };

	if (dx == 0) {
		sx = 0;
	}
	else {
		sx = (int)(dx / abs(dx));
	}

	if (dy == 0) {
		sy = 0;
	}
	else {
		sy = (int)(dy / abs(dy));
	}

	if (abs(dx) > abs(dy)) {
		cx = (float)(sx * s_nPickboxSize);
		cy = (float)(sx * s_nPickboxSize * dy / dx);
	}
	else if (abs(dx) == abs(dy)) {
		cx = (float)(sx * s_nPickboxSize);
		cy = (float)(sy * s_nPickboxSize);
	}
	else {
		cx = (float)(sy * s_nPickboxSize * dx / dy);
		cy = (float)(sy * s_nPickboxSize);
	}

	Vector2<float> intersection;
	if ((abs(dx) < s_nPickboxSize) && (abs(dy) < s_nPickboxSize)) {
		intersection = coordinates;
	}
	else {
		intersection = { (float)(position.x + cx), (float)(position.y + cy) };
	}

	return intersection;
}

void GLPointer::pickbox(POINT position, Colour3<float> colour)
{
	RECT pickbox;
	pickbox.left = position.x - s_nPickboxSize;
	pickbox.top = position.y + s_nPickboxSize;
	pickbox.right = position.x + s_nPickboxSize;
	pickbox.bottom = position.y - s_nPickboxSize;

	Vector2<int> vertices[4] = {
		{(int)pickbox.left, (int)pickbox.bottom},
		{(int)pickbox.left, (int)pickbox.top},
		{(int)pickbox.right, (int)pickbox.top},
		{(int)pickbox.right, (int)pickbox.bottom}
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
void GLPointer::crosshairs(POINT position, Colour3<float> colour, bool tintforxyz)
{
	Vector2<int> vertices[8] = {
		{(int)(position.x - s_nCrosshairSize), (int)position.y},
		{(int)(position.x - s_nPickboxSize), (int)position.y},
		{(int)(position.x + s_nPickboxSize), (int)position.y},
		{(int)(position.x + s_nCrosshairSize), (int)position.y},
		{(int)position.x, (int)(position.y - s_nCrosshairSize)},
		{(int)position.x, (int)(position.y - s_nPickboxSize)},
		{(int)position.x, (int)(position.y + s_nPickboxSize)},
		{(int)position.x, (int)(position.y + s_nCrosshairSize)}
	};

	Colour3<float> colours[8];
	if (tintforxyz == false) {
		for (unsigned int i = 0; i < 8; i++) {
			colours[i] = colour;
		}
	}
	else if (tintforxyz == true) {
		for (unsigned int i = 0; i < 4; i++) {
			colours[i] = c4fColourX;
			colours[i + 4] = c4fColourY;
		}
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid[0]);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(8));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else

#endif
}
void GLPointer::crosshairs(SIZE viewport, bool perspective, float fov, Matrix4<float> rotationmatrix, POINT position, Colour3<float> colour, bool tintforxyz)
{
	float fCrosshairSize[2] = { static_cast<float>(s_nCrosshairSize) / 3 * 2,static_cast<float>(s_nCrosshairSize) / 3 * 4 };

	Vector3<float> world_coordinates_x[2] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{-fCrosshairSize[0], 0.f, 0.f}),
		getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{fCrosshairSize[1], 0.f, 0.f}) };
	Vector3<float> world_coordinates_y[2] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, -fCrosshairSize[0], 0.f}),
		getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, fCrosshairSize[1], 0.f}) };
	Vector3<float> world_coordinates_z[2] = { getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f,0.f,-fCrosshairSize[0]}),
		getWorldCoordinates(rotationmatrix,position,0.f,Vector3<float>{0.f, 0.f, fCrosshairSize[1]}) };

	Vector2<float> screen_coordinates_x[2] = { getProjectedCoordinates(viewport, perspective, fov, world_coordinates_x[0]),
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates_x[1]) };
	Vector2<float> screen_coordinates_y[2] = { getProjectedCoordinates(viewport, perspective, fov, world_coordinates_y[0]),
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates_y[1]) };
	Vector2<float> screen_coordinates_z[2] = { getProjectedCoordinates(viewport, perspective, fov, world_coordinates_z[0]),
		getProjectedCoordinates(viewport, perspective, fov, world_coordinates_z[1]) };

	Vector2<float> intersection_coordinates_x[2] = { getIntersectionCoordinates(position, screen_coordinates_x[0]),
		getIntersectionCoordinates(position, screen_coordinates_x[1]) };
	Vector2<float> intersection_coordinates_y[2] = { getIntersectionCoordinates(position, screen_coordinates_y[0]),
		getIntersectionCoordinates(position, screen_coordinates_y[1]) };
	Vector2<float> intersection_coordinates_z[2] = { getIntersectionCoordinates(position, screen_coordinates_z[0]),
		getIntersectionCoordinates(position, screen_coordinates_z[1]) };

	Vector2<float> vertices[12] = {
		screen_coordinates_x[0],
		intersection_coordinates_x[0],
		intersection_coordinates_x[1],
		screen_coordinates_x[1],
		screen_coordinates_y[0],
		intersection_coordinates_y[0],
		intersection_coordinates_y[1],
		screen_coordinates_y[1],
		screen_coordinates_z[0],
		intersection_coordinates_z[0],
		intersection_coordinates_z[1],
		screen_coordinates_z[1]
	};

	Colour3<float> colours[12];
	if (tintforxyz == false) {
		for (unsigned int i = 0; i < 12; i++) {
			colours[i] = colour;
		}
	}
	else if (tintforxyz == true) {
		for (unsigned int i = 0; i < 4; i++) {
			colours[i] = c4fColourX;
			colours[i + 4] = c4fColourY;
			colours[i + 8] = c4fColourZ;
		}
	}

	glGenBuffersARB(1, &m_nVBOid[0]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid[0]);

	glVertexPointer(2, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(12));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
