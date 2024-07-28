#pragma once

//Quaternion class represented as sum of a scalar and a vector(rotation axis); [s, v] = s + (ix + jy + kz)

#include "LIB_matrices.h"

/**
* Declaration of Quaternion class.
*/
template <typename T>
struct Quaternion
{
	T s;    //scalar part, s
	T x;    //vector part (x, y, z)
	T y;
	T z;

	//ctors
	Quaternion() : s(0), x(0), y(0), z(0) {}
	Quaternion(T s, T x, T y, T z) : s(s), x(x), y(y), z(z) {}
	Quaternion(const Vector3& axis, T angle);       //rot axis & half of rot (radian)

	//util functions
	void        set(T s, T x, T y, T z);
	void        set(const Vector3& axis, T angle);  //angle is radian
	T       length() const;                         //compute norm of q
	Quaternion& normalize();                            //convert it to unit q
	Quaternion& conjugate();                            //convert it to conjugate
	Quaternion& invert();                               //convert it to inverse q
	Matrix4     getMatrix() const;                      //return as 4x4 matrix

	//operators
	Quaternion  operator-() const;                      //unary operator (negate)
	Quaternion  operator+(const Quaternion& rhs) const; //addition
	Quaternion  operator-(const Quaternion& rhs) const; //subtraction
	Quaternion  operator*(T a) const;               //scalar multiplication
	Quaternion  operator*(const Quaternion& rhs) const; //multiplication
	Quaternion  operator*(const Vector3& v) const;      //conjugation for rotation
	Quaternion& operator+=(const Quaternion& rhs);      //addition
	Quaternion& operator-=(const Quaternion& rhs);      //subtraction
	Quaternion& operator*=(T a);                    //scalar multiplication
	Quaternion& operator*=(const Quaternion& rhs);      //multiplication
	bool        operator==(const Quaternion& rhs) const;//exact comparison
	bool        operator!=(const Quaternion& rhs) const;//exact comparison

	//friend functions
	friend Quaternion operator*(const T a, const Quaternion q);
	friend std::ostream& operator<<(std::ostream& os, const Quaternion& q);

	//static functions
	//return quaternion for rotation from v1 to v2
	static Quaternion getQuaternion(const Vector3& v1, const Vector3& v2);
	//return quaternion from Euler angles (x, y) or (x, y, z)
	//The rotation order is x->y->z.
	static Quaternion getQuaternion(const Vector2& angles);
	static Quaternion getQuaternion(const Vector3& angles);
};

#include "LIB_quaternions.tpp"
