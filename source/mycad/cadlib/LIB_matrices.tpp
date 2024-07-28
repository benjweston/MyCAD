#pragma once

//The elements of the matrix are stored as column major order.
//| 0 2 |    | 0 3 6 |    |  0  4  8 12 |
//| 1 3 |    | 1 4 7 |    |  1  5  9 13 |
//           | 2 5 8 |    |  2  6 10 14 |
//                        |  3  7 11 15 |

#include "LIB_conversions.h"

template <typename T>
inline Matrix2<T>::Matrix2() {
	//initially identity matrix
	identity();
}

template <typename T>
inline Matrix2<T>::Matrix2(const T src[4]) {
	set(src);
}

template <typename T>
inline Matrix2<T>::Matrix2(T m0, T m1, T m2, T m3) {
	set(m0, m1, m2, m3);
}

template <typename T>
inline void Matrix2<T>::set(const T src[4]) {
	m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
}

template <typename T>
inline void Matrix2<T>::set(T m0, T m1, T m2, T m3) {
	m[0] = m0;  m[1] = m1;  m[2] = m2;  m[3] = m3;
}

template <typename T>
inline void Matrix2<T>::setRow(int index, const T row[2]) {
	m[index] = row[0];  m[index + 2] = row[1];
}

template <typename T>
inline void Matrix2<T>::setRow(int index, const Vector2<T>& v) {
	m[index] = v.x;  m[index + 2] = v.y;
}

template <typename T>
inline void Matrix2<T>::setColumn(int index, const T col[2]) {
	m[index * 2] = col[0];  m[index * 2 + 1] = col[1];
}

template <typename T>
inline void Matrix2<T>::setColumn(int index, const Vector2<T>& v) {
	m[index * 2] = v.x;  m[index * 2 + 1] = v.y;
}

template <typename T>
inline const T* Matrix2<T>::get() const {
	return m;
}

template <typename T>
T Matrix2<T>::getDeterminant() const {
	return m[0] * m[3] - m[1] * m[2];
}

///////////////////////////////////////////////////////////////////////////////
//retrieve rotation angle in degree from rotation matrix, R
//R = | c -s |
//    | s  c |
//angle = atan(s / c)
///////////////////////////////////////////////////////////////////////////////
template <typename T>
T Matrix2<T>::getAngle() const {
	//angle between -pi ~ +pi (-180 ~ +180)
	return RAD2DEG<T>() * atan2(m[1], m[0]);
}

template <typename T>
inline Matrix2<T>& Matrix2<T>::identity() {
	m[0] = m[3] = 1.0f;
	m[1] = m[2] = 0.0f;
	return *this;
}

template <typename T>
Matrix2<T>& Matrix2<T>::transpose() {
	std::swap(m[1], m[2]);
	return *this;
}

template <typename T>
Matrix2<T>& Matrix2<T>::invert() {
	T determinant = getDeterminant();
	if (fabs(determinant) <= EPSILON<T>()) {
		return identity();
	}

	T tmp = m[0];   //copy the first element
	T invDeterminant = 1.0f / determinant;
	m[0] = invDeterminant * m[3];
	m[1] = -invDeterminant * m[1];
	m[2] = -invDeterminant * m[2];
	m[3] = invDeterminant * tmp;

	return *this;
}

template <typename T>
inline Matrix2<T> Matrix2<T>::operator+(const Matrix2& rhs) const {
	return Matrix2(m[0] + rhs[0], m[1] + rhs[1], m[2] + rhs[2], m[3] + rhs[3]);
}

template <typename T>
inline Matrix2<T> Matrix2<T>::operator-(const Matrix2& rhs) const {
	return Matrix2(m[0] - rhs[0], m[1] - rhs[1], m[2] - rhs[2], m[3] - rhs[3]);
}

template <typename T>
inline Matrix2<T>& Matrix2<T>::operator+=(const Matrix2& rhs) {
	m[0] += rhs[0];  m[1] += rhs[1];  m[2] += rhs[2];  m[3] += rhs[3];
	return *this;
}

template <typename T>
inline Matrix2<T>& Matrix2<T>::operator-=(const Matrix2& rhs) {
	m[0] -= rhs[0];  m[1] -= rhs[1];  m[2] -= rhs[2];  m[3] -= rhs[3];
	return *this;
}

