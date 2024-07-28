#include "entities.h"
#include "LIB_constants.h"
#include "glUtil.h"

#include "fault.h"
#include "log.h"

#include "glExtension.h"

#define OPENGL_IMMEDIATE_MODE

using namespace mycad;

Entity::Entity()
	: m_glslReady(false),
	m_vboReady(false),
	m_vsId(0),
	m_fsId(0),
	m_shaderProgram(0),
	m_nVBOid(0),
	m_inPositionLocation(0),
	m_inColorLocation(0),
	m_vecVertices(0),
	m_iMode(GL_LINES),
	bRedraw(true)
{
	vsSource = R"(
		attribute vec4 in_Position;
		void main(void) {
			gl_Position = gl_ModelViewProjectionMatrix * in_Position;
		}
	)";

	fsSource = R"(
		uniform vec4 inColor;
		void main(){
			gl_FragColor = inColor;
		} 
	)";
}
Entity::~Entity() {
	if (m_glslReady) {
		deleteProgram(m_shaderProgram, m_vsId, m_fsId);
	}
	glDeleteBuffersARB(1, &m_nVBOid);
}

Colour3<int> Entity::IndexToVector3iColour(int index) {
	Colour3<int>colour3i = { 0,0,0 };
	colour3i[0] = dxfColors[index][0];
	colour3i[1] = dxfColors[index][1];
	colour3i[2] = dxfColors[index][2];
	return colour3i;
}
Colour3<float> Entity::IndexToVector3fColour(int index) {
	Colour3<float>colour3f = { 0.f,0.f,0.f };
	colour3f[0] = dxfColors[index][0] / 255.f;
	colour3f[1] = dxfColors[index][1] / 255.f;
	colour3f[2] = dxfColors[index][2] / 255.f;
	return colour3f;
}

DRW_Entity* Entity::getEntity() { return nullptr; }

void Entity::redraw() { bRedraw = true; }

void Entity::create() {}
void Entity::create(Colour3<float> colour) { UNREFERENCED_PARAMETER(colour); }
void Entity::paint(Colour3<float> colour) { UNREFERENCED_PARAMETER(colour); }



Block::Block()
{

}



Point::Point(DRW_Point* entity)
	: Entity(),
	m_lpEntity(entity)
{
	int pointSize = 5;
	float hyp = sqrtf((float)((pointSize * pointSize) >> 1));

	m_vecVertices.push_back((float)(m_lpEntity->basePoint.x) - hyp);
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.y) - hyp);
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.z));
	m_vecVertices.push_back(1.f);

	m_vecVertices.push_back((float)(m_lpEntity->basePoint.x) + hyp);
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.y) + hyp);
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.z));
	m_vecVertices.push_back(1.f);

	m_vecVertices.push_back((float)(m_lpEntity->basePoint.x) - hyp);
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.y) + hyp);
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.z));
	m_vecVertices.push_back(1.f);

	m_vecVertices.push_back((float)(m_lpEntity->basePoint.x) + hyp);
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.y) - hyp);
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.z));
	m_vecVertices.push_back(1.f);
}

DRW_Entity* Point::getEntity() { return m_lpEntity; }

void Point::create(Colour3<float> colour) {
#ifdef OPENGL_IMMEDIATE_MODE
	m_vecColours.clear();

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float) + m_vecColours.size() * sizeof(float), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_vecVertices.size() * sizeof(float), &m_vecVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float), m_vecColours.size() * sizeof(float), &m_vecColours[0]);
#else
	if (!m_glslReady) {
		glExtension& extension = glExtension::getInstance();
		bool glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported) {
			m_vsId = createShader(GL_VERTEX_SHADER, vsSource);
			if (!m_vsId) {
				return;
			}

			m_fsId = createShader(GL_FRAGMENT_SHADER, fsSource);
			if (!m_fsId) {
				return;
			}

			m_shaderProgram = createProgram(m_vsId, m_fsId);
			if (!m_shaderProgram) {
				return;
			}

			m_glslReady = true;
		}
	}

	m_nVBOid = createVertexBuffer<float>(m_vecVertices);
	if (m_nVBOid == 0) {
		return;
	}

	m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "in_Position");
	m_inColorLocation = glGetUniformLocation(m_shaderProgram, "inColor");
