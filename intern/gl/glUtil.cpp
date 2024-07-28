#include "glUtil.h"

#include "log.h"

/*HRESULT getHResultFromOpenGL(unsigned int err) {
	switch (err) {
		case GL_INVALID_ENUM: {
			return 0x80070057;//E_INVALIDARG
		}
		case GL_INVALID_VALUE: {
			return 0x80070057;//E_INVALIDARG
		}
		case GL_INVALID_OPERATION: {
			return 0x80004005;//E_FAIL
		}
		case GL_STACK_OVERFLOW: {
			return 0x8007000E;//E_OUTOFMEMORY
		}
		case GL_STACK_UNDERFLOW: {
			return 0x8007000E;//E_OUTOFMEMORY
		}
		case GL_OUT_OF_MEMORY: {
			return 0x8007000E;//E_OUTOFMEMORY
		}
		case GL_NO_ERROR: {
			return 0x00000000;//S_OK
		}
		default: {
			return 0x00000000;//S_OK
		}
	}
}*/
/*void getGLErrors() {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		switch (err) {
			case GL_INVALID_ENUM: {
				log("glGetError %ld GL_INVALID_ENUM", err);
				break;
			}
			case GL_INVALID_VALUE: {
				log("glGetError %ld GL_INVALID_VALUE", err);
				break;
			}
			case GL_INVALID_OPERATION: {
				log("glGetError %ld GL_INVALID_OPERATION", err);
				break;
			}
			case GL_STACK_OVERFLOW: {
				log("glGetError %ld GL_STACK_OVERFLOW", err);
				break;
			}
			case GL_STACK_UNDERFLOW: {
				log("glGetError %ld GL_STACK_UNDERFLOW", err);
				break;
			}
			case GL_OUT_OF_MEMORY: {
				log("glGetError %ld GL_OUT_OF_MEMORY", err);
				break;
			}
			case GL_NO_ERROR: {
				log("glGetError %ld GL_NO_ERROR", err);
				break;
			}
			default: {
				log("glGetError %ld GL_NO_ERROR", err);
			}
		}
	}
}*/

unsigned int createShader(int shadertype, const char* string)
{
	GLhandleARB handle = glCreateShader(shadertype);
	glShaderSource(handle, 1, (const GLchar**)&string, 0);
	glCompileShader(handle);

	int compiledStatus;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compiledStatus);
	if (compiledStatus == FALSE)
	{
		int maxLength;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = (char*)malloc(maxLength);
		glGetShaderInfoLog(handle, maxLength, &maxLength, infoLog);

		free(infoLog);

		return FALSE;
	}

	return handle;
}
unsigned int createProgram(unsigned int& vertexshader, unsigned int& fragmentshader)
{
	unsigned int program = glCreateProgram();

	glAttachShader(program, vertexshader);
	glAttachShader(program, fragmentshader);

	glBindAttribLocation(program, 0, "in_Position");
	glBindAttribLocation(program, 1, "in_Color");

	glLinkProgram(program);

	int linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&linkStatus);
	if (linkStatus == FALSE)
	{
		int maxLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = (char*)malloc(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

		free(infoLog);

		return FALSE;
	}

	return program;
}
void deleteProgram(unsigned int shader, unsigned int vertexshader, unsigned int fragmentshader)
{
	glDetachShader(shader, vertexshader);
	glDetachShader(shader, fragmentshader);

	glDeleteProgram(shader);

	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);
}

long glErrorHandler()//Uses glGetError() to get the current gl errors, displays the error codes and returns the last value for further use.
{
	DWORD dwErrorCode;
	while ((dwErrorCode = glGetError()) != GL_NO_ERROR) {
		switch (dwErrorCode) {
			case GL_INVALID_ENUM: {
				log("glErrorHandler() glError Code: %d (GL_INVALID_ENUM)", dwErrorCode);
				break;
			}
			case GL_INVALID_VALUE: {
				log("glErrorHandler() glError Code: %d (GL_INVALID_VALUE)", dwErrorCode);
				break;
			}
			case GL_INVALID_OPERATION: {
				log("glErrorHandler() glError Code: %d (GL_INVALID_OPERATION)", dwErrorCode);
				break;
			}
			case GL_STACK_OVERFLOW: {
				log("glErrorHandler() glError Code: %d (GL_STACK_OVERFLOW)", dwErrorCode);
				break;
			}
			case GL_STACK_UNDERFLOW: {
				log("glErrorHandler() glError Code: %d (GL_STACK_UNDERFLOW)", dwErrorCode);
				break;
			}
			case GL_OUT_OF_MEMORY: {
				log("glErrorHandler() glError Code: %d (GL_OUT_OF_MEMORY)", dwErrorCode);
				break;
			}
			case GL_NO_ERROR: {
				log("glErrorHandler() glError Code: %d (GL_NO_ERROR)", dwErrorCode);
				break;
			}
			default: {
				log("glErrorHandler() glError Code: %d (GL_NO_ERROR)", dwErrorCode);
			}
		}
	}
	return (long)dwErrorCode;
}
void glErrorHandler(long glerr)//Receives the value of the glerr and displays the error code.
{
	switch (glerr) {
		case GL_INVALID_ENUM: {
			log("glErrorHandler() glError Code: %d (GL_INVALID_ENUM)", glerr);
			break;
		}
		case GL_INVALID_VALUE: {
			log("glErrorHandler() glError Code: %d (GL_INVALID_VALUE)", glerr);
			break;
		}
		case GL_INVALID_OPERATION: {
			log("glErrorHandler() glError Code: %d (GL_INVALID_OPERATION)", glerr);
			break;
		}
		case GL_STACK_OVERFLOW: {
			log("glErrorHandler() glError Code: %d (GL_STACK_OVERFLOW)", glerr);
			break;
		}
		case GL_STACK_UNDERFLOW: {
			log("glErrorHandler() glError Code: %d (GL_STACK_UNDERFLOW)", glerr);
			break;
		}
		case GL_OUT_OF_MEMORY: {
			log("glErrorHandler() glError Code: %d (GL_OUT_OF_MEMORY)", glerr);
			break;
		}
		case GL_NO_ERROR: {
			log("glErrorHandler() glError Code: %d (GL_NO_ERROR)", glerr);
			break;
		}
		default: {
			log("glErrorHandler() glError Code: %d (GL_NO_ERROR)", glerr);
		}
	}
}