template <typename T>
inline Vector2<T> Matrix2<T>::operator*(const Vector2<T>& rhs) const {
	return Vector2<T>(m[0] * rhs.x + m[2] * rhs.y, m[1] * rhs.x + m[3] * rhs.y);
}

template <typename T>
inline Matrix2<T> Matrix2<T>::operator*(const Matrix2& rhs) const {
	return Matrix2<T>(m[0] * rhs[0] + m[2] * rhs[1], m[1] * rhs[0] + m[3] * rhs[1],
		m[0] * rhs[2] + m[2] * rhs[3], m[1] * rhs[2] + m[3] * rhs[3]);
}

template <typename T>
inline Matrix2<T>& Matrix2<T>::operator*=(const Matrix2& rhs) {
	*this = *this * rhs;
	return *this;
}

template <typename T>
inline bool Matrix2<T>::operator==(const Matrix2& rhs) const {
	return (m[0] == rhs[0]) && (m[1] == rhs[1]) && (m[2] == rhs[2]) && (m[3] == rhs[3]);
}

template <typename T>
inline bool Matrix2<T>::operator!=(const Matrix2& rhs) const {
	return (m[0] != rhs[0]) || (m[1] != rhs[1]) || (m[2] != rhs[2]) || (m[3] != rhs[3]);
}

template <typename T>
inline T Matrix2<T>::operator[](int index) const {
	return m[index];
}

template <typename T>
inline T& Matrix2<T>::operator[](int index) {
	return m[index];
}

template <typename T>
inline Matrix2<T> operator-(const Matrix2<T>& rhs) {
	return Matrix2<T>(-rhs[0], -rhs[1], -rhs[2], -rhs[3]);
}

template <typename T>
inline Matrix2<T> operator*(double s, const Matrix2<T>& rhs) {
	return Matrix2<T>(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3]);
}

template <typename T>
inline Vector2<T> operator*(const Vector2<T>& v, const Matrix2<T>& rhs) {
	return Vector2<T>(v.x*rhs[0] + v.y*rhs[1], v.x*rhs[2] + v.y*rhs[3]);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Matrix2<T>& m) {
	os << std::fixed << std::setprecision(5);
	os << "[" << std::setw(10) << m[0] << " " << std::setw(10) << m[2] << "]\n"
		<< "[" << std::setw(10) << m[1] << " " << std::setw(10) << m[3] << "]\n";
	os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
	return os;
}


template <typename T>
inline Matrix3<T>::Matrix3() {
	//initially identity matrix
	identity();
}

template <typename T>
inline Matrix3<T>::Matrix3(const T src[9]) {
	set(src);
}

template <typename T>
inline Matrix3<T>::Matrix3(T m0, T m1, T m2,
	T m3, T m4, T m5,
	T m6, T m7, T m8) {
	set(m0, m1, m2, m3, m4, m5, m6, m7, m8);
}

template <typename T>
inline void Matrix3<T>::set(const T src[9]) {
	m[0] = src[0];  m[1] = src[1];  m[2] = src[2];
	m[3] = src[3];  m[4] = src[4];  m[5] = src[5];
	m[6] = src[6];  m[7] = src[7];  m[8] = src[8];
}

template <typename T>
inline void Matrix3<T>::set(T m0, T m1, T m2,
	T m3, T m4, T m5,
	T m6, T m7, T m8) {
	m[0] = m0;  m[1] = m1;  m[2] = m2;
	m[3] = m3;  m[4] = m4;  m[5] = m5;
	m[6] = m6;  m[7] = m7;  m[8] = m8;
}

template <typename T>
inline void Matrix3<T>::setRow(int index, const T row[3]) {
	m[index] = row[0];  m[index + 3] = row[1];  m[index + 6] = row[2];
}

template <typename T>
inline void Matrix3<T>::setRow(int index, const Vector3<T>& v) {
	m[index] = v.x;  m[index + 3] = v.y;  m[index + 6] = v.z;
}

template <typename T>
inline void Matrix3<T>::setColumn(int index, const T col[3]) {
	m[index * 3] = col[0];  m[index * 3 + 1] = col[1];  m[index * 3 + 2] = col[2];
}

template <typename T>
inline void Matrix3<T>::setColumn(int index, const Vector3<T>& v) {
	m[index * 3] = v.x;  m[index * 3 + 1] = v.y;  m[index * 3 + 2] = v.z;
}

template <typename T>
inline const T* Matrix3<T>::get() const {
	return m;
}

