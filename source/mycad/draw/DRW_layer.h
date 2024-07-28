#pragma once

#include "LIB_matrices.h"

#include <windows.h>

namespace mycad {
	/**
	* Declaration of Layer class.
	*/
	class Layer {
	public:
		Layer();
		Layer(const char *name);
		~Layer();


	private:
		enum { MAX_LOADSTRING = 256 };

		char m_szName[MAX_LOADSTRING];

	};
}
