#pragma once

#include "DRW_space.h"

namespace mycad {
	/**
	* Declaration of Layout class.
	*/
	class Layout {
	public:
		Layout(int index, const char* name, SIZE lsize, CommandManager* commandmanager, DataBlocks* datablocks);
		virtual ~Layout();

		enum class Context : int {
			ModelLayout,
			PaperLayout
		};

		virtual Colour3<float> getCrosshairColour() = 0;

		void setIndex(int index);
		int getIndex();

		void setName(const char* lpszName);
		char* getName();

		ViewportManager* Viewports();

		virtual Context getContext() const = 0;

		virtual Space::Context setActiveSpace(POINT point) = 0;//Sets the active space, returns the current context
		virtual Space::Context setActiveSpace(Space::Context context) = 0;//Sets the active space, returns the current context
		virtual Space* getActiveSpace() const;//Gets a pointer to the active space.

		virtual BOOL setActiveViewport(POINT point);
		virtual Viewport* getActiveViewport() const;

		virtual void setPaperSize(SIZE papersize) { UNREFERENCED_PARAMETER(papersize); };
		virtual SIZE getPaperSize() const { return { 0,0 }; };

		virtual void move(const double x, const double y, const double z = 0.0) = 0;
		virtual void rotate(double x, double y, double z) = 0;
		virtual void zoom(const double z) = 0;

		virtual void activate();
		virtual void mousemove(POINT) = 0;
		virtual void paint() = 0;
		virtual void size(SIZE) = 0;
		virtual void timer() = 0;

	protected:
		enum { MAX_LOADSTRING = 256 };

		char m_szName[MAX_LOADSTRING];
		int m_nIndex;

		SIZE m_Size;

		CommandManager* m_lpCommandManager;
		ViewportManager m_ViewportManager;
		DataBlocks* m_lpDataBlocks;

		UIViewport m_UserInterface;
		Space* m_lpActiveSpace;

	};



	/**
	* Declaration of PaperLayout class.
	*/
	class PaperLayout : public Layout {
	public:
		PaperLayout(int index, const char* name, SIZE lsize, CommandManager* commandmanager, DataBlocks* datablocks);
		~PaperLayout();

		static void setBackgroundColour(Colour3<float> colour);
		static void setPaperColour(Colour3<float> colour);

		Colour3<float> getCrosshairColour();

		Context getContext() const override;

		Space::Context setActiveSpace(POINT point) override;//Sets the active space, returns the previous context
		Space::Context setActiveSpace(Space::Context context) override;//Sets the active space, returns the previous context

		virtual void setPaperSize(SIZE papersize) override;
		virtual SIZE getPaperSize() const override;

		void move(const double x, const double y, const double z = 0.0) override;
		void rotate(double x, double y, double z) override;
		void zoom(const double z) override;

		void mousemove(POINT position) override;
		void paint() override;
		void size(SIZE lsize) override;
		void timer() override {}


	private:
		static Colour3<float> s_c3fBackgroundColour;
		static Colour3<float> s_c3fPaperColour;

		int m_nIndex;

		unsigned int m_nVBOid_vertices;
		unsigned int m_nVBOid_indices;

		RECT rcPrintableAreaOffset;
		RECT rcShadowOffset;
		RECT rcViewportsOffset;

		SIZE m_slPapersize;
		RECT m_rcPaper;
		RECT m_rcShadow;
		RECT m_rcMargin;
		RECT m_rcViewports;

		PViewport m_PaperViewport;

		MSpace* m_lpModelSpace;
		PSpace* m_lpPaperSpace;

		void setPaperSize();

	};



	/**
	* Declaration of ModelSpace class.
	*/
	class ModelSpace : public Space {
	public:
		ModelSpace(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* ui);
		~ModelSpace() {}

		static void setBackgroundColour(Colour3<float> colour);
		static void setBorderColour(Colour3<float> colour);

		static void setCrosshairColour(Colour3<float> colour, bool tintforxyz, int index);
		static Colour3<float> getCrosshairColour();

		static void setUCSIconColour(Colour3<float> colour);

		void move(const double x, const double y, const double z = 0.0) override;
		void rotate(double x, double y, double z) override;
		void zoom(const double z) override;

		void paint(ViewportManager* viewportmanager) override;
		void paint() override;
		void size(ViewportManager* viewportmanager, int x, int y, int width, int height) override;
		void size(int x, int y, int width, int height) override;
		void size(SIZE lsize) override;
		void size() override;
		void timer() override;


	private:
		static Colour3<float> s_c3fBackgroundColour;
		static Colour3<float> s_c3fBorderColour;
		static Colour3<float> s_c3fCrosshairColour[2];
		static bool s_bTintForXYZ[2];
		static Colour3<float> s_c3fUCSIconColour;

		int m_nFPSCounter;
		int m_nFPS;
		char m_chFrameLength[8];

		ModelspaceUCSIcon m_UCSIcon;
		GLText* m_lpText_FPS;
		GLText* m_lpText_FrameLength;

		Context getContext() const override;

		void drawEntity(Entity* ent) override;
		void drawEntities() override;

		void initFPS();
		void initFrameLength();
		void setFrameLength();

		void drawPointer();
		void drawUCS();
		void drawCommand();

	};



	/**
	* Declaration of ModelLayout class.
	*/
	class ModelLayout : public Layout {
	public:
		ModelLayout(int index, const char* name, SIZE lsize, CommandManager* commandmanager, DataBlocks* datablocks);
		~ModelLayout();

		static void setBackgroundColour(Colour3<float> colour);

		Colour3<float> getCrosshairColour();

		Context getContext() const override;

		Space::Context setActiveSpace(POINT point) override;
		Space::Context setActiveSpace(Space::Context context) override;//Sets the active space, returns the previous context

		void move(const double x, const double y, const double z = 0.0) override;
		void rotate(double x, double y, double z) override;
		void zoom(const double z) override;

		void mousemove(POINT position) override;
		void paint() override;
		void size(SIZE lsize) override;
		void timer() override;


	private:
		static Colour3<float> s_c3fBackgroundColour;

		ModelSpace* m_lpModelSpace;

	};
}