template <typename T>
T Matrix3<T>::getDeterminant() const {
	return m[0] * (m[4] * m[8] - m[5] * m[7]) -
		m[1] * (m[3] * m[8] - m[5] * m[6]) +
		m[2] * (m[3] * m[7] - m[4] * m[6]);
}

///////////////////////////////////////////////////////////////////////////////
//retrieve angles in degree from rotation matrix, M = Rx*Ry*Rz
//Rx: rotation about X-axis, pitch
//Ry: rotation about Y-axis, yaw(heading)
//Rz: rotation about Z-axis, roll
//   Rx           Ry          Rz
//|1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
//|0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
//|0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
//
//Pitch: atan(-m[7] / m[8]) = atan(SxCy/CxCy)
//Yaw  : asin(m[6]) = asin(Sy)
//Roll : atan(-m[3] / m[0]) = atan(SzCy/CzCy)
///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vector3<T> Matrix3<T>::getAngle() const {
	T pitch, yaw, roll;         //3 angles

	//find yaw (around y-axis) first
	//NOTE: asin() returns -90~+90, so correct the angle range -180~+180
	//using z value of forward vector
	yaw = RAD2DEG<T>() * asin(m[6]);
	if (m[8] < 0) {
		if (yaw >= 0) yaw = 180.0 - yaw;
		else         yaw = -180.0 - yaw;
	}

	//find roll (around z-axis) and pitch (around x-axis)
	//if forward vector is (1,0,0) or (-1,0,0), then m[0]=m[4]=m[9]=m[10]=0
	if (m[0] > -EPSILON<T>() && m[0] < EPSILON<T>()) {
		roll = 0;  //@@ assume roll=0
		pitch = RAD2DEG<T>() * atan2(m[1], m[4]);
	}
	else {
		roll = RAD2DEG<T>() * atan2(-m[3], m[0]);
		pitch = RAD2DEG<T>() * atan2(-m[7], m[8]);
	}

	return Vector3<T>(pitch, yaw, roll);
}

template <typename T>
inline Matrix3<T>& Matrix3<T>::identity() {
	m[0] = m[4] = m[8] = 1.0f;
	m[1] = m[2] = m[3] = m[5] = m[6] = m[7] = 0.0f;
	return *this;
}

template <typename T>
Matrix3<T>& Matrix3<T>::transpose() {
	std::swap(m[1], m[3]);
	std::swap(m[2], m[6]);
	std::swap(m[5], m[7]);

	return *this;
}

template <typename T>
Matrix3<T>& Matrix3<T>::invert() {
	double determinant, invDeterminant;
	double tmp[9];

	tmp[0] = m[4] * m[8] - m[5] * m[7];
	tmp[1] = m[2] * m[7] - m[1] * m[8];
	tmp[2] = m[1] * m[5] - m[2] * m[4];
	tmp[3] = m[5] * m[6] - m[3] * m[8];
	tmp[4] = m[0] * m[8] - m[2] * m[6];
	tmp[5] = m[2] * m[3] - m[0] * m[5];
	tmp[6] = m[3] * m[7] - m[4] * m[6];
	tmp[7] = m[1] * m[6] - m[0] * m[7];
	tmp[8] = m[0] * m[4] - m[1] * m[3];

	//check determinant if it is 0
	determinant = m[0] * tmp[0] + m[1] * tmp[3] + m[2] * tmp[6];
	if (abs(determinant) <= EPSILON<T>()) {
		return identity(); //cannot inverse, make it idenety matrix
	}

	//divide by the determinant
	invDeterminant = 1.0 / determinant;
	m[0] = invDeterminant * tmp[0];
	m[1] = invDeterminant * tmp[1];
	m[2] = invDeterminant * tmp[2];
	m[3] = invDeterminant * tmp[3];
	m[4] = invDeterminant * tmp[4];
	m[5] = invDeterminant * tmp[5];
	m[6] = invDeterminant * tmp[6];
	m[7] = invDeterminant * tmp[7];
	m[8] = invDeterminant * tmp[8];

	return *this;
}

//template <typename T>
//inline Matrix3<T> Matrix3<T>::to() const {
//	return Matrix3((T)m[0], (T)m[1], (T)m[2], (T)m[3], (T)m[4], (T)m[5], (T)m[6] , (T)m[7], (T)m[8]);
//}

