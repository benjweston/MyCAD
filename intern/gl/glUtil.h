#pragma once

//#ifdef _MSC_VER
//#define _CRT_SECURE_NO_WARNINGS
//#endif

#include "glExtension.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

//HRESULT getHResultFromOpenGL(unsigned int err);//Maps glGetError error codes to HRESULT error codes
//void getGLErrors();

unsigned int createShader(int shadertype, const char* string);
unsigned int createProgram(unsigned int& vertexshader, unsigned int& fragmentshader);
void deleteProgram(unsigned int shader, unsigned int vertexshader, unsigned int fragmentshader);

/*template<typename T>
unsigned int createVertexBuffer(T vertices[], int size)
{
	//float sum = 0.f;
	//for (int i = 0; i < size; ++i) {
	//	sum = vertices[i];
	//}

	unsigned int nVBOid;

	glExtension& extension = glExtension::getInstance();
	bool vboSupported = extension.isSupported("GL_ARB_vertex_buffer_object");
	if (!vboSupported) {
		return FALSE;
	}

	glGenBuffersARB(1, &nVBOid);
	glBindBufferARB(GL_ARRAY_BUFFER, nVBOid);
	glBufferDataARB(GL_ARRAY_BUFFER, size * sizeof(T), &vertices, GL_STATIC_DRAW);

	int bsize = 0;
	glGetBufferParameterivARB(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
	if (bsize == 0) {
		return FALSE;
	}

	return nVBOid;
}*/

template<typename T, unsigned int SZ>
unsigned int createVertexBuffer(T(&vertices)[SZ], unsigned int index, int size, int type);

template<typename T, unsigned int SZ>
unsigned int createVertexBuffer(T(&vertices)[SZ]);

template<typename T>
unsigned int createVertexBuffer(std::vector<T> vertices, unsigned int index, int size, int type);

template<typename T>
unsigned int createVertexBuffer(std::vector<T> vertices);

long glErrorHandler();
void glErrorHandler(long glerr);

#include "glUtil.tpp"
