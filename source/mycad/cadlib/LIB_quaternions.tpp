#pragma once

#include "LIB_matrices.h"

template <typename T>
inline Quaternion<T>::Quaternion(const Vector3<T>& axis, T angle)
{
	//angle is radian
	set(axis, angle);
}

template <typename T>
inline void Quaternion<T>::set(T s, T x, T y, T z)
{
	this->s = s;  this->x = x;  this->y = y;  this->z = z;
}

template <typename T>
inline void Quaternion<T>::set(const Vector3<T>& axis, T angle)
{
	Vector3 v = axis;
	v.normalize();                  //convert to unit vector
	double sine = sin(angle);       //angle is radian
	s = cos(angle);
	x = v.x * sine;
	y = v.y * sine;
	z = v.z * sine;
}

template <typename T>
inline T Quaternion<T>::length() const
{
	return sqrt(s*s + x * x + y * y + z * z);
}

template <typename T>
inline Quaternion<T>& Quaternion<T>::normalize()
{
	const double EPSILON = 0.00001;
	double d = s * s + x * x + y * y + z * z;
	if (d < EPSILON)
		return *this; //do nothing if it is zero

	double invLength = 1.0 / sqrt(d);
	s *= invLength;  x *= invLength;  y *= invLength;  z *= invLength;
	return *this;
}

template <typename T>
inline Quaternion<T>& Quaternion<T>::conjugate()
{
	x = -x;  y = -y;  z = -z;
	return *this;
}

template <typename T>
inline Quaternion<T>& Quaternion<T>::invert()
{
	const double EPSILON = 0.00001;
	double d = s * s + x * x + y * y + z * z;
	if (d < EPSILON)
		return *this; //do nothing if it is zero

	Quaternion<T> q = *this;
	*this = q.conjugate() * (1.0 / d); //q* / |q||q|
	return *this;
}

template <typename T>
inline Matrix4<T> Quaternion<T>::getMatrix() const
{
	//NOTE: assume the quaternion is unit length
	//compute cmdson values
	T x2 = x + x;
	T y2 = y + y;
	T z2 = z + z;
	T xx2 = x * x2;
	T xy2 = x * y2;
	T xz2 = x * z2;
	T yy2 = y * y2;
	T yz2 = y * z2;
	T zz2 = z * z2;
	T sx2 = s * x2;
	T sy2 = s * y2;
	T sz2 = s * z2;

	//build 4x4 matrix (column-major) and return
	return Matrix4<T>(1 - (yy2 + zz2), xy2 + sz2, xz2 - sy2, 0,
		xy2 - sz2, 1 - (xx2 + zz2), yz2 + sx2, 0,
		xz2 + sy2, yz2 - sx2, 1 - (xx2 + yy2), 0,
		0, 0, 0, 1);

	//for non-unit quaternion
	//ss+xx-yy-zz, 2xy+2sz,     2xz-2sy,     0
	//2xy-2sz,     ss-xx+yy-zz, 2yz-2sx,     0
	//2xz+2sy,     2yz+2sx,     ss-xx-yy+zz, 0
	//0,           0,           0,           1
}

template <typename T>
inline Quaternion<T> Quaternion<T>::operator-() const
{
	return Quaternion<T>(-s, -x, -y, -z);
}

template <typename T>
inline Quaternion<T> Quaternion<T>::operator+(const Quaternion& rhs) const
{
	return Quaternion<T>(s + rhs.s, x + rhs.x, y + rhs.y, z + rhs.z);
}

template <typename T>
inline Quaternion<T> Quaternion<T>::operator-(const Quaternion& rhs) const
{
	return Quaternion<T>(s - rhs.s, x - rhs.x, y - rhs.y, z - rhs.z);
}

template <typename T>
inline Quaternion<T> Quaternion<T>::operator*(T a) const
{
	return Quaternion<T>(a*s, a*x, a*y, a*z);
}

template <typename T>
inline Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const
{
	Vector3 v1(x, y, z);
	Vector3 v2(rhs.x, rhs.y, rhs.z);

	Vector3 cross = v1.cross(v2);                   //v x v'
	T dot = v1.dot(v2);                         //v . v'
	Vector3 v3 = cross + (s * v2) + (rhs.s * v1);   //v x v' + sv' + s'v

	return Quaternion<T>(s * rhs.s - dot, v3.x, v3.y, v3.z);
}

