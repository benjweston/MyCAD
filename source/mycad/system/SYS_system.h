#pragma once

#include "wm_window.h"

namespace mycad {
	/**
	Declaration of System class.
	*/
	class System {

		enum class ApplicationInterfaceType : int {
			SingleDocuments = 0,
			MultipleDocuments,
			DialogBased,
			MultpleTopLevelDocuments
		};

	public:
		static System* createSystem(HINSTANCE hInstance);
		static int disposeSystem();
		static System* getSystem();

		virtual int processEvents() = 0;

	protected:

		System();//Private constructor to prevent more than one instantiation of class.
		System(HINSTANCE hInstance);//Private constructor to prevent more than one instantiation of class.
		virtual ~System();

		static ApplicationInterfaceType s_enApplicationInterfaceType;

		static System* s_system;

		virtual BOOL createApplication() = 0;

	};
}
