#pragma once

#include "MKE_application.h"

namespace mycad {
	/**
	Declaration of SDIApplication class.
	*/
	class MDIApplication : public Application {
	public:
		/**
		Constructor.
		Creates a new MDIApplication window.
		\param windowmanager: pointer to a WindowManager instance.
		\param documentmanager: pointer to a DocumentManager instance.
		*/
		MDIApplication(WindowManager* windowmanager, DocumentManager* documentmanager);
		/**
		Destructor.
		Closes the window and disposes resources allocated.
		*/
		~MDIApplication();

		static HWND s_hMDIClient;

		int wm_create(WPARAM wParam, LPARAM lParam) override;

	private:
		void createDocument(const char* filename) override;

		void wm_command_Window_Close();
		void wm_command_Window_CloseAll();
		void wm_command_Window_Cascade();
		void wm_command_Window_TileHorizontally();
		void wm_command_Window_TileVertically();
		void wm_command_Window_ArrangeIcons();

		virtual int wm_command_Default(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void wm_create_mdiclient(CREATESTRUCT* lpcs);

		void wm_notify_tcn_selchange_documents() override;

		int wm_notify_tcn_selchanging_documents() override;

		void wm_size() override;

	};
}