#endif

	//bRedraw = false;
}
void Point::paint(Colour3<float> colour) {
	if (bRedraw) {
		create(colour);
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_vecVertices.size() * sizeof(float)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(4));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexAttribPointer(m_inPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	if (m_glslReady) {
		glUseProgramObjectARB(m_shaderProgram);
		glUniform4f(m_inColorLocation, colour.r, colour.g, colour.b, 1.0);
	}

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glDrawArrays(GL_LINES, 0, (GLsizei)(4));

	if (m_glslReady) {
		glUseProgram(0);
	}

	glDisableVertexAttribArray(m_inPositionLocation);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}



Line::Line(DRW_Line* entity)
	: Entity(),
	m_lpEntity(entity)
{
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.x));
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.y));
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.z));
	m_vecVertices.push_back(1.f);

	m_vecVertices.push_back((float)(m_lpEntity->secPoint.x));
	m_vecVertices.push_back((float)(m_lpEntity->secPoint.y));
	m_vecVertices.push_back((float)(m_lpEntity->secPoint.z));
	m_vecVertices.push_back(1.f);
}

DRW_Entity* Line::getEntity() { return m_lpEntity; }

void Line::create(Colour3<float> colour)
{
#ifdef OPENGL_IMMEDIATE_MODE
	m_vecColours.clear();

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float) + m_vecColours.size() * sizeof(float), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_vecVertices.size() * sizeof(float), &m_vecVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float), m_vecColours.size() * sizeof(float), &m_vecColours[0]);
#else
	if (!m_glslReady) {
		glExtension& extension = glExtension::getInstance();
		bool glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported) {
			m_vsId = createShader(GL_VERTEX_SHADER, vsSource);
			if (!m_vsId) {
				return;
			}

			m_fsId = createShader(GL_FRAGMENT_SHADER, fsSource);
			if (!m_fsId) {
				return;
			}

			m_shaderProgram = createProgram(m_vsId, m_fsId);
			if (!m_shaderProgram) {
				return;
			}

			m_glslReady = true;
		}
	}

	m_nVBOid = createVertexBuffer<float>(m_vecVertices);
	if (m_nVBOid == 0) {
		return;
	}

	m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "in_Position");
	m_inColorLocation = glGetUniformLocation(m_shaderProgram, "inColor");
#endif

	//bRedraw = false;
}
void Line::paint(Colour3<float> colour)
{
	if (bRedraw) {
		create(colour);
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_vecVertices.size() * sizeof(float)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(2));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexAttribPointer(m_inPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(m_inPositionLocation);

	if (m_glslReady) {
		glUseProgramObjectARB(m_shaderProgram);
		glUniform4f(m_inColorLocation, colour.r, colour.g, colour.b, 1.0);
	}

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glDrawArrays(GL_LINES, 0, (GLsizei)(2));

	if (m_glslReady) {
		glUseProgram(0);
	}

	glDisableVertexAttribArray(m_inPositionLocation);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}



Ray::Ray(DRW_Ray* entity)
	: Entity(),
	m_lpEntity(entity)
{
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.x));
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.y));
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.z));
	m_vecVertices.push_back(1.f);

	m_vecVertices.push_back((float)(m_lpEntity->secPoint.x));
	m_vecVertices.push_back((float)(m_lpEntity->secPoint.y));
	m_vecVertices.push_back((float)(m_lpEntity->secPoint.z));
	m_vecVertices.push_back(0.f);
}

DRW_Entity* Ray::getEntity() { return m_lpEntity; }

void Ray::create(Colour3<float> colour) {
#ifdef OPENGL_IMMEDIATE_MODE
	m_vecColours.clear();

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float) + m_vecColours.size() * sizeof(float), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_vecVertices.size() * sizeof(float), &m_vecVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float), m_vecColours.size() * sizeof(float), &m_vecColours[0]);
#else
	if (!m_glslReady) {
		glExtension& extension = glExtension::getInstance();
		bool glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported) {
			m_vsId = createShader(GL_VERTEX_SHADER, vsSource);
			if (!m_vsId) {
				return;
			}

			m_fsId = createShader(GL_FRAGMENT_SHADER, fsSource);
			if (!m_fsId) {
				return;
			}

			m_shaderProgram = createProgram(m_vsId, m_fsId);
			if (!m_shaderProgram) {
				return;
			}

			m_glslReady = true;
		}
	}

	m_nVBOid = createVertexBuffer<float>(m_vecVertices);
	if (m_nVBOid == 0) {
		return;
	}

	m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "in_Position");
	m_inColorLocation = glGetUniformLocation(m_shaderProgram, "inColor");