template <typename T>
inline Matrix3<T> Matrix3<T>::operator+(const Matrix3& rhs) const {
	return Matrix3(m[0] + rhs[0], m[1] + rhs[1], m[2] + rhs[2],
		m[3] + rhs[3], m[4] + rhs[4], m[5] + rhs[5],
		m[6] + rhs[6], m[7] + rhs[7], m[8] + rhs[8]);
}

template <typename T>
inline Matrix3<T> Matrix3<T>::operator-(const Matrix3& rhs) const {
	return Matrix3(m[0] - rhs[0], m[1] - rhs[1], m[2] - rhs[2],
		m[3] - rhs[3], m[4] - rhs[4], m[5] - rhs[5],
		m[6] - rhs[6], m[7] - rhs[7], m[8] - rhs[8]);
}

template <typename T>
inline Matrix3<T>& Matrix3<T>::operator+=(const Matrix3& rhs) {
	m[0] += rhs[0];  m[1] += rhs[1];  m[2] += rhs[2];
	m[3] += rhs[3];  m[4] += rhs[4];  m[5] += rhs[5];
	m[6] += rhs[6];  m[7] += rhs[7];  m[8] += rhs[8];
	return *this;
}

template <typename T>
inline Matrix3<T>& Matrix3<T>::operator-=(const Matrix3& rhs) {
	m[0] -= rhs[0];  m[1] -= rhs[1];  m[2] -= rhs[2];
	m[3] -= rhs[3];  m[4] -= rhs[4];  m[5] -= rhs[5];
	m[6] -= rhs[6];  m[7] -= rhs[7];  m[8] -= rhs[8];
	return *this;
}

template <typename T>
inline Vector3<T> Matrix3<T>::operator*(const Vector3<T>& rhs) const {
	return Vector3<T>(m[0] * rhs.x + m[3] * rhs.y + m[6] * rhs.z,
		m[1] * rhs.x + m[4] * rhs.y + m[7] * rhs.z,
		m[2] * rhs.x + m[5] * rhs.y + m[8] * rhs.z);
}

template <typename T>
inline Matrix3<T> Matrix3<T>::operator*(const Matrix3& rhs) const {
	return Matrix3(m[0] * rhs[0] + m[3] * rhs[1] + m[6] * rhs[2], m[1] * rhs[0] + m[4] * rhs[1] + m[7] * rhs[2], m[2] * rhs[0] + m[5] * rhs[1] + m[8] * rhs[2],
		m[0] * rhs[3] + m[3] * rhs[4] + m[6] * rhs[5], m[1] * rhs[3] + m[4] * rhs[4] + m[7] * rhs[5], m[2] * rhs[3] + m[5] * rhs[4] + m[8] * rhs[5],
		m[0] * rhs[6] + m[3] * rhs[7] + m[6] * rhs[8], m[1] * rhs[6] + m[4] * rhs[7] + m[7] * rhs[8], m[2] * rhs[6] + m[5] * rhs[7] + m[8] * rhs[8]);
}

template <typename T>
inline Matrix3<T>& Matrix3<T>::operator*=(const Matrix3& rhs) {
	*this = *this * rhs;
	return *this;
}

template <typename T>
inline bool Matrix3<T>::operator==(const Matrix3& rhs) const {
	return (m[0] == rhs[0]) && (m[1] == rhs[1]) && (m[2] == rhs[2]) &&
		(m[3] == rhs[3]) && (m[4] == rhs[4]) && (m[5] == rhs[5]) &&
		(m[6] == rhs[6]) && (m[7] == rhs[7]) && (m[8] == rhs[8]);
}

template <typename T>
inline bool Matrix3<T>::operator!=(const Matrix3& rhs) const {
	return (m[0] != rhs[0]) || (m[1] != rhs[1]) || (m[2] != rhs[2]) ||
		(m[3] != rhs[3]) || (m[4] != rhs[4]) || (m[5] != rhs[5]) ||
		(m[6] != rhs[6]) || (m[7] != rhs[7]) || (m[8] != rhs[8]);
}

template <typename T>
inline T Matrix3<T>::operator[](int index) const {
	return m[index];
}

template <typename T>
inline T& Matrix3<T>::operator[](int index) {
	return m[index];
}

template <typename T>
inline Matrix3<T> operator-(const Matrix3<T>& rhs) {
	return Matrix3<T>(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8]);
}

template <typename T>
inline Matrix3<T> operator*(double s, const Matrix3<T>& rhs) {
	return Matrix3<T>(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8]);
}

