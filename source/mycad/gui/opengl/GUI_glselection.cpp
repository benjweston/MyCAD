#include "GUI_glselection.h"

#include "log.h"

//#include <GL/gl.h>
#include "glExtension.h"

#include<algorithm>
#include <cassert>

using namespace mycad;

Colour4<float> GLSelection::s_c4fWindowSelectionColour({ 0.f, 0.5f, 1.f, 0.25f });
Colour4<float> GLSelection::s_c4fCrossingSelectionColour({ 0.f, 1.f, 0.25f, 0.25f });

GLSelection::GLSelection()
	: m_nDirection(1),
	m_nSpace(7),
	m_nSquare(10),
	m_ptStartPosition({ 0,0 }),
	m_c4fColour(s_c4fCrossingSelectionColour),
	m_nVBOid_vertices(0),
	m_nVBOid_indices(0)
{
	log("Log::GLSelection::GLSelection()");
}
GLSelection::GLSelection(POINT position)
	: m_nDirection(1),
	m_nSpace(7),
	m_nSquare(10),
	m_ptStartPosition(position),
	m_c4fColour(s_c4fCrossingSelectionColour),
	m_nVBOid_vertices(0),
	m_nVBOid_indices(0)
{
	log("Log::GLSelection::GLSelection(POINT position)");
}
GLSelection::~GLSelection()
{
	log("Log::GLSelection::~GLSelection()");

	glDeleteBuffersARB(1, &m_nVBOid_vertices);
	glDeleteBuffersARB(1, &m_nVBOid_indices);

	m_nVBOid_vertices = 0;
	m_nVBOid_indices = 0;
}

void GLSelection::setCrossingSelectionColour(Colour4<float> colour) { s_c4fCrossingSelectionColour = colour; }
void GLSelection::setWindowSelectionColour(Colour4<float> colour) { s_c4fWindowSelectionColour = colour; }

void GLSelection::drawSelectionIcon(const POINT position, Colour3<float> colour)
{
	Vector2<int> vertices[4] = {
		{(int)position.x + m_nSpace, (int)position.y + m_nSpace},
		{(int)position.x + m_nSpace, (int)position.y + m_nSpace + m_nSquare},
		{(int)position.x + m_nSpace + m_nSquare + 1, (int)position.y + m_nSpace + m_nSquare},
		{(int)position.x + m_nSpace + m_nSquare + 1, (int)position.y + m_nSpace}
	};

	Colour3<float> colours[4] = {
		colour,
		colour,
		colour,
		colour
	};

	glLineStipple(1, 0xCCCC);

	glGenBuffersARB(1, &m_nVBOid_vertices);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(4));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}
void GLSelection::draw(POINT position, Colour3<float> colour) { UNREFERENCED_PARAMETER(position); UNREFERENCED_PARAMETER(colour); }



GLWindow::GLWindow()
	: GLSelection()
{

}
GLWindow::GLWindow(POINT position)
	: GLSelection(position)
{

}

