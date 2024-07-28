#pragma once

#include <string>

#include <windows.h>

namespace MyCAD {
	class Shader {
	public:
		Shader();
		Shader(const char* vertexPath, const char* fragmentPath);
		~Shader();

		unsigned int getID();

        void use();

        void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;

		void setFloatv(const std::string& name, float* value) const;

	private:
		unsigned int m_ID;

		void checkCompileErrors(unsigned int shader, std::string type);

	};
}