#endif

	//bRedraw = false;
}
void Ray::paint(Colour3<float> colour) {
	if (bRedraw) {
		create(colour);
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_vecVertices.size() * sizeof(float)));

	glDrawArrays(GL_LINES, 0, (GLsizei)(2));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexAttribPointer(m_inPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(m_inPositionLocation);

	if (m_glslReady) {
		glUseProgramObjectARB(m_shaderProgram);
		glUniform4f(m_inColorLocation, colour.r, colour.g, colour.b, 1.0);
	}

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glDrawArrays(GL_LINES, 0, (GLsizei)(2));

	if (m_glslReady) {
		glUseProgram(0);
	}

	glDisableVertexAttribArray(m_inPositionLocation);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}



Xline::Xline(DRW_Xline* entity)
	: Entity(),
	m_lpEntity(entity)
{
	m_iMode = GL_LINE_STRIP;

	m_vecVertices.push_back((float)(-m_lpEntity->secPoint.x));
	m_vecVertices.push_back((float)(-m_lpEntity->secPoint.y));
	m_vecVertices.push_back((float)(-m_lpEntity->secPoint.z));
	m_vecVertices.push_back(0.f);

	m_vecVertices.push_back((float)(m_lpEntity->basePoint.x));
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.y));
	m_vecVertices.push_back((float)(m_lpEntity->basePoint.z));
	m_vecVertices.push_back(1.f);

	m_vecVertices.push_back((float)(m_lpEntity->secPoint.x));
	m_vecVertices.push_back((float)(m_lpEntity->secPoint.y));
	m_vecVertices.push_back((float)(m_lpEntity->secPoint.z));
	m_vecVertices.push_back(0.f);
}

DRW_Entity* Xline::getEntity() { return m_lpEntity; }

void Xline::create(Colour3<float> colour) {
#ifdef OPENGL_IMMEDIATE_MODE
	m_vecColours.clear();

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	m_vecColours.push_back(colour.r);
	m_vecColours.push_back(colour.g);
	m_vecColours.push_back(colour.b);

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float) + m_vecColours.size() * sizeof(float), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_vecVertices.size() * sizeof(float), &m_vecVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float), m_vecColours.size() * sizeof(float), &m_vecColours[0]);
#else
	if (!m_glslReady) {
		glExtension& extension = glExtension::getInstance();
		bool glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported) {
			m_vsId = createShader(GL_VERTEX_SHADER, vsSource);
			if (!m_vsId) {
				return;
			}

			m_fsId = createShader(GL_FRAGMENT_SHADER, fsSource);
			if (!m_fsId) {
				return;
			}

			m_shaderProgram = createProgram(m_vsId, m_fsId);
			if (!m_shaderProgram) {
				return;
			}

			m_glslReady = true;
		}
	}

	m_nVBOid = createVertexBuffer<float>(m_vecVertices);
	if (m_nVBOid == 0) {
		return;
	}

	m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "in_Position");
	m_inColorLocation = glGetUniformLocation(m_shaderProgram, "inColor");
#endif

	//bRedraw = false;
}
void Xline::paint(Colour3<float> colour) {
	if (bRedraw) {
		create(colour);
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_vecVertices.size() * sizeof(float)));

	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)(3));

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexAttribPointer(m_inPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(m_inPositionLocation);

	if (m_glslReady) {
		glUseProgramObjectARB(m_shaderProgram);
		glUniform4f(m_inColorLocation, colour.r, colour.g, colour.b, 1.0);
	}

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)(3));

	if (m_glslReady) {
		glUseProgram(0);
	}

	glDisableVertexAttribArray(m_inPositionLocation);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}



