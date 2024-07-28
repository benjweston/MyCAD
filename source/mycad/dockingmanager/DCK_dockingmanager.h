#pragma once

#include "LIB_objectmanager.h"

namespace mycad {
	/**
	Declaration of DockingManager class.
	*/
	class Pane;
	class DockingManager : public ObjectManager<Pane> {
	public:
		DockingManager();
		~DockingManager();


	private:


	};
}
