#pragma once

#include "glExtension.h"

template<typename T, unsigned int SZ>
unsigned int createVertexBuffer(T(&vertices)[SZ], unsigned int index, int size, int type)
{
	unsigned int nVBOid;

	glExtension& extension = glExtension::getInstance();
	bool vboSupported = extension.isSupported("GL_ARB_vertex_buffer_object");
	if (!vboSupported) {
		return FALSE;
	}

	glGenBuffersARB(1, &nVBOid);

	glBindBufferARB(GL_ARRAY_BUFFER, nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	int bsize = 0;
	glGetBufferParameterivARB(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
	if (bsize == 0) {
		return FALSE;
	}

	glVertexAttribPointer(index, size, type, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(index);

	return nVBOid;
}

template<typename T, unsigned int SZ>
unsigned int createVertexBuffer(T(&vertices)[SZ])
{
	unsigned int nVBOid;

	glExtension& extension = glExtension::getInstance();
	bool vboSupported = extension.isSupported("GL_ARB_vertex_buffer_object");
	if (!vboSupported) {
		return FALSE;
	}

	glGenBuffersARB(1, &nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	int bsize = 0;
	glGetBufferParameterivARB(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
	if (bsize == 0) {
		return FALSE;
	}

	return nVBOid;
}

template<typename T>
unsigned int createVertexBuffer(std::vector<T> vertices, unsigned int index, int size, int type)
{
	unsigned int nVBOid = 0;

	glExtension& extension = glExtension::getInstance();
	bool vboSupported = extension.isSupported("GL_ARB_vertex_buffer_object");
	if (!vboSupported) {
		return FALSE;
	}

	glGenBuffersARB(1, &nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), &vertices[0], GL_STATIC_DRAW);

	int bsize = 0;
	glGetBufferParameterivARB(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
	if (bsize == 0) {
		return FALSE;
	}

	glVertexAttribPointer(index, size, type, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(index);

	return nVBOid;
}

template<typename T>
unsigned int createVertexBuffer(std::vector<T> vertices)
{
	unsigned int nVBOid;

	glExtension& extension = glExtension::getInstance();
	bool vboSupported = extension.isSupported("GL_ARB_vertex_buffer_object");
	if (!vboSupported) {
		return FALSE;
	}

	glGenBuffersARB(1, &nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), &vertices[0], GL_STATIC_DRAW);

	int bsize = 0;
	glGetBufferParameterivARB(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
	if (bsize == 0) {
		return FALSE;
	}

	return nVBOid;
}