Circle::Circle(DRW_Circle* entity)
	: Entity(),
	m_lpEntity(entity)
{
	m_iMode = GL_LINE_LOOP;

	double startAngle = 0.0;
	double endAngle = TAU<double>();

	double m_dbMagnification = 1.0;
	unsigned int uiArcSmoothness{ 72 };

	if (m_lpEntity->radious == 0) {
		return;
	}

	int segments = (int)uiArcSmoothness;
	if (uiArcSmoothness < 4) {
		segments = 4;
	}
	if (m_dbMagnification < 1.0) {
		segments = (int)(segments / m_dbMagnification);
	}

	double currentAngle = startAngle;
	double incrementAngle = (TAU<double>() / segments);

	m_vecVertices.clear();
	while (currentAngle < endAngle) {
		currentAngle += incrementAngle;
		if (currentAngle > endAngle) {
			currentAngle = endAngle;
		}

		double dx = m_lpEntity->radious * cos(currentAngle);
		double dy = m_lpEntity->radious * sin(currentAngle);

		double x = m_lpEntity->basePoint.x + dx;
		double y = m_lpEntity->basePoint.y + dy;
		double z = m_lpEntity->basePoint.z;

		m_vecVertices.push_back((float)x);
		m_vecVertices.push_back((float)y);
		m_vecVertices.push_back((float)z);
		m_vecVertices.push_back(1.f);
	}
}

DRW_Entity* Circle::getEntity() { return m_lpEntity; }

Vector2<double>Circle::CalculateCircleCoordinates(const double angle, const double rad)
{
	double x = rad * cos(angle);
	double y = rad * sin(angle);
	return Vector2<double>(x, y);
}

void Circle::create(Colour3<float> colour) {
#ifdef OPENGL_IMMEDIATE_MODE
	m_vecColours.clear();

	for (unsigned i = 0; i < m_vecVertices.size()/4; i++) {
		m_vecColours.push_back(colour.r);
		m_vecColours.push_back(colour.g);
		m_vecColours.push_back(colour.b);
	}

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float) + m_vecColours.size() * sizeof(float), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_vecVertices.size() * sizeof(float), &m_vecVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float), m_vecColours.size() * sizeof(float), &m_vecColours[0]);
#else
	if (!m_glslReady) {
		glExtension& extension = glExtension::getInstance();
		bool glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported) {
			m_vsId = createShader(GL_VERTEX_SHADER, vsSource);
			if (!m_vsId) {
				return;
			}

			m_fsId = createShader(GL_FRAGMENT_SHADER, fsSource);
			if (!m_fsId) {
				return;
			}

			m_shaderProgram = createProgram(m_vsId, m_fsId);
			if (!m_shaderProgram) {
				return;
			}

			m_glslReady = true;
		}
	}

	m_nVBOid = createVertexBuffer<float>(m_vecVertices);
	if (m_nVBOid == 0) {
		return;
	}

	m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "in_Position");
	m_inColorLocation = glGetUniformLocation(m_shaderProgram, "inColor");
#endif

	//bRedraw = false;
}
void Circle::paint(Colour3<float> colour) {
	if (bRedraw) {
		create(colour);
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_vecVertices.size() * sizeof(float)));

	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)m_vecVertices.size() / 4);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexAttribPointer(m_inPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(m_inPositionLocation);

	if (m_glslReady) {
		glUseProgramObjectARB(m_shaderProgram);
		glUniform4f(m_inColorLocation, colour.r, colour.g, colour.b, 1.0);
	}

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)m_vecVertices.size() / 4);

	if (m_glslReady) {
		glUseProgram(0);
	}

	glDisableVertexAttribArray(m_inPositionLocation);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}