template <typename T>
inline Vector3<T> operator*(const Vector3<T>& v, const Matrix3<T>& m) {
	return Vector3<T>(v.x*m[0] + v.y*m[1] + v.z*m[2], v.x*m[3] + v.y*m[4] + v.z*m[5], v.x*m[6] + v.y*m[7] + v.z*m[8]);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Matrix3<T>& m) {
	os << std::fixed << std::setprecision(5);
	os << "[" << std::setw(10) << m[0] << " " << std::setw(10) << m[3] << " " << std::setw(10) << m[6] << "]\n"
		<< "[" << std::setw(10) << m[1] << " " << std::setw(10) << m[4] << " " << std::setw(10) << m[7] << "]\n"
		<< "[" << std::setw(10) << m[2] << " " << std::setw(10) << m[5] << " " << std::setw(10) << m[8] << "]\n";
	os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
	return os;
}


template <typename T>
inline Matrix4<T>::Matrix4() {
	//initially identity matrix
	identity();
}

template <typename T>
inline Matrix4<T>::Matrix4(const T src[16]) {
	set(src);
}

template <typename T>
inline Matrix4<T>::Matrix4(T m00, T m01, T m02, T m03,
	T m04, T m05, T m06, T m07,
	T m08, T m09, T m10, T m11,
	T m12, T m13, T m14, T m15) {
	set(m00, m01, m02, m03, m04, m05, m06, m07, m08, m09, m10, m11, m12, m13, m14, m15);
}

template <typename T>
inline void Matrix4<T>::set(const T src[16]) {
	m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
	m[4] = src[4];  m[5] = src[5];  m[6] = src[6];  m[7] = src[7];
	m[8] = src[8];  m[9] = src[9];  m[10] = src[10]; m[11] = src[11];
	m[12] = src[12]; m[13] = src[13]; m[14] = src[14]; m[15] = src[15];
}

template <typename T>
inline void Matrix4<T>::set(T m00, T m01, T m02, T m03,
	T m04, T m05, T m06, T m07,
	T m08, T m09, T m10, T m11,
	T m12, T m13, T m14, T m15) {
	m[0] = m00;  m[1] = m01;  m[2] = m02;  m[3] = m03;
	m[4] = m04;  m[5] = m05;  m[6] = m06;  m[7] = m07;
	m[8] = m08;  m[9] = m09;  m[10] = m10;  m[11] = m11;
	m[12] = m12;  m[13] = m13;  m[14] = m14;  m[15] = m15;
}

template <typename T>
inline void Matrix4<T>::setRow(int index, const T row[4]) {
	m[index] = row[0];  m[index + 4] = row[1];  m[index + 8] = row[2];  m[index + 12] = row[3];
}

template <typename T>
inline void Matrix4<T>::setRow(int index, const Vector4<T>& v) {
	m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;  m[index + 12] = v.w;
}

template <typename T>
inline void Matrix4<T>::setRow(int index, const Vector3<T>& v) {
	m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;
}

template <typename T>
inline void Matrix4<T>::setColumn(int index, const T col[4]) {
	m[index * 4] = col[0];  m[index * 4 + 1] = col[1];  m[index * 4 + 2] = col[2];  m[index * 4 + 3] = col[3];
}

template <typename T>
inline void Matrix4<T>::setColumn(int index, const Vector4<T>& v) {
	m[index * 4] = v.x;  m[index * 4 + 1] = v.y;  m[index * 4 + 2] = v.z;  m[index * 4 + 3] = v.w;
}

template <typename T>
inline void Matrix4<T>::setColumn(int index, const Vector3<T>& v) {
	m[index * 4] = v.x;  m[index * 4 + 1] = v.y;  m[index * 4 + 2] = v.z;
}

template <typename T>
inline const T* Matrix4<T>::get() const {
	return m;
}

template <typename T>
inline const T* Matrix4<T>::getTranspose() {
	tm[0] = m[0];   tm[1] = m[4];   tm[2] = m[8];   tm[3] = m[12];
	tm[4] = m[1];   tm[5] = m[5];   tm[6] = m[9];   tm[7] = m[13];
	tm[8] = m[2];   tm[9] = m[6];   tm[10] = m[10];  tm[11] = m[14];
	tm[12] = m[3];   tm[13] = m[7];   tm[14] = m[11];  tm[15] = m[15];
	return tm;
}

