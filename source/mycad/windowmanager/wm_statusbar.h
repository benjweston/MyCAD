#pragma once

#include "wm_window.h"

#include "wm_tooltip.h"

#include <commctrl.h>
#include <vector>
#include <array>

namespace mycad {
	/**
	* Declaration of STATUSBARPANELINFO typedef struct.
	*/
	typedef struct tagSTATUSBARPANELINFO {
		tagSTATUSBARPANELINFO() : uWidth(24), nVisible(TRUE), lpTooltip(nullptr), hwndProgress(0) {}
		tagSTATUSBARPANELINFO(UINT width) : uWidth(width), nVisible(TRUE), lpTooltip(nullptr), hwndProgress(0) {}
		UINT uWidth;
		BOOL nVisible;
		Tooltip* lpTooltip;
		HWND hwndProgress;
	} STATUSBARPANELINFO, *LPSTATUSBARPANELINFO;

	/**
	* Declaration of Tooltip class.
	*/
	class Statusbar : public Window {
	public:
		/**
		* Constructor.
		* Creates a new window.
		* To check if the window was created properly, use the #getValid() method.
		*/
		Statusbar(DWORD dwStyle,
			HWND hwndParent,
			UINT uId,
			HINSTANCE hInstance);

		/**
		* Destructor.
		* Closes the window and disposes resources allocated.
		*/
		virtual ~Statusbar();

		BOOL setText(UINT uIndex, const char* szText, UINT uFlags = SBT_POPOUT);

		BOOL setTooltip(UINT uIndex, Tooltip tooltip);
		BOOL setTooltip(UINT uIndex, DWORD dwStyle, UINT uFlags, UINT_PTR uId);
		BOOL setTooltip(UINT uIndex, DWORD dwStyle, UINT uFlags, UINT_PTR uId, const char* lpszText);
		BOOL setTooltip(UINT uIndex, DWORD dwStyle, UINT uFlags, UINT_PTR uId, const char* lpszTitle, const char* lpszText);
		BOOL setTooltipText(UINT uIndex, const char* szText);
		BOOL setTooltipTitle(UINT uIndex, const char* szText);

		template<std::size_t SIZE>
		void setParts(std::array<int, SIZE>& panels)
		{
			if (m_nPanels.size() > 0) {
				for (int i = 0; i < m_nPanels.size(); i++) {
					if (m_nPanels[i]->hwndProgress != NULL) {
						DestroyWindow(m_nPanels[i]->hwndProgress);
					}
					m_nPanels[i]->hwndProgress = NULL;
					if (m_nPanels[i]->lpTooltip != NULL) {
						DestroyWindow(m_nPanels[i]->lpTooltip->getHWND());
					}
					//m_nPanels[i]->lpTooltip->setHWND(NULL);
					delete m_nPanels[i]->lpTooltip;
					m_nPanels[i]->nVisible = TRUE;
					m_nPanels[i]->uWidth = 24;
					m_nPanelPositions[i] = 0;
				}
			}
			m_nPanels.clear();
			m_nPanelPositions.clear();
			for (int i = 0; i < SIZE; i++) {
				m_nPanels.push_back(new STATUSBARPANELINFO(panels[i]));
				m_nPanelPositions.push_back(0);
			}
		}
		const int getParts();
		int getParts(int number, int nParts[]);

		BOOL setPanelVisibility(UINT uIndex, BOOL nVisibility);
		BOOL getPanelVisibility(UINT uIndex);

		HRESULT showProgressbar(UINT uIndex);
		int showProgressbar(UINT uIndex, int nPosition);
		void hideProgressbar(UINT uIndex);

		//LPRECT getClientRect();

		int wm_exitsizemove() override;
		int wm_size();

	private:
		RECT m_rcStatusbar;
		std::vector<STATUSBARPANELINFO*>m_nPanels;
		std::vector<int>m_nPanelPositions;

	};
}