Arc::Arc(DRW_Arc* entity)
	: Entity(),
	m_lpEntity(entity)
{
	m_iMode = GL_LINE_STRIP;

	double m_dbMagnification = 1.0;
	unsigned int uiArcSmoothness{ 72 };

	int segments = (int)uiArcSmoothness;
	if (uiArcSmoothness < 4) {
		segments = 4;
	}
	if (m_dbMagnification < 1.0) {
		segments = (int)(segments / m_dbMagnification);
	}

	if (m_lpEntity->isccw == 1) {
		if (m_lpEntity->staangle > m_lpEntity->endangle) {
			m_lpEntity->staangle -= TAU<double>();
		}
	}
	else {
		if (m_lpEntity->staangle < m_lpEntity->endangle) {
			m_lpEntity->staangle += TAU<double>();
		}
	}

	double currentAngle = m_lpEntity->staangle;
	double incrementAngle = (TAU<double>() / segments);

	Vector2<double> coordinate = CalculateCircleCoordinates(currentAngle, m_lpEntity->radious);

	double x = m_lpEntity->basePoint.x + coordinate.x;
	double y = m_lpEntity->basePoint.y + coordinate.y;
	double z = m_lpEntity->basePoint.z;

	m_vecVertices.clear();

	m_vecVertices.push_back((float)x);
	m_vecVertices.push_back((float)y);
	m_vecVertices.push_back((float)z);
	m_vecVertices.push_back(1.f);

	if (m_lpEntity->isccw == 1) {//IsCounterClockWise == true;
		while (currentAngle < m_lpEntity->endangle) {
			currentAngle += incrementAngle;
			if (currentAngle > m_lpEntity->endangle) {
				currentAngle = m_lpEntity->endangle;
			}

			coordinate = CalculateCircleCoordinates(currentAngle, m_lpEntity->radious);

			x = m_lpEntity->basePoint.x + coordinate.x;
			y = m_lpEntity->basePoint.y + coordinate.y;

			m_vecVertices.push_back((float)x);
			m_vecVertices.push_back((float)y);
			m_vecVertices.push_back((float)z);
			m_vecVertices.push_back(1.f);
		}
	}
	else {//IsCounterClockWise != true;
		while (currentAngle > m_lpEntity->endangle) {
			currentAngle -= incrementAngle;
			if (currentAngle < m_lpEntity->endangle) {
				currentAngle = m_lpEntity->endangle;
			}

			coordinate = CalculateCircleCoordinates(currentAngle, m_lpEntity->radious);

			x = m_lpEntity->basePoint.x + coordinate.x;
			y = m_lpEntity->basePoint.y + coordinate.y;

			m_vecVertices.push_back((float)x);
			m_vecVertices.push_back((float)y);
			m_vecVertices.push_back((float)z);
			m_vecVertices.push_back(1.f);
		}
	}
}

DRW_Entity* Arc::getEntity() { return m_lpEntity; }

Vector2<double>Arc::CalculateCircleCoordinates(const double angle, const double rad)
{
	double x = rad * cos(angle);
	double y = rad * sin(angle);
	return Vector2<double>(x, y);
}

void Arc::create(Colour3<float> colour) {
#ifdef OPENGL_IMMEDIATE_MODE
	m_vecColours.clear();

	for (unsigned i = 0; i < m_vecVertices.size()/4; i++) {
		m_vecColours.push_back(colour.r);
		m_vecColours.push_back(colour.g);
		m_vecColours.push_back(colour.b);
	}

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float) + m_vecColours.size() * sizeof(float), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_vecVertices.size() * sizeof(float), &m_vecVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float), m_vecColours.size() * sizeof(float), &m_vecColours[0]);
#else
	if (!m_glslReady) {
		glExtension& extension = glExtension::getInstance();
		bool glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported) {
			m_vsId = createShader(GL_VERTEX_SHADER, vsSource);
			if (!m_vsId) {
				return;
			}

			m_fsId = createShader(GL_FRAGMENT_SHADER, fsSource);
			if (!m_fsId) {
				return;
			}

			m_shaderProgram = createProgram(m_vsId, m_fsId);
			if (!m_shaderProgram) {
				return;
			}

			m_glslReady = true;
		}
	}

	m_nVBOid = createVertexBuffer<float>(m_vecVertices);
	if (m_nVBOid == 0) {
		return;
	}

	m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "in_Position");
	m_inColorLocation = glGetUniformLocation(m_shaderProgram, "inColor");
#endif

	//bRedraw = false;
}
void Arc::paint(Colour3<float> colour) {
	if (bRedraw) {
		create(colour);
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_vecVertices.size() * sizeof(float)));

	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)m_vecVertices.size() / 4);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexAttribPointer(m_inPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(m_inPositionLocation);

	if (m_glslReady) {
		glUseProgramObjectARB(m_shaderProgram);
		glUniform4f(m_inColorLocation, colour.r, colour.g, colour.b, 1.0);
	}

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)(m_vecVertices.size() / 4));

	if (m_glslReady) {
		glUseProgram(0);
	}

	glDisableVertexAttribArray(m_inPositionLocation);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}



