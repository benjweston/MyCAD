#pragma once

#include "DOC_document.h"

namespace mycad {
	/**
	Declaration of SDIDocument class.
	*/
	class SDIDocument : public Document {
	public:
		SDIDocument(DynamicInputWindow* commandwindows[], const char* filename, int tabindex = 0);
		~SDIDocument();

	private:
		

	};
}
