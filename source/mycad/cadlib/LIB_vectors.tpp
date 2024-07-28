#pragma once

#include "LIB_constants.h"

//fast math routines from Doom3 SDK
template <typename T>
inline T invSqrt(T x) {
	double xhalf = 0.5f * x;
	int i = *(int*)&x;          //get bits for floating value
	i = 0x5f3759df - (i >> 1);    //gives initial guess
	x = *(T*)&i;				//convert bits back to double
	x = x * (1.5f - xhalf * x*x); //Newton step
	return x;
}

template <typename T>
inline Vector2<T> Vector2<T>::operator-() const {
	return Vector2<T>(-x, -y);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator+(const Vector2& rhs) const {
	return Vector2<T>(x + rhs.x, y + rhs.y);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator-(const Vector2& rhs) const {
	return Vector2<T>(x - rhs.x, y - rhs.y);
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator+=(const Vector2& rhs) {
	x += rhs.x; y += rhs.y; return *this;
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator-=(const Vector2& rhs) {
	x -= rhs.x; y -= rhs.y; return *this;
}

template <typename T>
inline Vector2<T> Vector2<T>::operator*(const T a) const {
	return Vector2<T>(x*a, y*a);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator*(const Vector2& rhs) const {
	return Vector2<T>(x*rhs.x, y*rhs.y);
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator*=(const T a) {
	x *= a; y *= a; return *this;
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator*=(const Vector2& rhs) {
	x *= rhs.x; y *= rhs.y; return *this;
}

template <typename T>
inline Vector2<T> Vector2<T>::operator/(const T a) const {
	return Vector2<T>(x / a, y / a);
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator/=(const T a) {
	x /= a; y /= a; return *this;
}

template <typename T>
inline bool Vector2<T>::operator==(const Vector2& rhs) const {
	return (x == rhs.x) && (y == rhs.y);
}

template <typename T>
inline bool Vector2<T>::operator!=(const Vector2& rhs) const {
	return (x != rhs.x) || (y != rhs.y);
}

template <typename T>
inline bool Vector2<T>::operator<(const Vector2& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	return false;
}

template <typename T>
inline T Vector2<T>::operator[](int index) const {
	return (&x)[index];
}

template <typename T>
inline T& Vector2<T>::operator[](int index) {
	return (&x)[index];
}

template <typename T>
inline void Vector2<T>::set(T ix, T iy) {
	x = ix; y = iy;
}

template <typename T>
inline T Vector2<T>::length() const {
	return sqrt(x*x + y * y);
}

template <typename T>
inline T Vector2<T>::fDistance(const Vector2& vec) const {
	return sqrt((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y));
}

template <typename T>
inline Vector2<T>& Vector2<T>::normalize() {
	T xxyy = x * x + y * y;
	if (xxyy < EPSILON<T>()) {
		return *this;
	}
	T invLength = invSqrt(xxyy);
	x *= invLength;
	y *= invLength;
	return *this;
}

template <typename T>
inline T Vector2<T>::dot(const Vector2& rhs) const {
	return (x*rhs.x + y * rhs.y);
}

template <typename T>
inline bool Vector2<T>::equal(const Vector2& rhs, double epsilon) const {
	return abs(x - rhs.x) < epsilon && abs(y - rhs.y) < epsilon;
}

template <typename T>
inline Vector2<T> operator*(const T a, const Vector2<T> vec) {
	return Vector2<T>(a*vec.x, a*vec.y);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vector2<T>& vec) {
	os << "(" << vec.x << ", " << vec.y << ")";
	return os;
}


template <typename T>
inline Vector3<T> Vector3<T>::operator-() const {
	return Vector3<T>(-x, -y, -z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator+(const Vector3& rhs) const {
	return Vector3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator-(const Vector3& rhs) const {
	return Vector3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator+=(const Vector3& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator-=(const Vector3& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
}

template <typename T>
inline Vector3<T> Vector3<T>::operator*(const T a) const {
	return Vector3<T>(x*a, y*a, z*a);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator*(const Vector3& rhs) const {
	return Vector3<T>(x*rhs.x, y*rhs.y, z*rhs.z);
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator*=(const T a) {
	x *= a; y *= a; z *= a; return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator*=(const Vector3& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
}

template <typename T>
inline Vector3<T> Vector3<T>::operator/(const T a) const {
	return Vector3<T>(x / a, y / a, z / a);
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator/=(const T a) {
	x /= a; y /= a; z /= a; return *this;
}

template <typename T>
inline bool Vector3<T>::operator==(const Vector3& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

template <typename T>
inline bool Vector3<T>::operator!=(const Vector3& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

template <typename T>
inline bool Vector3<T>::operator<(const Vector3& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	if (z < rhs.z) return true;
	if (z > rhs.z) return false;
	return false;
}

template <typename T>
inline T Vector3<T>::operator[](int index) const {
	return (&x)[index];
}

template <typename T>
inline T& Vector3<T>::operator[](int index) {
	return (&x)[index];
}

template <typename T>
inline void Vector3<T>::set(T ix, T iy, T iz) {
	x = ix; y = iy; z = iz;
}

template <typename T>
inline T Vector3<T>::length() const {
	return sqrt(x*x + y * y + z * z);
}

template <typename T>
inline T Vector3<T>::fDistance(const Vector3& vec) const {
	return sqrt((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y) + (vec.z - z)*(vec.z - z));
}

template <typename T>
inline T Vector3<T>::angle(const Vector3& vec) const {
	//return angle between [0, 180]
	double l1 = this->length();
	double l2 = vec.length();
	double d = this->dot(vec);
	double angle = acos(d / (l1 * l2)) / 3.141592 * 180.0;
	return angle;
}

template <typename T>
inline Vector3<T>& Vector3<T>::normalize() {
	double xxyyzz = x * x + y * y + z * z;
	if (xxyyzz < EPSILON<T>()) {
		return *this;
	}
	T invLength = invSqrt(xxyyzz);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return *this;
}

template <typename T>
inline T Vector3<T>::dot(const Vector3& rhs) const {
	return (x*rhs.x + y * rhs.y + z * rhs.z);
}

template <typename T>
inline Vector3<T> Vector3<T>::cross(const Vector3& rhs) const {
	return Vector3<T>(y*rhs.z - z * rhs.y, z*rhs.x - x * rhs.z, x*rhs.y - y * rhs.x);
}

template <typename T>
inline bool Vector3<T>::equal(const Vector3& rhs, T epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
}

template <typename T>
inline Vector3<T> operator*(const T a, const Vector3<T> vec) {
	return Vector3<T>(a*vec.x, a*vec.y, a*vec.z);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vector3<T>& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}


template <typename T>
inline Vector4<T> Vector4<T>::operator-() const {
	return Vector4<T>(-x, -y, -z, -w);
}

template <typename T>
inline Vector4<T> Vector4<T>::operator+(const Vector4& rhs) const {
	return Vector4<T>(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

template <typename T>
inline Vector4<T> Vector4<T>::operator-(const Vector4& rhs) const {
	return Vector4<T>(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

template <typename T>
inline Vector4<T>& Vector4<T>::operator+=(const Vector4& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
}

template <typename T>
inline Vector4<T>& Vector4<T>::operator-=(const Vector4& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
}

template <typename T>
inline Vector4<T> Vector4<T>::operator*(const T a) const {
	return Vector4<T>(x*a, y*a, z*a, w*a);
}

template <typename T>
inline Vector4<T> Vector4<T>::operator*(const Vector4& rhs) const {
	return Vector4<T>(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

template <typename T>
inline Vector4<T>& Vector4<T>::operator*=(const T a) {
	x *= a; y *= a; z *= a; w *= a; return *this;
}

template <typename T>
inline Vector4<T>& Vector4<T>::operator*=(const Vector4& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
}

template <typename T>
inline Vector4<T> Vector4<T>::operator/(const T a) const {
	return Vector4<T>(x / a, y / a, z / a, w / a);
}

template <typename T>
inline Vector4<T>& Vector4<T>::operator/=(const T a) {
	x /= a; y /= a; z /= a; w /= a; return *this;
}

template <typename T>
inline bool Vector4<T>::operator==(const Vector4& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}

template <typename T>
inline bool Vector4<T>::operator!=(const Vector4& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}

template <typename T>
inline bool Vector4<T>::operator<(const Vector4& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	if (z < rhs.z) return true;
	if (z > rhs.z) return false;
	if (w < rhs.w) return true;
	if (w > rhs.w) return false;
	return false;
}

template <typename T>
inline T Vector4<T>::operator[](int index) const {
	return (&x)[index];
}

template <typename T>
inline T& Vector4<T>::operator[](int index) {
	return (&x)[index];
}

template <typename T>
inline void Vector4<T>::set(T ix, T iy, T iz, T iw) {
	x = ix; y = iy; z = iz; w = iw;
}

template <typename T>
inline T Vector4<T>::length() const {
	return sqrt(x*x + y * y + z * z + w * w);
}

template <typename T>
inline T Vector4<T>::fDistance(const Vector4& vec) const {
	return sqrt((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y) + (vec.z - z)*(vec.z - z) + (vec.w - w)*(vec.w - w));
}

template <typename T>
inline Vector4<T>& Vector4<T>::normalize() {
	double xxyyzz = x * x + y * y + z * z;
	if (xxyyzz < EPSILON<T>()) {
		return *this;
	}
	T invLength = invSqrt(xxyyzz);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return *this;
}

template <typename T>
inline T Vector4<T>::dot(const Vector4& rhs) const {
	return (x*rhs.x + y * rhs.y + z * rhs.z + w * rhs.w);
}

template <typename T>
inline bool Vector4<T>::equal(const Vector4& rhs, T epsilon) const {
	return abs(x - rhs.x) < epsilon && abs(y - rhs.y) < epsilon &&
		abs(z - rhs.z) < epsilon && abs(w - rhs.w) < epsilon;
}

template <typename T>
inline Vector4<T> operator*(const T a, const Vector4<T> vec) {
	return Vector4<T>(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vector4<T>& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
	return os;
}