EllipsE::EllipsE(DRW_Ellipse* entity)
	: Entity(),
	m_lpEntity(entity)
{
	if ((m_lpEntity->staparam == 0.0) && (m_lpEntity->endparam == PI<double>())) {
		m_iMode = GL_LINE_LOOP;
	}
	else {
		m_iMode = GL_LINE_STRIP;
	}

	Vector4<float> secPoint = { static_cast<float>(m_lpEntity->secPoint.x),static_cast<float>(m_lpEntity->secPoint.y),static_cast<float>(m_lpEntity->secPoint.z), 1.f };
	double majorAxis = secPoint.length();
	double minorAxis = majorAxis * m_lpEntity->ratio;

	double m_dbMagnification = 1.0;
	unsigned int uiArcSmoothness{ 72 };

	int segments = (int)uiArcSmoothness;
	if (uiArcSmoothness < 4) {
		segments = 4;
	}
	if (m_dbMagnification < 1.0) {
		segments = (int)(segments / m_dbMagnification);
	}

	if (m_lpEntity->isccw == 1) {
		if (m_lpEntity->staparam > m_lpEntity->endparam) {
			m_lpEntity->staparam -= TAU<double>();
		}
	}
	else {
		if (m_lpEntity->staparam < m_lpEntity->endparam) {
			m_lpEntity->staparam += TAU<double>();
		}
	}

	double currentAngle = m_lpEntity->staparam;
	double incrementAngle = (TAU<double>() / segments);

	double radius = CalculateEllipseRadius(majorAxis, minorAxis, currentAngle);
	Vector2<double> coordinate = CalculateEllipseCoordinates(currentAngle, radius);

	double x = m_lpEntity->basePoint.x + coordinate.x;
	double y = m_lpEntity->basePoint.y + coordinate.y;
	double z = m_lpEntity->basePoint.z;

	m_vecVertices.clear();

	m_vecVertices.push_back((float)x);
	m_vecVertices.push_back((float)y);
	m_vecVertices.push_back((float)z);
	m_vecVertices.push_back(1.f);

	if (m_lpEntity->isccw == 1) {
		while (currentAngle < m_lpEntity->endparam) {
			currentAngle += incrementAngle;
			if (currentAngle > m_lpEntity->endparam) {
				currentAngle = m_lpEntity->endparam;
			}

			radius = CalculateEllipseRadius(majorAxis, minorAxis, currentAngle);
			coordinate = CalculateEllipseCoordinates(currentAngle, radius);

			x = m_lpEntity->basePoint.x + coordinate.x;
			y = m_lpEntity->basePoint.y + coordinate.y;

			m_vecVertices.push_back((float)x);
			m_vecVertices.push_back((float)y);
			m_vecVertices.push_back((float)z);
			m_vecVertices.push_back(1.f);
		}
	}
	else {
		while (currentAngle > m_lpEntity->endparam) {
			currentAngle -= incrementAngle;
			if (currentAngle < m_lpEntity->endparam) {
				currentAngle = m_lpEntity->endparam;
			}

			radius = CalculateEllipseRadius(majorAxis, minorAxis, currentAngle);
			coordinate = CalculateEllipseCoordinates(currentAngle, radius);

			x = m_lpEntity->basePoint.x + coordinate.x;
			y = m_lpEntity->basePoint.y + coordinate.y;

			m_vecVertices.push_back((float)x);
			m_vecVertices.push_back((float)y);
			m_vecVertices.push_back((float)z);
			m_vecVertices.push_back(1.f);
		}
	}
}

DRW_Entity* EllipsE::getEntity() { return m_lpEntity; }

double EllipsE::CalculateEllipseRadius(const double a, const double b, const double angle) {
	double num = a * b;
	double den = sqrt(pow((a * sin(angle)), 2) + pow((b * cos(angle)), 2));
	return num / den;
}
Vector2<double> EllipsE::CalculateEllipseCoordinates(const double angle, const double radius) {
	double x = radius * cos(angle);
	double y = radius * sin(angle);

	double rot = atan(m_lpEntity->secPoint.y / m_lpEntity->secPoint.x);

	double x_rot = x * cos(rot) - y * sin(rot);
	double y_rot = x * sin(rot) + y * cos(rot);

	return Vector2<double>(x_rot, y_rot);
}

