#pragma once

#include "DRW_camera.h"
#include "DRW_viewportmanager.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <Wingdi.h>

namespace mycad {
	typedef struct tagGLRECT : RECT {
	public:
		tagGLRECT() { SetGLRect(0, 0, 0, 0); }
		tagGLRECT(LONG xLeft, LONG yTop, LONG xWidth, LONG yHeight) { SetGLRect(xLeft, yTop, xLeft + xWidth, yTop + yHeight); }

		BOOL SetGLRect(int xLeft, int yTop, int xWidth, int yHeight) {
			width = xWidth;
			height = yHeight;
			extents = { xWidth,yHeight };
			return ::SetRect(this, xLeft, yTop, xLeft + xWidth, yTop + yHeight);
		}
		BOOL SetGLRect(int xWidth, int yHeight) {
			width = xWidth;
			height = yHeight;
			extents = { xWidth,yHeight };
			return ::SetRect(this, left, top, left + xWidth, top + yHeight);
		}
		LONG width;
		LONG height;
		SIZE extents;

	} GLRECT, *PGLRECT, *NPGLRECT, *LPGLRECT;

	enum class Orientation : int {
		Horizontal,
		Vertical
	};

	/**
	* Declaration of Splitter class.
	*/
	class Splitter : public GLRECT {
	public:
		Splitter();
		Splitter(Orientation orientation, float ratio);
		~Splitter();

		static void setThickness(int thickness);
		static int getThickness();

		void setOrientation(Orientation orientation);
		Orientation getOrientation();

		void setRatio(float ratio);
		float getRatio();

		virtual void size(const int x, const int y, const int cx, const int cy);
		virtual void size(const int cx, const int cy);

	private:
		static int s_Thickness;

		Orientation m_Orientation;
		float m_Ratio;

	};



	/**
	* Declaration of Viewport class.
	*/
	class Viewport : public GLRECT {
	public:
		Viewport();
		virtual ~Viewport();

		POINT getCursor() const;
		POINT getCursor(POINT position) const;

		virtual Camera* getUICamera() const = 0;
		virtual Camera* getCamera() const = 0;

		BOOL setUCSVisible(BOOL visible);
		BOOL getUCSVisible();

		BOOL setUCSOrigin(BOOL origin);
		BOOL getUCSOrigin();

		int setUCSDisplayState(int state);
		int getUCSDisplayState();

		bool setGridVisible(bool visible);
		bool getGridVisible();

		bool setGridXYVisible(bool visible);
		bool getGridXYVisible();
		bool setGridYZVisible(bool visible);
		bool getGridYZVisible();
		bool setGridZXVisible(bool visible);
		bool getGridZXVisible();

		virtual void move(const double x, const double y, const double z = 0.0) = 0;
		virtual void rotate(double x, double y, double z) = 0;
		virtual void zoom(const double z) = 0;

		virtual void mousemove(POINT position);
		virtual void size(const int x, const int y, const int cx, const int cy);
		virtual void size(const int cx, const int cy);

	protected:
		Camera* m_lpUICamera;
		POINT m_ptCursor;

		int m_nUCS_DisplayState;
		BOOL m_nUCS_Visible;
		BOOL m_nUCS_Origin;

		bool m_bGrid_Visible;
		bool m_bGridXY_Visible;
		bool m_bGridYZ_Visible;
		bool m_bGridZX_Visible;

	};



	/**
	* Declaration of UIViewport class.
	*/
	class UIViewport : public Viewport {
	public:
		UIViewport();
		~UIViewport();

		Camera* getUICamera() const override;
		Camera* getCamera() const override;

		void move(const double x, const double y, const double z = 0.0) override { UNREFERENCED_PARAMETER(x); UNREFERENCED_PARAMETER(y); UNREFERENCED_PARAMETER(z); }
		void rotate(double x, double y, double z) override { UNREFERENCED_PARAMETER(x); UNREFERENCED_PARAMETER(y); UNREFERENCED_PARAMETER(z); }
		void zoom(const double z) override { UNREFERENCED_PARAMETER(z); }

		void size(const int x, const int y, const int cx, const int cy) override;
		void size(const int cx, const int cy) override;

	private:
		Camera* m_lpCamera;

	};



	/**
	* Declaration of PViewport class.
	*/
	class PViewport : public Viewport {
	public:
		PViewport();
		~PViewport();

		Camera* getUICamera() const override;
		Camera* getCamera() const override;

		void move(const double x, const double y, const double z = 0.0) override;
		void rotate(double x, double y, double z) override { UNREFERENCED_PARAMETER(x); UNREFERENCED_PARAMETER(y); UNREFERENCED_PARAMETER(z); }
		void zoom(const double z) override;

		void size(const int x, const int y, const int cx, const int cy) override;
		void size(const int cx, const int cy) override;

	private:
		Camera* m_lpCamera;

	};



	/**
	* Declaration of MViewport class.
	*/
	class MViewport : public Viewport {
	public:
		MViewport();
		~MViewport();

		Camera* getUICamera() const override;
		Camera* getCamera() const override;

		void setViewportManager(ViewportManager* viewportmanager);
		ViewportManager* getViewportManager() const;

		void move(const double x, const double y, const double z = 0.0) override;
		void rotate(double x, double y, double z) override;
		void zoom(const double z) override;

		void size(const int x, const int y, const int cx, const int cy) override;
		void size(const int cx, const int cy) override;

	private:
		Camera* m_lpCamera;
		ViewportManager* m_lpViewportManager;

	};
}
