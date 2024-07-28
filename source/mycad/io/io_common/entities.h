#pragma once

#include "drw_entities.h"

#include "LIB_vectors.h"
#include "LIB_matrices.h"
#include "LIB_colours.h"

#include <vector>
#include <windows.h>

namespace mycad {
	/**
	* Declaration of Entity class.
	*/
	class Entity {
	public:
		Entity();
		virtual ~Entity();

		virtual DRW_Entity* getEntity();

		virtual void redraw();

		virtual void paint(Colour3<float> colour);

		static Colour3<int>IndexToVector3iColour(int index);
		static Colour3<float>IndexToVector3fColour(int index);

	protected:
		const char* vsSource;
		const char* fsSource;

		bool m_glslReady;
		bool m_vboReady;

		unsigned int m_vsId;
		unsigned int m_fsId;
		unsigned int m_shaderProgram;
		unsigned int m_nVBOid;

		int m_inPositionLocation;
		int m_inColorLocation;

		std::vector<float> m_vecVertices;
		std::vector<float> m_vecColours;
		int m_iMode;

		bool bRedraw;

		virtual void create();
		virtual void create(Colour3<float> colour);

	};



	/**
	* Declaration of Block class.
	*/
	class Block : public Entity {
	public:
		Block();


	protected:
		

	private:


	};



	/**
	* Declaration of Point class.
	*/
	class Point : public Entity {
	public:
		Point(DRW_Point* entity);

		DRW_Entity* getEntity() override;

		void paint(Colour3<float> colour) override;

	protected:
		void create(Colour3<float> colour) override;

	private:
		DRW_Point* m_lpEntity;

	};



	class Line : public Entity {
	public:
		Line(DRW_Line* entity);

		DRW_Entity* getEntity() override;

		void paint(Colour3<float> colour) override;

	private:
		DRW_Line* m_lpEntity;

		void create(Colour3<float> colour) override;

	};



	class Ray : public Entity {
	public:
		Ray(DRW_Ray* entity);

		DRW_Entity* getEntity() override;

		void paint(Colour3<float> colour) override;

	private:
		DRW_Ray* m_lpEntity;

		void create(Colour3<float> colour) override;

	};



	class Xline : public Entity {
	public:
		Xline(DRW_Xline* entity);

		DRW_Entity* getEntity() override;

		void paint(Colour3<float> colour) override;

	private:
		DRW_Xline* m_lpEntity;

		void create(Colour3<float> colour) override;

	};



	class Circle : public Entity {
	public:
		Circle(DRW_Circle* entity);

		DRW_Entity* getEntity() override;

		void paint(Colour3<float> colour) override;

	private:
		DRW_Circle* m_lpEntity;

		Vector2<double>CalculateCircleCoordinates(const double angle, const double rad);

		void create(Colour3<float> colour) override;

	};



	class Arc : public Entity {
	public:
		Arc(DRW_Arc* entity);

		DRW_Entity* getEntity() override;

		void paint(Colour3<float> colour) override;

	private:
		DRW_Arc* m_lpEntity;

		Vector2<double>CalculateCircleCoordinates(const double angle, const double rad);

		void create(Colour3<float> colour) override;

	};



	class EllipsE : public Entity {
	public:
		EllipsE(DRW_Ellipse* entity);

		DRW_Entity* getEntity() override;

		void paint(Colour3<float> colour) override;

	private:
		DRW_Ellipse* m_lpEntity;

		double CalculateEllipseRadius(const double a, const double b, const double angle);
		Vector2<double> CalculateEllipseCoordinates(const double angle, const double radius);

		void create(Colour3<float> colour) override;

	};



	class LWPolyline : public Entity {
	public:
		LWPolyline(DRW_LWPolyline* entity);

		DRW_Entity* getEntity() override;

		void paint(Colour3<float> colour) override;

	private:
		DRW_LWPolyline* m_lpEntity;

		void create(Colour3<float> colour) override;

	};



	class Polyline : public Entity {
	public:
		Polyline(DRW_Polyline* entity);

		DRW_Entity* getEntity() override;

		void paint(Colour3<float> colour) override;

	private:
		DRW_Polyline* m_lpEntity;

		void create(Colour3<float> colour) override;

	};
}