void EllipsE::create(Colour3<float> colour) {
#ifdef OPENGL_IMMEDIATE_MODE
	m_vecColours.clear();

	for (unsigned i = 0; i < m_vecVertices.size()/4; i++) {
		m_vecColours.push_back(colour.r);
		m_vecColours.push_back(colour.g);
		m_vecColours.push_back(colour.b);
	}

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float) + m_vecColours.size() * sizeof(float), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_vecVertices.size() * sizeof(float), &m_vecVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float), m_vecColours.size() * sizeof(float), &m_vecColours[0]);
#else
	if (!m_glslReady) {
		glExtension& extension = glExtension::getInstance();
		bool glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported) {
			m_vsId = createShader(GL_VERTEX_SHADER, vsSource);
			if (!m_vsId) {
				return;
			}

			m_fsId = createShader(GL_FRAGMENT_SHADER, fsSource);
			if (!m_fsId) {
				return;
			}

			m_shaderProgram = createProgram(m_vsId, m_fsId);
			if (!m_shaderProgram) {
				return;
			}

			m_glslReady = true;
		}
	}

	m_nVBOid = createVertexBuffer<float>(m_vecVertices);
	if (m_nVBOid == 0) {
		return;
	}

	m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "in_Position");
	m_inColorLocation = glGetUniformLocation(m_shaderProgram, "inColor");
#endif

	//bRedraw = false;
}
void EllipsE::paint(Colour3<float> colour) {
	if (bRedraw) {
		create(colour);
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_vecVertices.size() * sizeof(float)));

	glDrawArrays(m_iMode, 0, (GLsizei)m_vecVertices.size() / 4);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexAttribPointer(m_inPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(m_inPositionLocation);

	if (m_glslReady) {
		glUseProgramObjectARB(m_shaderProgram);
		glUniform4f(m_inColorLocation, colour.r, colour.g, colour.b, 1.0);
	}

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glDrawArrays(m_iMode, 0, (GLsizei)m_vecVertices.size() / 4);

	if (m_glslReady) {
		glUseProgram(0);
	}

	glDisableVertexAttribArray(m_inPositionLocation);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}



LWPolyline::LWPolyline(DRW_LWPolyline* entity)
	: Entity(),
	m_lpEntity(entity)
{
	if (m_lpEntity->flags == 1) {
		m_iMode = GL_LINE_LOOP;
	}
	else {
		m_iMode = GL_LINE_STRIP;
	}

	for (unsigned i = 0; i < m_lpEntity->vertlist.size(); i++) {
		m_vecVertices.push_back((float)(m_lpEntity->vertlist[i]->x));
		m_vecVertices.push_back((float)(m_lpEntity->vertlist[i]->y));
		m_vecVertices.push_back(0.f);
		m_vecVertices.push_back(1.f);
	}
}

DRW_Entity* LWPolyline::getEntity() { return m_lpEntity; }

void LWPolyline::create(Colour3<float> colour) {
#ifdef OPENGL_IMMEDIATE_MODE
	m_vecColours.clear();

	for (unsigned i = 0; i < m_lpEntity->vertlist.size(); i++) {
		m_vecColours.push_back(colour.r);
		m_vecColours.push_back(colour.g);
		m_vecColours.push_back(colour.b);
	}

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float) + m_vecColours.size() * sizeof(float), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_vecVertices.size() * sizeof(float), &m_vecVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float), m_vecColours.size() * sizeof(float), &m_vecColours[0]);
#else
	if (!m_glslReady) {
		glExtension& extension = glExtension::getInstance();
		bool glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported) {
			m_vsId = createShader(GL_VERTEX_SHADER, vsSource);
			if (!m_vsId) {
				return;
			}

			m_fsId = createShader(GL_FRAGMENT_SHADER, fsSource);
			if (!m_fsId) {
				return;
			}

			m_shaderProgram = createProgram(m_vsId, m_fsId);
			if (!m_shaderProgram) {
				return;
			}

			m_glslReady = true;
		}
	}

	m_nVBOid = createVertexBuffer<float>(m_vecVertices);
	if (m_nVBOid == 0) {
		return;
	}

	m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "in_Position");
	m_inColorLocation = glGetUniformLocation(m_shaderProgram, "inColor");