void GLWindow::draw(POINT position, Colour3<float> colour)
{
	Vector2<float> v2nNewPosition = Vector2<float>{ (float)(position.x), (float)(position.y) };

	//Test for direction on each new position.
	if (v2nNewPosition.x != m_ptStartPosition.x) {
		if (v2nNewPosition.x > m_ptStartPosition.x) {
			m_nDirection = 0;//CCW
		}
		else {
			m_nDirection = 1;//CW
		}
	}

	if (m_nDirection == 0) {
		m_c4fColour = s_c4fWindowSelectionColour;
	}
	else if (m_nDirection == 1) {
		m_c4fColour = s_c4fCrossingSelectionColour;
		glEnable(GL_LINE_STIPPLE);
	}

	RECT rcExtents;
	rcExtents.left = m_ptStartPosition.x;
	rcExtents.top = m_ptStartPosition.y;
	rcExtents.right = position.x;
	rcExtents.bottom = position.y;

	Vector2<int>vertices[4];
	if (((rcExtents.right > rcExtents.left) && (rcExtents.bottom > rcExtents.top)) || ((rcExtents.right < rcExtents.left) && (rcExtents.bottom < rcExtents.top))) {
		vertices[0] = { (int)rcExtents.left, (int)rcExtents.bottom };
		vertices[1] = { (int)rcExtents.left, (int)rcExtents.top };
		vertices[2] = { (int)rcExtents.right, (int)rcExtents.top };
		vertices[3] = { (int)rcExtents.right, (int)rcExtents.bottom };
	}
	else if (((rcExtents.right > rcExtents.left) && (rcExtents.bottom < rcExtents.top)) || ((rcExtents.right < rcExtents.left) && (rcExtents.bottom > rcExtents.top))) {
		vertices[0] = { (int)rcExtents.left, (int)rcExtents.bottom };
		vertices[1] = { (int)rcExtents.right, (int)rcExtents.bottom };
		vertices[2] = { (int)rcExtents.right, (int)rcExtents.top };
		vertices[3] = { (int)rcExtents.left, (int)rcExtents.top };
	}

	Colour4<float>colours[4];
	for (unsigned int i = 0; i < 4; i++) {
		colours[i] = m_c4fColour;
	}

	unsigned int indices[4] = { 0, 1, 3, 2 };

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glGenBuffersARB(1, &m_nVBOid_vertices);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glGenBuffersARB(1, &m_nVBOid_indices);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nVBOid_indices);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(indices), indices, GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nVBOid_indices);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid_vertices);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glPolygonMode(GL_FRONT, GL_FILL);
	glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)(4), GL_UNSIGNED_INT, (GLvoid*)(0));

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

	glLineStipple(2, 0xCCCC);

	for (unsigned int i = 0; i < 4; i++) {
		colours[i] = { colour.r,colour.g,colour.b,1.f };
		indices[i] = i;
	}

	glGenBuffersARB(1, &m_nVBOid_vertices);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(colours), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(colours), colours);

	glGenBuffersARB(1, &m_nVBOid_indices);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nVBOid_indices);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(indices), indices, GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nVBOid_indices);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid_vertices);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(sizeof(vertices)));

	glPolygonMode(GL_BACK, GL_LINES);//Reverse polygon winding - show back instead of front.
	glDrawElements(GL_LINE_LOOP, (GLsizei)(4), GL_UNSIGNED_INT, (GLvoid*)(0));

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	drawSelectionIcon(position, colour);

	glPolygonMode(GL_FRONT, GL_LINES);//Restore polygon mode to GL_FRONT

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	if (m_nDirection == 1) {
		glDisable(GL_LINE_STIPPLE);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



GLLasso::GLLasso()
	: GLSelection(),
	m_rcBoundingRect({ 0,0,0,0 })
{

}
GLLasso::GLLasso(POINT position, Colour3<float> colour)
	: GLSelection(position)
{
	//logMode(LOG_MODE_BOTH);

	m_v2nVertices.push_back(Vector2<int>{position.x, position.y});
	m_c3fColours.push_back(colour);
	m_rcBoundingRect = { position.x, position.y, position.x, position.y };
}
GLLasso::~GLLasso() {
	m_v2nVertices.clear();
	m_v2nVertices = std::vector<Vector2<int>>();
	m_c3fColours.clear();
	m_c3fColours = std::vector<Colour3<float>>();
}

bool GLLasso::createPixelatedPath(std::vector<Vector2<int>> path, Colour4<float> colour, std::vector<Vector2<int>>* vertices, std::vector<Colour4<float>>* colours)
{
	if (path.size() < 3) {//If the path has less than 3 vertices it is not a polygon and does not require filling.
		return false;
	}

	vertices->push_back(path[0]);
	colours->push_back(colour);
	for (unsigned int i = 1; i < path.size(); ++i) {
		Vector2<int> v2nVertex1 = path[i - 1];
		Vector2<int> v2nVertex2 = path[i];
		Vector2<int> v2nDVertex = v2nVertex2 - v2nVertex1;
		if (abs(v2nDVertex.y) > 1) {
			int dy = 1;
			if (v2nDVertex.y < 0) {
				dy = -1;
			}
			for (int j = 1; j < abs(v2nDVertex.y); ++j) {
				int dx = (j * v2nDVertex.x) / abs(v2nDVertex.y);
				Vector2<int> v2nNewVertex = { v2nVertex1.x + dx, v2nVertex1.y + j * dy };
				vertices->push_back(v2nNewVertex);
				colours->push_back(colour);
			}
		}
		vertices->push_back(path[i]);
		colours->push_back(colour);
	}
	//Don't forget to return to the start point...
	Vector2<int> v2nVertex1 = path.back();
	Vector2<int> v2nVertex2 = path.front();
	Vector2<int> v2nDVertex = v2nVertex2 - v2nVertex1;
	if (abs(v2nDVertex.y) > 1) {
		int dy = 1;
		if (v2nDVertex.y < 0) {
			dy = -1;
		}
		for (int j = 1; j < abs(v2nDVertex.y); ++j) {
			int dx = (j * v2nDVertex.x) / abs(v2nDVertex.y);
			Vector2<int> v2nNewVertex = { v2nVertex1.x + dx, v2nVertex1.y + j * dy };
			vertices->push_back(v2nNewVertex);
			colours->push_back(colour);
		}
	}

	if (vertices->size() < 4) {//Any polygon with less than 4 pixel vertices will not have an interior area to fill.
		return false;
	}

	return true;
}
bool GLLasso::createLineVerticesList(std::vector<Vector2<int>> path, Colour4<float> colour, std::vector<Vector2<int>>*vertices, std::vector<Colour4<float>>*colours)
{
	if (path.size() < 3) {//If the path has less than 3 vertices it is not a polygon and does not require filling.
		return false;
	}

	for (int i = m_rcBoundingRect.bottom; i < m_rcBoundingRect.top; ++i) {
		std::vector<Vector2<int>> newpath;
		for (unsigned int j = 0; j < path.size(); ++j) {//Get all vertices with this y value...
			if (path[j].y == i) {
				newpath.push_back(path[j]);
			}
		}
		std::sort(newpath.begin(), newpath.end(), std::less<Vector2<int>>());//Sort the vector by ascending numeric value...
		//Using the Even-Odd rule...
		for (unsigned int k = 0; k < (newpath.size() - 1); k += 2) {//Get each pair of vertices - GL_LINES will draw a line between each second pair starting with the zeroth vertex
			vertices->push_back(newpath[k]);
			colours->push_back(colour);
			vertices->push_back(newpath[static_cast<size_t>(k) + 1]);
			colours->push_back(colour);
		}
		//TODO - Using the Non-zero Winding rule...
		//
		//
	}

	if (vertices->size() < 2) {
		return false;
	}

	return true;
}
void GLLasso::resizeBoundingBox(Vector2<int> position)
{
	if (position.x < m_rcBoundingRect.left) {
		m_rcBoundingRect.left = position.x;
	}
	else if (position.x > m_rcBoundingRect.right) {
		m_rcBoundingRect.right = position.x;
	}

	if (position.y < m_rcBoundingRect.bottom) {
		m_rcBoundingRect.bottom = position.y;
	}
	else if (position.y > m_rcBoundingRect.top) {
		m_rcBoundingRect.top = position.y;
	}
}
void GLLasso::removeDuplicateVertices(Vector2<int> position, Colour3<float> colour)
{
	if (position != m_v2nVertices.back()) {//If new vertex is not equal to previous vertex...
		size_t nSize = (int)m_v2nVertices.size();
		if (nSize > 1) {
			Vector2<int> nDf1 = position - m_v2nVertices[nSize - 2];
			Vector2<int> nDf2 = m_v2nVertices[nSize - 1] - m_v2nVertices[nSize - 2];

			//Cross-multiplication creates two vectors with equal y components...
			Vector2<int> nDx12 = nDf1 * nDf2.y;//Multply Df1 by y2
			Vector2<int> nDx21 = nDf2 * nDf1.y;//Multply Df2 by y1

			//...so that checking the equality of the x components is equivalent to checking if the angles are equal without the dividing by 0 problems.
			if (nDx12.x == nDx21.x) {
				m_v2nVertices.pop_back();//Delete in-line vertex.
				m_c3fColours.pop_back();
			}
		}
		m_v2nVertices.push_back(position);
		m_c3fColours.push_back(colour);
	}
}
void GLLasso::drawFill()
{
	std::vector<Vector2<int>> v2nPixelatedPath;
	std::vector<Colour4<float>> c4fPixelatedPathColours;

	bool bPixelatedPath = createPixelatedPath(m_v2nVertices, m_c4fColour, &v2nPixelatedPath, &c4fPixelatedPathColours);
	if (!bPixelatedPath) {//If path has less than 4 vertices...
		return;
	}

	std::vector<Vector2<int>> v2nScanlines;
	std::vector<Colour4<float>> c4fScanlineColours;
	bool bLineVerticesList = createLineVerticesList(v2nPixelatedPath, m_c4fColour, &v2nScanlines, &c4fScanlineColours);
	if (!bLineVerticesList) {//If lines list has less than 2 vertices...
		return;
	}

	glGenBuffersARB(1, &m_nVBOid_vertices);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, v2nScanlines.size() * sizeof(Vector2<int>) + c4fScanlineColours.size() * sizeof(Colour4<float>), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, v2nScanlines.size() * sizeof(Vector2<int>), &v2nScanlines[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, v2nScanlines.size() * sizeof(Vector2<int>), c4fScanlineColours.size() * sizeof(Colour4<float>), &c4fScanlineColours[0]);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(v2nScanlines.size() * sizeof(Vector2<int>)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(v2nScanlines.size()));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}
void GLLasso::drawPath(POINT position, Colour3<float> colour)
{
	if (m_nDirection == 1) {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(2, 0xCCCC);
	}

	glGenBuffersARB(1, &m_nVBOid_vertices);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid_vertices);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_v2nVertices.size() * sizeof(Vector2<int>) + m_c3fColours.size() * sizeof(Colour3<float>), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_v2nVertices.size() * sizeof(Vector2<int>), &m_v2nVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_v2nVertices.size() * sizeof(Vector2<int>), m_c3fColours.size() * sizeof(Colour3<float>), &m_c3fColours[0]);

	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_INT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_v2nVertices.size() * sizeof(Vector2<int>)));

	glPolygonMode(GL_FRONT, GL_LINES);
	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(m_v2nVertices.size()));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);

	drawSelectionIcon(position, colour);

	if (m_nDirection == 1) {
		glDisable(GL_LINE_STIPPLE);
	}
}

void GLLasso::draw(POINT position, Colour3<float> colour)
{
	Vector2<int> v2nNewPosition = Vector2<int>{ position.x, position.y };
	if (m_bTestDirection == true) {
		if (v2nNewPosition.x != m_ptStartPosition.x) {
			if (v2nNewPosition.x > m_ptStartPosition.x) {
				m_nDirection = 0;//CCW
			}
			else {
				m_nDirection = 1;//CW
			}
			m_bTestDirection = false;
		}
	}

	if (m_nDirection == 0) {
		m_c4fColour = s_c4fWindowSelectionColour;
	}
	else if (m_nDirection == 1) {
		m_c4fColour = s_c4fCrossingSelectionColour;
	}
	
	removeDuplicateVertices(v2nNewPosition, colour);
	resizeBoundingBox(v2nNewPosition);

	if (m_v2nVertices.size() < 1) {
		return;
	}

	drawFill();

	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

	drawPath(position, colour);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT, GL_LINES);//Restore polygon mode to GL_FRONT
}
