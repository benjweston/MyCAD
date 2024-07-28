#pragma once

#include "wm_windowwin32.h"

#include "CMD_commandmanager.h"

#include "DRW_layer.h"
#include "DRW_layermanager.h"
#include "DRW_layout.h"
#include "DRW_layoutmanager.h"

#include "io.h"

#include <thread>

namespace mycad {
	/**
	Declaration of Document class.
	*/
	class DynamicInputWindow;
	class CommandManager;
	class GLRenderContext;
	class GLRenderThread;
	class ViewManager;
	class GLRenderWindow : public WindowWin32 {
	public:
		GLRenderWindow(DynamicInputWindow* commandwindows[], const char* filename);
		~GLRenderWindow();

		enum class Context : int {
			ModelSpace2D,
			Sheet_Layout,
			ParallelProjection3D,
			PerspectiveProjection3D,
			BlockEditor,
			CommandLine,
			PlotPreview
		};

		enum class InterfaceElement : int {
			UniformBackground,
			PaperBackground,
			TopOfSkyBackground,
			BackgroundSkyHorizon,
			BackgroundGroundOrigin,
			BackgroundGroundHorizon,
			BottomOfEarthBackground,
			BackgroundEarthHorizon,
			Crosshairs,
			ViewportControls
		};

		enum class SystemInterfaceElement : int {
			ActiveBorder,
			ActiveCaption
		};

		static bool s_bScrollbars;
		static bool s_bContinuousSize;

		static void setSplitterThickness(long thickness);

		static void setColour(Context context, InterfaceElement interfaceelement, COLORREF colour, BOOL tintforxyz = 0);
		static COLORREF getColour(Context context, InterfaceElement interfaceelement);

		static void setSystemColour(SystemInterfaceElement systeminterfaceelement, COLORREF colour);
		static COLORREF getSystemColour(SystemInterfaceElement systeminterfaceelement);

		void setFileName(const char* filename);
		char* getFileName();

		Vector3<double> getCoordinates();

		LayerManager *getLayerManager();
		LayoutManager* getLayoutManager();
		Layout* getActiveLayout();
		ViewManager* getViewManager();
		ViewportManager *getViewports();
		Command* getActiveCommand();
		CommandManager* getCommandManager();

		BOOL setStatusbarText(HWND hstatusbar, int index = -1);
		BOOL setDynamicInputStyle();

		int resumeRenderThread();
		int suspendRenderThread();

		//Public window functions.
		int wm_activate(WPARAM wParam, LPARAM lParam) override;
		int wm_char(WPARAM wParam, LPARAM lParam);
		int wm_close() override;
		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;
		int wm_create(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_entermenuloop(WPARAM wParam) override;
		int wm_exitmenuloop(WPARAM wParam) override;
		int wm_exitsizemove() override;
		int wm_hscroll(WPARAM wParam, LPARAM lParam) override;
		int wm_keydown(WPARAM wParam, LPARAM lParam) override;
		int wm_killfocus(WPARAM wParam) override;
		int wm_lbuttondblclk(WPARAM wParam, LPARAM lParam) override;
		int wm_lbuttondown(WPARAM wParam, LPARAM lParam) override;
		int wm_lbuttonup(WPARAM wParam, LPARAM lParam) override;
		int wm_mouseleave() override;
		int wm_mousemove(WPARAM wParam, LPARAM lParam) override;
		int wm_mousewheel(WPARAM wParam, LPARAM lParam) override;
		int wm_move(LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_paint() override;
		int wm_setcursor(WPARAM wParam, LPARAM lParam) override;
		int wm_setfocus(WPARAM wParam) override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;
		int wm_timer(WPARAM wParam, LPARAM lParam) override;
		int wm_vscroll(WPARAM wParam, LPARAM lParam) override;
		int wm_windowposchanged(LPARAM lParam);
		int wm_windowposchanging(LPARAM lParam);

	private://Private overrides of public window functions.
		void wm_size();


	private:
		DynamicInputWindow* m_lpCommandWindows[3];//Command windows 0, 1 & 2 are TextWindow, CommandLine & DynamicInput.
		char m_szFileName[MAX_LOADSTRING];
		DWORD dwThreadId;

		GLRenderContext* m_lpGLRenderContext;
		GLRenderThread* m_lpGLRenderThread;

		CommandManager m_CommandManager;

		DRW_Data m_DRW_Data;

		std::thread* m_lpRenderThread;
		std::thread* m_lpFrameThread;
		std::thread* m_lpFPSThread;
		std::thread* m_lpDataThread;

		int m_nHScroll;
		int m_nVScroll;

		SCROLLINFO m_siHorz;
		SCROLLINFO m_siVert;

		MouseTrackEvents m_mouseTrack;

		POINT m_ptMouseDown;
		POINT m_ptMouseUp;
		POINT m_ptMouseMove;
		bool m_bMouseMoved;

		bool m_bMenuLoopActive;
		bool m_bShortcutMenuLoopActive;
		bool m_bLbuttonDblclk;

		int m_nCharWidth;
		size_t m_cch;
		int m_nCurChar;
		char* m_pchInputBuf;
		char m_ch;

		static void setModelSpace2D(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz = false);
		static void setLayout(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz = false);
		static void setParallelProjection3D(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz = false);
		static void setPerspectiveProjection3D(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz = false);
		static void setBlockEditor(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz = false);
		static void setCommandLine(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz = false);
		static void setPlotPreview(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz = false);

		BOOL showDynamicinput(WPARAM wParam, LPARAM lParam);
		BOOL showCommandline(WPARAM wParam, LPARAM lParam);

		void readEntity(DRW_Entity* ent);
		void readBlock(DRW_DataBlock* block);
		void readDimstyle(DRW_Dimstyle* dimstyle);
		void readLayer(DRW_Layer* ler);
		void readLinetype(DRW_LType* linetype);
		void readTextstyle(DRW_Textstyle* textstyle);
		void readVport(DRW_Vport* vpt);
		void readAppId(DRW_AppId appid);
		void readImage(DRW_DataImg* dataimg);

		void readHeader();
		void readEntities();
		void readBlocks();
		void readDimstyles();
		void readLayers();
		void readLinetypes();
		void readTextstyles();
		void readVports();
		void readAppIds();
		void readImages();

		void readData();
		bool fileOpen(const char* fileName);

		POINT WinToGL(POINT pt);

		void clearCommandTextBuffer();

		int createCommandWindow(Command* lpCommand, CommandContext nContext = CommandContext::COMMAND);
		int createCommandDialog(Command* lpCommand, UINT nDialogId);

		void exitCommand();

		BOOL wm_char_view(WORD code);
		void wm_char_back();
		void wm_char_return();
		void wm_char_escape();
		void wm_char_end();
		void wm_char_left();
		void wm_char_right();
		void wm_char_default(WPARAM wParam, LPARAM lParam);

		void wm_command_sendcommand(WPARAM wParam, LPARAM lParam);
		void wm_command_sendcommand_text(WPARAM wParam, LPARAM lParam);
		void wm_command_sendcommand_param(WPARAM wParam, LPARAM lParam);

	};
}