template <typename T>
T Matrix4<T>::getDeterminant() const {
	return m[0] * getCofactor(m[5], m[6], m[7], m[9], m[10], m[11], m[13], m[14], m[15]) -
		m[1] * getCofactor(m[4], m[6], m[7], m[8], m[10], m[11], m[12], m[14], m[15]) +
		m[2] * getCofactor(m[4], m[5], m[7], m[8], m[9], m[11], m[12], m[13], m[15]) -
		m[3] * getCofactor(m[4], m[5], m[6], m[8], m[9], m[10], m[12], m[13], m[14]);
}

///////////////////////////////////////////////////////////////////////////////
//retrieve angles in degree from rotation matrix, M = Rx*Ry*Rz
//Rx: rotation about X-axis, pitch
//Ry: rotation about Y-axis, yaw(heading)
//Rz: rotation about Z-axis, roll
//   Rx           Ry          Rz
//|1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
//|0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
//|0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
//
//Pitch: atan(-m[7] / m[8]) = atan(SxCy/CxCy)
//Yaw  : asin(m[6]) = asin(Sy)
//Roll : atan(-m[3] / m[0]) = atan(SzCy/CzCy)
///////////////////////////////////////////////////////////////////////////////
template <typename T>
Vector3<T> Matrix4<T>::getAngle() const {
	T pitch, yaw, roll;         //3 angles

	//find yaw (around y-axis) first
	//NOTE: asin() returns -90~+90, so correct the angle range -180~+180
	//using z value of forward vector
	yaw = RAD2DEG<T>() * asin(m[6]);
	if (m[8] < 0) {
		if (yaw >= 0) yaw = 180.0 - yaw;
		else         yaw = -180.0 - yaw;
	}

	//find roll (around z-axis) and pitch (around x-axis)
	//if forward vector is (1,0,0) or (-1,0,0), then m[0]=m[4]=m[9]=m[10]=0
	if (m[0] > -EPSILON<T>() && m[0] < EPSILON<T>()) {
		roll = 0;  //@@ assume roll=0
		pitch = RAD2DEG<T>() * atan2(m[1], m[4]);
	}
	else {
		roll = RAD2DEG<T>() * atan2(-m[3], m[0]);
		pitch = RAD2DEG<T>() * atan2(-m[7], m[8]);
	}

	return Vector3<T>(pitch, yaw, roll);
}

//template <typename T>
//inline Matrix3<T> Matrix4<T>::getRotationMatrix() const {
//	Matrix3<T> mat(m[0], m[1], m[2],
//		m[4], m[5], m[6],
//		m[8], m[9], m[10]);
//	return mat;
//}

template <typename T>
inline Matrix4<T>& Matrix4<T>::identity() {
	m[0] = m[5] = m[10] = m[15] = 1.0f;
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	return *this;
}

template <typename T>
Matrix4<T>& Matrix4<T>::transpose() {
	std::swap(m[1], m[4]);
	std::swap(m[2], m[8]);
	std::swap(m[3], m[12]);
	std::swap(m[6], m[9]);
	std::swap(m[7], m[13]);
	std::swap(m[11], m[14]);

	return *this;
}

template <typename T>
Matrix4<T>& Matrix4<T>::invert() {
	//If the 4th row is [0,0,0,1] then it is affine matrix and
	//it has no projective transformation.
	if (m[3] == 0 && m[7] == 0 && m[11] == 0 && m[15] == 1) {
		this->invertAffine();
	}
	else {
		this->invertGeneral();
		//invertProjective() is not optimized (slower than generic one)
		if (fabs(m[0] * m[5] - m[1] * m[4]) > EPSILON<T>())
			this->invertProjective();   //inverse using matrix partition
		else
			this->invertGeneral();      //generalized inverse

	}

	return *this;
}

template <typename T>
inline Matrix4<T> Matrix4<T>::operator+(const Matrix4& rhs) const {
	return Matrix4(m[0] + rhs[0], m[1] + rhs[1], m[2] + rhs[2], m[3] + rhs[3],
		m[4] + rhs[4], m[5] + rhs[5], m[6] + rhs[6], m[7] + rhs[7],
		m[8] + rhs[8], m[9] + rhs[9], m[10] + rhs[10], m[11] + rhs[11],
		m[12] + rhs[12], m[13] + rhs[13], m[14] + rhs[14], m[15] + rhs[15]);
}