template <typename T>
inline Quaternion<T> Quaternion<T>::operator*(const Vector3<T>& v) const
{
	Quaternion<T> q(0, x, y, z);
	return *this * q;
}

template <typename T>
inline Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& rhs)
{
	s += rhs.s;  x += rhs.x;  y += rhs.y;  z += rhs.z;
	return *this;
}

template <typename T>
inline Quaternion<T>& Quaternion<T>::operator-=(const Quaternion& rhs)
{
	s -= rhs.s;  x -= rhs.x;  y -= rhs.y;  z -= rhs.z;
	return *this;
}

template <typename T>
inline Quaternion<T>& Quaternion<T>::operator*=(T a)
{
	s *= a;  x *= a;  y *= a; z *= a;
	return *this;
}

template <typename T>
inline Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& rhs)
{
	Vector3<T> v1(x, y, z);
	Vector3<T> v2(rhs.x, rhs.y, rhs.z);

	Vector3<T> cross = v1.cross(v2);                   //v x v'
	T dot = v1.dot(v2);                         //v . v'
	Vector3<T> v3 = cross + (s * v2) + (rhs.s * v1);   //v x v' + sv' + s'v

	s = s * rhs.s - dot;
	x = v3.x;
	y = v3.y;
	z = v3.z;
	return *this;
}

template <typename T>
inline bool Quaternion<T>::operator==(const Quaternion& rhs) const
{
	return (s == rhs.s) && (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

template <typename T>
inline bool Quaternion<T>::operator!=(const Quaternion& rhs) const
{
	return (s != rhs.s) || (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

/********************************************************************************************/
/*										static functions									*/
/********************************************************************************************/
template <typename T>
inline Quaternion<T> Quaternion<T>::getQuaternion(const Vector3<T>& v1, const Vector3<T>& v2)
{
	const double EPSILON = 0.001;
	const double HALF_M_PI = 1.570796;

	//if two vectors are equal return the vector with 0 rotation
	if (v1.equal(v2, EPSILON))
	{
		return Quaternion<T>(v1, 0);
	}
	//if two vectors are opposite return a perpendicular vector with 180 angle
	else if (v1.equal(-v2, EPSILON))
	{
		Vector3 v;
		if (v1.x > -EPSILON && v1.x < EPSILON)       //if x ~= 0
			v.set(1, 0, 0);
		else if (v1.y > -EPSILON && v1.y < EPSILON)  //if y ~= 0
			v.set(0, 1, 0);
		else                                        //if z ~= 0
			v.set(0, 0, 1);
		return Quaternion<T>(v, HALF_M_PI);
	}

	Vector3 u1 = v1;                    //convert to normal vector
	Vector3 u2 = v2;
	u1.normalize();
	u2.normalize();

	Vector3 v = u1.cross(u2);           //compute rotation axis
	T angle = acos(u1.dot(u2));    //rotation angle
	return Quaternion<T>(v, angle*0.5);   //return half angle for quaternion
}

template <typename T>
inline Quaternion<T> Quaternion<T>::getQuaternion(const Vector2<T>& angles)
{
	Quaternion<T> qx = Quaternion<T>(Vector3<T>(1, 0, 0), angles.x);   //rotate along X
	Quaternion<T> qy = Quaternion<T>(Vector3<T>(0, 1, 0), angles.y);   //rotate along Y
	return qx * qy; //order: y->x
}

template <typename T>
inline Quaternion<T> Quaternion<T>::getQuaternion(const Vector3<T>& angles)
{
	Quaternion<T> qx = Quaternion<T>(Vector3<T>(1, 0, 0), angles.x);   //rotate along X
	Quaternion<T> qy = Quaternion<T>(Vector3<T>(0, 1, 0), angles.y);   //rotate along Y
	Quaternion<T> qz = Quaternion<T>(Vector3<T>(0, 0, 1), angles.z);   //rotate along Z
	return qx * qy * qz;    //order: z->y->x
}

/********************************************************************************************/
/*										friend functions									*/
/********************************************************************************************/
template <typename T>
inline Quaternion<T> operator*(const T a, const Quaternion<T>& q) {
	return Quaternion<T>(a*q.s, a*q.x, a*q.y, a*q.z);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Quaternion<T>& q) {
	os << "(" << q.s << ", " << q.x << ", " << q.y << ", " << q.z << ")";
	return os;
}
