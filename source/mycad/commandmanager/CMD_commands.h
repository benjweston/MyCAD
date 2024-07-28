#pragma once

#include "wm_windowwin32.h"

#include "LIB_colours.h"
#include "LIB_objectmanager.h"

#include "CMD_resource.h"

namespace mycad {
	enum class CommandContext : int {
		COMMAND = 0,
		VIEW
	};

	enum class CommandMode : int {
		NONE = 0,
		FILE,
		OUTPUT,
		BOTH
	};

	typedef struct tagCOMMANDINFO {
		int nID;
		char szAlias[256]{ 0 };
		char szCommandDisplayName[256]{ 0 };
		char szMacro[256]{ 0 };
		std::vector<std::string> vecPrompts;
		char szTooltip[256]{ 0 };
		char szDescription[256]{ 0 };
		int nValue{ 0 };
	} COMMANDINFO, *LPCOMMANDINFO;

	/**
	* Declaration of Command class.
	*/
	class DynamicInputWindow;
	class CommandLine;
	class DynamicInput;
	class TextWindow;
	class Command : public WindowWin32 {
	public:
		Command();
		Command(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		Command(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);
		Command(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, CommandMode mode = CommandMode::BOTH);
		Command(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, CommandMode mode = CommandMode::BOTH);
		virtual ~Command();

		static void setMode(CommandMode mode);
		static CommandMode getMode();

		static void setSystemVariable(int variableid, int value);
		static int getSystemVariable(int variableid);

		int getID() const;

		virtual void draw(POINT position, Colour3<float> colour);

		virtual void setText(const char* text);

	protected:
		static CommandMode m_commandMode;

		COMMANDINFO* m_lpCommandInfo;

		TextWindow* m_lpTextWindow;
		CommandLine* m_lpCommandLine;
		DynamicInput* m_lpDynamicInput;

		int m_nStep;

		bool m_bShortcutMenuLoopActive;

		POINT m_ptMouseDown;
		POINT m_ptMouseUp;
		POINT m_ptMouseMove;

		Colour3<float> m_c3fColour;

		virtual void initTextWindow();
		virtual void initCommandLine();

		virtual void initTextWindow(const char* text);
		virtual void initCommandLine(const char* text);

		virtual void initDynamicInput();

	};



