#pragma once

#include "LIB_objectmanager.h"

namespace mycad {
	/**
	* Declaration of DocumentManager class.
	*/
	class Document;
	class DocumentManager : public ObjectManager<Document> {
	public:
		DocumentManager();
		~DocumentManager();

		LRESULT sendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:


	};
}
