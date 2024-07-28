#pragma once

#include "LIB_matrices.h"

#include <windows.h>

#define PresetViews \
    X(Current, -1) \
	X(Top, 0) \
    X(Bottom, 1) \
    X(Left, 2) \
	X(Right, 3) \
	X(Front, 4) \
	X(Back, 5) \
	X(SEIsometric, 6) \
	X(SWIsometric, 7) \
	X(NWIsometric, 8) \
	X(NEIsometric, 9)

#define VisualStyles \
	X(Current, -1) \
	X(Wireframe2D, 0) \
    X(Wireframe3D, 1) \
    X(Hidden2D, 2) \
	X(Hidden3D, 3) \
	X(Realistic, 4) \
	X(Conceptual, 5) \
	X(Shaded, 6) \
	X(ShadedWithEdges, 7) \
	X(ShadesOfGrey, 8) \
	X(Sketchy, 9) \
	X(XRay, 10)

namespace mycad {
	/**
	* Declaration of View class.
	*/
	class Camera;
	class View  {
		friend class Camera;

	public:
		enum class PresetView {
#define X(ID, VALUE) ID = VALUE,
			PresetViews
			INVALID
#undef X
		};

		enum class VisualStyle {
#define X(ID, VALUE) ID = VALUE,
			VisualStyles
			INVALID
#undef X
		};

	public:
		View();
		View(const View& view);
		View(const char* name, PresetView presetview = PresetView::Top);
		~View();

		void setName(const char *lpszName);
		char *getName();

		PresetView getPresetView();

		const char* viewToString(PresetView view);
		PresetView stringToView(const char* viewName);

		VisualStyle getVisualStyle() const;

		const char* visualstyleToString(VisualStyle visualstyle);
		VisualStyle stringToVisualstyle(const char* visualstyleName);

		bool getPerspective() const;

		double getFieldOfView() const;

		Vector3<double> getTarget() const;

		Vector3<double> getCamera() const;

		double getNear() const;

		double getFar() const;

		bool getClipping() const;

		void size(int width, int height);

	private:
		enum { MAX_LOADSTRING = 256 };

		char m_szName[MAX_LOADSTRING];

		SIZE m_szSize;

		PresetView m_enPresetView;
		VisualStyle m_enVisualStyle;

		bool m_bPerspective;

		Vector3<double> m_v3dTarget;
		Vector3<double> m_v3dCamera;
		Vector3<double> m_v3dDirection;

		double m_dNear;
		double m_dFar;

		double m_dMagnification;//This represents the reciprocal of the magnification co-efficient so as to avoid division operations.
		double m_dMagnificationMinimum;

		double m_dFieldOfView;//This is half the included field-of-view angle
		double m_dFieldOfViewMinimum;

		bool m_bClipping;

	};
}