#endif

	//bRedraw = false;
}
void LWPolyline::paint(Colour3<float> colour) {
	if (m_lpEntity->vertlist.size() == 0) {
		return;
	}

	if (bRedraw) {
		create(colour);
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_vecVertices.size() * sizeof(float)));

	glDrawArrays(m_iMode, 0, (GLsizei)m_lpEntity->vertlist.size());

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexAttribPointer(m_inPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(m_inPositionLocation);

	if (m_glslReady) {
		glUseProgramObjectARB(m_shaderProgram);
		glUniform4f(m_inColorLocation, colour.r, colour.g, colour.b, 1.0);
	}

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glDrawArrays(m_iMode, 0, (GLsizei)m_vecVertices.size() / 4);

	if (m_glslReady) {
		glUseProgram(0);
	}

	glDisableVertexAttribArray(m_inPositionLocation);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}



Polyline::Polyline(DRW_Polyline* entity)
	: Entity(),
	m_lpEntity(entity)
{
	if (m_lpEntity->flags == 1) {
		m_iMode = GL_LINE_LOOP;
	}
	else {
		m_iMode = GL_LINE_STRIP;
	}

	for (unsigned i = 0; i < m_lpEntity->vertlist.size(); i++) {
		m_vecVertices.push_back((float)(m_lpEntity->vertlist[i]->basePoint.x));
		m_vecVertices.push_back((float)(m_lpEntity->vertlist[i]->basePoint.y));
		m_vecVertices.push_back((float)(m_lpEntity->vertlist[i]->basePoint.z));
		m_vecVertices.push_back(1.f);
	}
}

DRW_Entity* Polyline::getEntity() { return m_lpEntity; }

void Polyline::create(Colour3<float> colour) {
#ifdef OPENGL_IMMEDIATE_MODE
	m_vecColours.clear();

	for (unsigned i = 0; i < m_lpEntity->vertlist.size(); i++) {
		m_vecColours.push_back(colour.r);
		m_vecColours.push_back(colour.g);
		m_vecColours.push_back(colour.b);
	}

	glGenBuffersARB(1, &m_nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float) + m_vecColours.size() * sizeof(float), 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, m_vecVertices.size() * sizeof(float), &m_vecVertices[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, m_vecVertices.size() * sizeof(float), m_vecColours.size() * sizeof(float), &m_vecColours[0]);
#else
	if (!m_glslReady) {
		glExtension& extension = glExtension::getInstance();
		bool glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported) {
			m_vsId = createShader(GL_VERTEX_SHADER, vsSource);
			if (!m_vsId) {
				return;
			}

			m_fsId = createShader(GL_FRAGMENT_SHADER, fsSource);
			if (!m_fsId) {
				return;
			}

			m_shaderProgram = createProgram(m_vsId, m_fsId);
			if (!m_shaderProgram) {
				return;
			}

			m_glslReady = true;
		}
	}

	m_nVBOid = createVertexBuffer<float>(m_vecVertices);
	if (m_nVBOid == 0) {
		return;
	}

	m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "in_Position");
	m_inColorLocation = glGetUniformLocation(m_shaderProgram, "inColor");
#endif

	//bRedraw = false;
}
void Polyline::paint(Colour3<float> colour) {
	if (m_lpEntity->vertlist.size() == 0) {
		return;
	}

	if (bRedraw) {
		create(colour);
	}

#ifdef OPENGL_IMMEDIATE_MODE
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(m_vecVertices.size() * sizeof(float)));

	glDrawArrays(m_iMode, 0, (GLsizei)m_lpEntity->vertlist.size());

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#else
	glBindBufferARB(GL_ARRAY_BUFFER, m_nVBOid);

	glVertexAttribPointer(m_inPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(m_inPositionLocation);

	if (m_glslReady) {
		glUseProgramObjectARB(m_shaderProgram);
		glUniform4f(m_inColorLocation, colour.r, colour.g, colour.b, 1.0);
	}

	glVertexPointer(4, GL_FLOAT, 0, (GLvoid*)(0));
	glDrawArrays(m_iMode, 0, (GLsizei)m_vecVertices.size() / 4);

	if (m_glslReady) {
		glUseProgram(0);
	}

	glDisableVertexAttribArray(m_inPositionLocation);

	glBindBufferARB(GL_ARRAY_BUFFER, 0);
#endif
}
