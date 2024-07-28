#pragma once

#include "MKE_application.h"

namespace mycad {
	/**
	Declaration of SDIApplication class.
	*/
	class SDIApplication : public Application {
	public:
		/**
		Constructor.
		Creates a new SDIApplication window.
		\param windowmanager: pointer to a WindowManager instance.
		\param documentmanager: pointer to a DocumentManager instance.
		*/
		SDIApplication(WindowManager* windowmanager, DocumentManager* documentmanager);
		/**
		Destructor.
		Closes the window and disposes resources allocated.
		*/
		~SDIApplication();

		int wm_create(WPARAM wParam, LPARAM lParam) override;

	private:
		void createDocument(const char* filename) override;

	};
}