	/**
	* Declaration of CommandWindow class.
	*/
	class GLControl;
	class CommandWindow : public Command {
	public:
		CommandWindow();
		CommandWindow(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		CommandWindow(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);
		CommandWindow(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, CommandMode mode = CommandMode::BOTH);
		CommandWindow(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, CommandMode mode = CommandMode::BOTH);
		virtual ~CommandWindow();

		virtual int wm_char(WPARAM wParam, LPARAM lParam);
		virtual int wm_close() override;
		virtual int wm_command(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_destroy() override;
		virtual int wm_entermenuloop(WPARAM wParam) override;
		virtual int wm_exitmenuloop(WPARAM wParam) override;
		virtual int wm_keydown(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_lbuttondblclk(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_lbuttondown(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_lbuttonup(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_mouseleave() override;
		virtual int wm_mousemove(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_mousewheel(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_notify(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_paint() override;

	protected:
		GLControl* m_lpGLControl;

		virtual int wm_char_return();
		virtual int wm_char_escape();
		virtual int wm_char_back();
		virtual int wm_char_default(WPARAM wParam, LPARAM lParam);

	};



	/**
	* Declaration of CommandDialog class.
	*/
	class CommandDialog : public Command {
	public:
		CommandDialog();
		CommandDialog(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		CommandDialog(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);
		CommandDialog(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, CommandMode mode = CommandMode::BOTH);
		CommandDialog(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, CommandMode mode = CommandMode::BOTH);
		virtual ~CommandDialog();

	};



	/**
	* Declaration of CViewBase class.
	*/
	class CViewBase : public CommandWindow {
	public:
		CViewBase(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		CViewBase(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);

	private:
		virtual void initCommandLine();
		virtual void initTextWindow();

	};



	/**
	* Declaration of CView2D class.
	*/
	class CView2D : public CViewBase {
	public:
		CView2D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		CView2D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);

		int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;

	private:


	};



	/**
	* Declaration of CView3D class.
	*/
	class CView3D : public CViewBase {
	public:
		CView3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		CView3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);

		int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;

	private:
		void wm_contextmenu_orbit_visualaids_ucsicon();

	};



	/**
	* Declaration of CPan class.
	*/
	class CPan : public CView2D {
	public:
		CPan(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		CPan(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);

		int wm_setcursor(WPARAM wParam, LPARAM lParam) override;

	private:


	};



	/**
	* Declaration of CPan3D class.
	*/
	class CPan3D : public CView3D {
	public:
		CPan3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		CPan3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);

		int wm_setcursor(WPARAM wParam, LPARAM lParam) override;

	private:


	};



	/**
	* Declaration of CZoom class.
	*/
	class CZoom : public CView2D {
	public:
		CZoom(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		CZoom(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);

		int wm_setcursor(WPARAM wParam, LPARAM lParam) override;

	private:


	};



	/**
	* Declaration of CZoom3D class.
	*/
	class CZoom3D : public CView3D {
	public:
		CZoom3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		CZoom3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);

		int wm_setcursor(WPARAM wParam, LPARAM lParam) override;

	private:


	};



	/**
	* Declaration of C3DOrbit class.
	*/
	class C3DOrbit : public CView3D {
	public:
		C3DOrbit(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		C3DOrbit(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);

		int wm_setcursor(WPARAM wParam, LPARAM lParam) override;

	private:


	};



	/**
	* Declaration of CLine class.
	*/
	class CLine : public CommandWindow {
	public:
		CLine(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, CommandMode mode = CommandMode::BOTH);
		CLine(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, CommandMode mode = CommandMode::BOTH);

		int wm_lbuttondown(WPARAM wParam, LPARAM lParam) override;
		int wm_lbuttonup(WPARAM wParam, LPARAM lParam) override;
		int wm_mousemove(WPARAM wParam, LPARAM lParam) override;

		int wm_char_return() override;

	private:
		POINT m_ptStartPoint;
		std::vector<POINT> m_vecNextPoints;

	};



	/**
	* Declaration of CSelect class.
	*/
	class CSelect : public CommandWindow {
	public:
		CSelect(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, Colour3<float> colour, CommandMode mode = CommandMode::BOTH);
		CSelect(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, Colour3<float> colour, CommandMode mode = CommandMode::BOTH);

		static SIZE s_szMouseHoverSize;

		enum class SelectionType {
			Pick,
			Window,
			Lasso
		};

		enum class SelectionMethod {
			Window,
			WPolygon,
			CPolygon,
			Fence,
		};

		void setSelectionType(SelectionType selectiontype);
		SelectionType getSelectionType();

		void setSelectionMethod(SelectionMethod selectionmethod);
		SelectionMethod getSelectionMethod();

		bool getMouseMoved();

		int wm_lbuttondown(WPARAM wParam, LPARAM lParam) override;
		int wm_lbuttonup(WPARAM wParam, LPARAM lParam) override;
		int wm_mousemove(WPARAM wParam, LPARAM lParam) override;

		int wm_char_return() override;

		void draw(POINT position, Colour3<float> colour);

	private:
		SelectionType m_enSelectionType;
		SelectionMethod m_enSelectionMethod;

		bool m_bMouseMoved;
		RECT m_rcMouseHoverSize;//Obtained with SystemParametersInfo(SPI_GETMOUSEHOVERWIDTH/SPI_GETMOUSEHOVERHEIGHT) so that system/graphics mouse behaviour is consistent.
		HRGN m_hrMouseHoverSize;

		void createMouseHoverSizeRegion();

	};


	/**
	* Declaration of CUnknown class.
	*/
	class CUnknown : public CommandWindow {
	public:
		CUnknown(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);


	private:
		void initTextWindow(const char* text) override;
		void initCommandLine(const char* text) override;

	};
}
