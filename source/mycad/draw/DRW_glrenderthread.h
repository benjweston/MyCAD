#pragma once

#include "CMD_commands.h"
#include "CMD_commandmanager.h"
#include "DRW_layer.h"
#include "DRW_layermanager.h"
#include "DRW_layout.h"
#include "DRW_layoutmanager.h"
#include "DRW_view.h"
#include "DRW_viewmanager.h"

#include "DRW_glrendercontext.h"

#include <atomic>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <thread>

namespace mycad {
	struct MessageParameters {
		MessageParameters() : wParam(0), lParam(0) {}
		MessageParameters(WPARAM wParam, LPARAM lParam) : wParam(wParam), lParam(lParam) {}
		WPARAM wParam;
		LPARAM lParam;
	};

	struct ThreadMessage {
		ThreadMessage(UINT i) { uMsg = i; lpParams = nullptr; }
		ThreadMessage(UINT i, std::shared_ptr<void> m) { uMsg = i; lpParams = m; }
		UINT uMsg;
		std::shared_ptr<void> lpParams;
	};

	/**
	* Declaration of GLRenderThread class.
	*/
	class GLRenderThread {
	public:
		GLRenderThread();
		GLRenderThread(CommandManager* commandmanager);
		~GLRenderThread();

		void setSwapBuffers();

		CommandManager* getCommandManager();
		DataBlock* Entities();
		LayerManager* getLayerManager();
		LayoutManager* getLayoutManager();
		ViewManager* getViewManager();

		Vector3<double>getCoordinates();

		void SendThreadMessage(UINT uMsg);
		void SendThreadMessage(UINT uMsg, WPARAM wParam);
		void SendThreadMessage(UINT uMsg, LPARAM lParam);
		void SendThreadMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void renderThreadProc(GLRenderContext* rendercontext, SIZE size);
		void swapbuffersThreadProc();
		void calculateFPSThreadProc();;

	private:
		enum { MAX_LOADSTRING = 256 };

		HWND m_hWnd;
		DWORD dwThreadId;

		int m_cx;
		int m_cy;

		std::mutex m_CommandManagerMutex;
		std::mutex m_LayoutManagerMutex;

		std::mutex m_LayerManagerMutex;
		std::mutex m_ViewManagerMutex;
		std::mutex m_DataBlocksMutex;

		CommandManager* m_lpCommandManager;
		LayoutManager* m_lpLayoutManager;//std::shared_ptr<LayoutManager*> m_lpLayoutManager;

		LayerManager m_LayerManager;
		ViewManager m_ViewManager;
		DataBlocks m_DataBlocks;

		GLRenderContext* m_lpGLRenderContext;

		std::atomic<int> m_nFPS;
		std::atomic<bool> m_bRenderLoop;
		std::atomic<bool> m_bSwapBuffers;
		std::atomic<bool> m_bMouseInRect;
		std::atomic<bool> m_bMouseMoved;

		std::queue<std::shared_ptr<ThreadMessage>> m_renderQueue;
		std::mutex m_renderMutex;
		std::condition_variable m_renderCV;
		std::mutex m_messageMutex;

		bool m_bVBOSupported;
		bool m_bGLSLSupported;

		DWORD m_dwKeystate;
		POINT m_ptMouseDown;
		POINT m_ptMouseUp;
		POINT m_ptMouseMove;

		POINT m_ptCursor;

		POINT WinToGL(POINT point);

		Vector2<double> getKeyBoardInput(UINT keycode);

		void SendThreadMessage(UINT uMsg, std::shared_ptr<MessageParameters> mp);

		void tm_hScroll(WPARAM wParam, LPARAM lParam);
		void tm_keyDown(WPARAM wParam, LPARAM lParam);
		void tm_lButtonDblClk(WPARAM wParam, LPARAM lParam);
		void tm_lButtonDown(WPARAM wParam, LPARAM lParam);
		void tm_lButtonUp(WPARAM wParam, LPARAM lParam);
		void tm_mouseLeave();
		void tm_mouseMove(WPARAM wParam, LPARAM lParam);
		void tm_mouseWheel(WPARAM wParam, LPARAM lParam);
		void tm_notify(WPARAM wParam, LPARAM lParam);
		void tm_paint();
		void tm_size(WPARAM wParam, LPARAM lParam);
		void tm_timer(WPARAM wParam, LPARAM lParam);
		void tm_vScroll(WPARAM wParam, LPARAM lParam);

		void getSupportedExtensions();

		bool initVertexBuffers();
		bool initShaders();
		void initLights();
		void initGL();

	};
}
