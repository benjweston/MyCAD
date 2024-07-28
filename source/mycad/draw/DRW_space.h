#pragma once

#include "CMD_commandmanager.h"

#include "DRW_viewport.h"

#include "GUI_glgrid.h"
#include "GUI_glpointer.h"
#include "GUI_glucsicon.h"
#include "GUI_gltext.h"

#include "entities.h"

//#define IDC_LAYOUT 0x0030
#if !defined IDC_LAYOUT
#define IDC_LAYOUT 0x0030
#endif

namespace mycad {
	/**
	* Declaration of DataBlock class.
	*/
	class DataBlock : public Block {
	public:
		DataBlock() {}
		DataBlock(const Block& p) :Block(p) {}
		~DataBlock() {
			for (std::list<Entity*>::const_iterator it = ent.begin(); it != ent.end(); ++it) {
				delete* it;
			}
		}
		std::list<Entity*> ent;
	};

	/**
	* Declaration of DataBlocks class.
	*/
	class DataBlocks {
	public:
		DataBlocks() {}
		~DataBlocks() {

		}
		DataBlock m_mBlock;
		std::list<DataBlock*> m_Blocks;

	};

	/**
	* Declaration of Space class.
	*/
	class Space {
	public:
		Space(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* uiviewport);
		virtual ~Space();

		enum class Context : int {
			ModelSpace,
			PaperSpace
		};

		static void setBackgroundColour(Colour3<float> colour);

		virtual Context getContext() const = 0;

		virtual BOOL setActiveViewport(POINT point);
		virtual Viewport* getActiveViewport() const;

		virtual void setViewportConfiguration(ViewportManager::LPVIEWPORTDATA configdat);

		void setView(View* view);
		bool setPresetView(const View::PresetView presetview);
		bool setVisualStyle(const View::VisualStyle visualstyle);

		bool setPerspective(const bool perspective);
		bool getPerspective();

		virtual BOOL PtInRect(POINT point);

		virtual void move(const double x, const double y, const double z = 0.0) = 0;
		virtual void rotate(double x, double y, double z) = 0;
		virtual void zoom(const double z) = 0;

		virtual void mousemove(POINT position);
		virtual void paint(ViewportManager* viewportmanager);
		virtual void paint();
		virtual void size(ViewportManager* viewportmanager, int x, int y, int width, int height);
		virtual void size(int x, int y, int width, int height);
		virtual void size(SIZE lsize);
		virtual void size() {}
		virtual void timer() {}


	protected:
		static Colour3<float> s_c3fBackgroundColour;

		CommandManager* m_lpCommandManager;
		ViewportManager* m_lpViewportManager;
		DataBlocks* m_lpDataBlocks;

		MViewport* m_lpActiveViewport;
		UIViewport* m_lpUserInterface;

		GLRECT m_rcDimensions;
		POINT m_ptCursor;

		int m_nBorderWidth;

		unsigned int m_nVBOid_vertices;
		unsigned int m_nVBOid_indices;

		GLGrid m_Grid;
		GLPointer m_Pointer;

		GLText* m_lpText_Properties;

		virtual BOOL setActiveViewport(ViewportManager* viewportmanager, POINT point);

		virtual void drawActiveBorder(int left, int top, int right, int bottom, int width, Colour4<float> colour = { 0.f, 0.f, 0.f, 1.f });
		virtual void drawActiveBorder(LPRECT lprc, int width, Colour4<float> colour = { 0.f, 0.f, 0.f, 1.f });

		virtual void drawBorders(int left, int top, int right, int bottom, Colour4<float> colour = { 0.f, 0.f, 0.f, 1.f });
		virtual void drawBorders(LPRECT lprc, Colour4<float> colour = { 0.f, 0.f, 0.f, 1.f });

		virtual void initProperties();
		virtual void setProperties();

		virtual void drawGrid(SIZE viewport, bool xy, bool yz, bool zx);

		virtual void drawEntity(Entity* ent);
		virtual void drawEntities();

	};



	/**
	* Declaration of PaperSpace class.
	*/
	class PaperSpace : public Space {
	public:
		PaperSpace(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* uiviewport, PViewport* paperviewport, RECT* paper, RECT* shadow, RECT* margin, RECT* viewports);
		virtual ~PaperSpace();

		static void setCrosshairColour(Colour3<float> colour, bool tintforxyz);
		static Colour3<float> getCrosshairColour();

		virtual void paint(ViewportManager* viewportmanager);
		virtual void size(SIZE lsize) override;


	protected:
		static Colour3<float> s_c3fCrosshairColour;
		static bool s_bTintForXYZ;

		PViewport* m_lpPaperViewport;
		PViewport m_Paper;

		RECT* m_lprcPaper;
		RECT* m_lprcShadow;
		RECT* m_lprcMargin;
		RECT* m_lprcViewports;

		void drawShadow();
		void drawPaper();
		void drawMargin();

	};



	/**
	* Declaration of PSpace class.
	*/
	class PSpace : public PaperSpace {
	public:
		PSpace(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* uiviewport, PViewport* paperviewport, RECT* paper, RECT* shadow, RECT* margin, RECT* viewports);
		~PSpace();

		static void setUCSIconColour(Colour3<float> colour);

		Context getContext() const override;

		Viewport* getActiveViewport() const override;//Overrides the virtual function in the Space class.

		void move(const double x, const double y, const double z = 0.0) override;
		void rotate(double x, double y, double z) override;
		void zoom(const double z) override;

		void mousemove(POINT position) override;
		void paint(ViewportManager* viewportmanager) override;
		void paint() override;


	private:
		static Colour3<float> s_c3fUCSIconColour;

		LayoutTabUCSIcon m_UCSIcon;

		void drawPointer(Colour3<float> colour);
		void drawUCS();
		void drawCommand();

	};



	/**
	* Declaration of MSpace class.
	*/
	class MSpace : public PaperSpace {
	public:
		MSpace(CommandManager* commandmanager, DataBlocks* datablocks, ViewportManager* viewportmanager, UIViewport* uiviewport, PViewport* paperviewport, RECT* paper, RECT* shadow, RECT* margin, RECT* viewports);
		~MSpace();

		static void setUCSIconColour(Colour3<float> colour);

		Context getContext() const override;

		void move(const double x, const double y, const double z = 0.0) override;
		void rotate(double x, double y, double z) override;
		void zoom(const double z) override;

		void paint(ViewportManager* viewportmanager) override;
		void paint() override;
		void size(SIZE lsize) override;
		void size() override;


	private:
		static Colour3<float> s_c3fUCSIconColour;

		ModelspaceUCSIcon m_UCSIcon;

		void drawPointer();
		void drawUCS();
		void drawCommand();

	};
}
