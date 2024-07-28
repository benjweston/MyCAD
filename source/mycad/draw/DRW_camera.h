#pragma once

#include "DRW_view.h"

namespace mycad {
	/**
	* Declaration of Camera class.
	*/
	class Camera {
	public:
		Camera(View* view, unsigned int id = 0);
		~Camera();

		unsigned int getID() const;

		double getDepth(int x, int y) const;

		Vector3<double> UnProject(const POINT point, const double z = 0.0);
		POINT Project(const double x, const double y);
		POINT Project(const double x, const double y, const double z);

		void setView(View* view);
		View* getView();

		bool setPresetView(const View::PresetView presetview);
		View::PresetView getPresetView();

		const char* viewToString(View::PresetView presetview);
		View::PresetView stringToView(const char* presetviewName);

		bool setVisualStyle(const View::VisualStyle visualstyle);
		View::VisualStyle getVisualStyle() const;

		const char* visualstyleToString(View::VisualStyle visualstyle);
		View::VisualStyle stringToVisualstyle(const char* visualstyleName);

		bool setPerspective(const bool perspective);
		bool getPerspective() const;

		void setDirection(double x, double y, double z);

		void setNear(const double n);
		double getNear() const;

		void setFar(const double f);
		double getFar() const;

		void setClipping(const bool clipping);
		bool getClipping() const;

		void setMagnification(const double magnification);
		double getMagnification() const;

		void setFOV(const double fov);
		double getFOV() const;

		void setPosition(const double x, const double y, const double z = 0.0);
		Vector3<double> getPosition() const;

		void setDistance(const double distance);
		double getDistance() const;

		Vector3<double> getLeftAxis() const;
		Vector3<double> getUpAxis() const;
		Vector3<double> getForwardAxis() const;

		const Matrix4<double>& getModelMatrixd();
		const Matrix4<double>& getViewMatrixd();
		const Matrix4<double>& getModelViewMatrixd();
		const Matrix4<double>& getProjectionMatrixd();

		const Matrix4<float>& getModelViewMatrixf();
		const Matrix4<float>& getProjectionMatrixf();

		const Matrix4<float>& getModelViewRotationMatrixf();

		const double* getModelMatrixElementsd();
		const double* getViewMatrixElementsd();
		const double* getModelViewMatrixElementsd();
		const double* getProjectionMatrixElementsd();

		const float* getModelViewMatrixElementsf();
		const float* getProjectionMatrixElementsf();

		bool isViewOrthogonal();

		void fov(double dv);
		void move(const double x, const double y, const double z = 0.0);
		void rotate(double x, double y, double z);
		void zoom(const double z);
		void zoom(const int x, const int y, const double z);

		void size(const int left, const int bottom, const int right, const int top);

	private:
		static unsigned int s_ndGenerator;

		View* m_lpView;
		unsigned int m_nId;
		int m_nLeft, m_nRight, m_nBottom, m_nTop; 

		Matrix4<double> m_m4dModel;
		Matrix4<double> m_m4dView;
		Matrix4<double> m_m4dModelView;
		Matrix4<double> m_m4dProjection;

		Matrix4<float> m_m4fModelView;
		Matrix4<float> m_m4fProjection;

		Matrix4<float> m_m4dModelViewRotation;

		void MagnificationToFOV();
		void FOVToMagnification();

		Matrix4<double> setOrthoFrustum(double l, double r, double b, double t, double n, double f);
		Matrix4<double> setFrustum(double l, double r, double b, double t, double n, double f);
		Matrix4<double> setFrustum(double fovY, double aspectRatio, double front, double back);

		void updateModelMatrix();
		void updateViewMatrix();
		void updateModelViewMatrix();
		void updateProjectionMatrix();

	};
}