template <typename T>
inline Matrix4<T> Matrix4<T>::operator-(const Matrix4& rhs) const {
	return Matrix4(m[0] - rhs[0], m[1] - rhs[1], m[2] - rhs[2], m[3] - rhs[3],
		m[4] - rhs[4], m[5] - rhs[5], m[6] - rhs[6], m[7] - rhs[7],
		m[8] - rhs[8], m[9] - rhs[9], m[10] - rhs[10], m[11] - rhs[11],
		m[12] - rhs[12], m[13] - rhs[13], m[14] - rhs[14], m[15] - rhs[15]);
}

template <typename T>
inline Matrix4<T>& Matrix4<T>::operator+=(const Matrix4& rhs) {
	m[0] += rhs[0];   m[1] += rhs[1];   m[2] += rhs[2];   m[3] += rhs[3];
	m[4] += rhs[4];   m[5] += rhs[5];   m[6] += rhs[6];   m[7] += rhs[7];
	m[8] += rhs[8];   m[9] += rhs[9];   m[10] += rhs[10];  m[11] += rhs[11];
	m[12] += rhs[12];  m[13] += rhs[13];  m[14] += rhs[14];  m[15] += rhs[15];
	return *this;
}

template <typename T>
inline Matrix4<T>& Matrix4<T>::operator-=(const Matrix4& rhs) {
	m[0] -= rhs[0];   m[1] -= rhs[1];   m[2] -= rhs[2];   m[3] -= rhs[3];
	m[4] -= rhs[4];   m[5] -= rhs[5];   m[6] -= rhs[6];   m[7] -= rhs[7];
	m[8] -= rhs[8];   m[9] -= rhs[9];   m[10] -= rhs[10];  m[11] -= rhs[11];
	m[12] -= rhs[12];  m[13] -= rhs[13];  m[14] -= rhs[14];  m[15] -= rhs[15];
	return *this;
}

template <typename T>
inline Vector4<T> Matrix4<T>::operator*(const Vector4<T>& rhs) const {
	return Vector4<T>(m[0] * rhs.x + m[4] * rhs.y + m[8] * rhs.z + m[12] * rhs.w,
		m[1] * rhs.x + m[5] * rhs.y + m[9] * rhs.z + m[13] * rhs.w,
		m[2] * rhs.x + m[6] * rhs.y + m[10] * rhs.z + m[14] * rhs.w,
		m[3] * rhs.x + m[7] * rhs.y + m[11] * rhs.z + m[15] * rhs.w);
}

template <typename T>
inline Vector3<T> Matrix4<T>::operator*(const Vector3<T>& rhs) const {
	return Vector3<T>(m[0] * rhs.x + m[4] * rhs.y + m[8] * rhs.z + m[12],
		m[1] * rhs.x + m[5] * rhs.y + m[9] * rhs.z + m[13],
		m[2] * rhs.x + m[6] * rhs.y + m[10] * rhs.z + m[14]);
}

template <typename T>
inline Matrix4<T> Matrix4<T>::operator*(const Matrix4& n) const {
	return Matrix4<T>(m[0] * n[0] + m[4] * n[1] + m[8] * n[2] + m[12] * n[3], m[1] * n[0] + m[5] * n[1] + m[9] * n[2] + m[13] * n[3], m[2] * n[0] + m[6] * n[1] + m[10] * n[2] + m[14] * n[3], m[3] * n[0] + m[7] * n[1] + m[11] * n[2] + m[15] * n[3],
		m[0] * n[4] + m[4] * n[5] + m[8] * n[6] + m[12] * n[7], m[1] * n[4] + m[5] * n[5] + m[9] * n[6] + m[13] * n[7], m[2] * n[4] + m[6] * n[5] + m[10] * n[6] + m[14] * n[7], m[3] * n[4] + m[7] * n[5] + m[11] * n[6] + m[15] * n[7],
		m[0] * n[8] + m[4] * n[9] + m[8] * n[10] + m[12] * n[11], m[1] * n[8] + m[5] * n[9] + m[9] * n[10] + m[13] * n[11], m[2] * n[8] + m[6] * n[9] + m[10] * n[10] + m[14] * n[11], m[3] * n[8] + m[7] * n[9] + m[11] * n[10] + m[15] * n[11],
		m[0] * n[12] + m[4] * n[13] + m[8] * n[14] + m[12] * n[15], m[1] * n[12] + m[5] * n[13] + m[9] * n[14] + m[13] * n[15], m[2] * n[12] + m[6] * n[13] + m[10] * n[14] + m[14] * n[15], m[3] * n[12] + m[7] * n[13] + m[11] * n[14] + m[15] * n[15]);
}

template <typename T>
inline Matrix4<T>& Matrix4<T>::operator*=(const Matrix4& rhs) {
	*this = *this * rhs;
	return *this;
}

template <typename T>
inline bool Matrix4<T>::operator==(const Matrix4& n) const {
	return (m[0] == n[0]) && (m[1] == n[1]) && (m[2] == n[2]) && (m[3] == n[3]) &&
		(m[4] == n[4]) && (m[5] == n[5]) && (m[6] == n[6]) && (m[7] == n[7]) &&
		(m[8] == n[8]) && (m[9] == n[9]) && (m[10] == n[10]) && (m[11] == n[11]) &&
		(m[12] == n[12]) && (m[13] == n[13]) && (m[14] == n[14]) && (m[15] == n[15]);
}

template <typename T>
inline bool Matrix4<T>::operator!=(const Matrix4& n) const {
	return (m[0] != n[0]) || (m[1] != n[1]) || (m[2] != n[2]) || (m[3] != n[3]) ||
		(m[4] != n[4]) || (m[5] != n[5]) || (m[6] != n[6]) || (m[7] != n[7]) ||
		(m[8] != n[8]) || (m[9] != n[9]) || (m[10] != n[10]) || (m[11] != n[11]) ||
		(m[12] != n[12]) || (m[13] != n[13]) || (m[14] != n[14]) || (m[15] != n[15]);
}

template <typename T>
inline T Matrix4<T>::operator[](int index) const {
	return m[index];
}

template <typename T>
inline T& Matrix4<T>::operator[](int index) {
	return m[index];
}

template <typename T>
inline Matrix4<T> operator-(const Matrix4<T>& rhs) {
	return Matrix4<T>(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8], -rhs[9], -rhs[10], -rhs[11], -rhs[12], -rhs[13], -rhs[14], -rhs[15]);
}

template <typename T>
inline Matrix4<T> operator*(double s, const Matrix4<T>& rhs) {
	return Matrix4<T>(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8], s*rhs[9], s*rhs[10], s*rhs[11], s*rhs[12], s*rhs[13], s*rhs[14], s*rhs[15]);
}

template <typename T>
inline Vector4<T> operator*(const Vector4<T>& v, const Matrix4<T>& m) {
	return Vector4<T>(v.x*m[0] + v.y*m[1] + v.z*m[2] + v.w*m[3], v.x*m[4] + v.y*m[5] + v.z*m[6] + v.w*m[7], v.x*m[8] + v.y*m[9] + v.z*m[10] + v.w*m[11], v.x*m[12] + v.y*m[13] + v.z*m[14] + v.w*m[15]);
}

template <typename T>
inline Vector3<T> operator*(const Vector3<T>& v, const Matrix4<T>& m) {
	return Vector3<T>(v.x*m[0] + v.y*m[1] + v.z*m[2], v.x*m[4] + v.y*m[5] + v.z*m[6], v.x*m[8] + v.y*m[9] + v.z*m[10]);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Matrix4<T>& m) {
	os << std::fixed << std::setprecision(5);
	os << "[" << std::setw(10) << m[0] << " " << std::setw(10) << m[4] << " " << std::setw(10) << m[8] << " " << std::setw(10) << m[12] << "]\n"
		<< "[" << std::setw(10) << m[1] << " " << std::setw(10) << m[5] << " " << std::setw(10) << m[9] << " " << std::setw(10) << m[13] << "]\n"
		<< "[" << std::setw(10) << m[2] << " " << std::setw(10) << m[6] << " " << std::setw(10) << m[10] << " " << std::setw(10) << m[14] << "]\n"
		<< "[" << std::setw(10) << m[3] << " " << std::setw(10) << m[7] << " " << std::setw(10) << m[11] << " " << std::setw(10) << m[15] << "]\n";
	os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////
////compute cofactor of 3x3 minor matrix without sign
////input params are 9 elements of the minor matrix
////NOTE: The caller must know its sign.
/////////////////////////////////////////////////////////////////////////////////
template <typename T>
T Matrix4<T>::getCofactor(T m0, T m1, T m2,
	T m3, T m4, T m5,
	T m6, T m7, T m8) const {
	return m0 * (m4 * m8 - m5 * m7) -
		m1 * (m3 * m8 - m5 * m6) +
		m2 * (m3 * m7 - m4 * m6);
}
