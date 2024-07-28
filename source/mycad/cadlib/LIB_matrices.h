#pragma once

//The elements of the matrix are stored as column major order.
//| 0 2 |    | 0 3 6 |    |  0  4  8 12 |
//| 1 3 |    | 1 4 7 |    |  1  5  9 13 |
//           | 2 5 8 |    |  2  6 10 14 |
//                        |  3  7 11 15 |

#include "LIB_vectors.h"

#include <iomanip>

/**
* Declaration of Matrix2 class.
*/
template <typename T>
class Matrix2 {
public:
	//constructors
	Matrix2();  //init with identity
	Matrix2(const T src[4]);
	Matrix2(T m0, T m1, T m2, T m3);

	void			set(const T src[4]);
	void			set(T m0, T m1, T m2, T m3);
	void			setRow(int index, const T row[2]);
	void			setRow(int index, const Vector2<T>& v);
	void			setColumn(int index, const T col[2]);
	void			setColumn(int index, const Vector2<T>& v);

	const T*		get() const;
	T				getDeterminant() const;
	T				getAngle() const;							//retrieve angle (degree) from matrix

	Matrix2&		identity();
	Matrix2&		transpose();								//transpose itself and return reference
	Matrix2&		invert();

	//operators
	Matrix2<T>		operator+(const Matrix2<T>& rhs) const;		//add rhs
	Matrix2<T>		operator-(const Matrix2<T>& rhs) const;		//subtract rhs
	Matrix2<T>&		operator+=(const Matrix2<T>& rhs);			//add rhs and update this object
	Matrix2<T>&		operator-=(const Matrix2<T>& rhs);			//subtract rhs and update this object
	Vector2<T>		operator*(const Vector2<T>& rhs) const;		//multiplication: v' = M * v
	Matrix2<T>		operator*(const Matrix2<T>& rhs) const;		//multiplication: M3 = M1 * M2
	Matrix2<T>&		operator*=(const Matrix2<T>& rhs);			//multiplication: M1' = M1 * M2
	bool			operator==(const Matrix2<T>& rhs) const;	//exact compare, no epsilon
	bool			operator!=(const Matrix2<T>& rhs) const;	//exact compare, no epsilon
	T				operator[](int index) const;				//subscript operator v[0], v[1]
	T&				operator[](int index);						//subscript operator v[0], v[1]

	////friends functions
	//friend Matrix2 operator-(const Matrix2& m);                     //unary operator (-)
	//friend Matrix2 operator*(double scalar, const Matrix2& m);       //pre-multiplication
	//friend Vector2d operator*(const Vector2d& vec, const Matrix2& m); //pre-multiplication
	//friend std::ostream& operator<<(std::ostream& os, const Matrix2& m);

	//static functions

protected:

private:
	T m[4];

};

/**
* Declaration of Matrix3 class.
*/
template <typename T>
class Matrix3 {
public:
	//constructors
	Matrix3();  //init with identity
	Matrix3(const T src[9]);
	Matrix3(T m0, T m1, T m2,									//1st column
		T m3, T m4, T m5,										//2nd column
		T m6, T m7, T m8);										//3rd column

	void			set(const T src[9]);
	void			set(T m0, T m1, T m2,						//1st column
		T m3, T m4, T m5,						//2nd column
		T m6, T m7, T m8);						//3rd column
	void			setRow(int index, const T row[3]);
	void			setRow(int index, const Vector3<T>& v);
	void			setColumn(int index, const T col[3]);
	void			setColumn(int index, const Vector3<T>& v);

	const T*		get() const;

	T				getDeterminant() const;
	Vector3<T>		getAngle() const;							//return (pitch, yaw, roll)

	Matrix3&		identity();
	Matrix3&		transpose();								//transpose itself and return reference
	Matrix3&		invert();

	//type conversion
	//Matrix3<T>		to() const;

	//operators
	Matrix3<T>		operator+(const Matrix3<T>& rhs) const;		//add rhs
	Matrix3<T>		operator-(const Matrix3<T>& rhs) const;		//subtract rhs
	Matrix3<T>&		operator+=(const Matrix3<T>& rhs);			//add rhs and update this object
	Matrix3<T>&		operator-=(const Matrix3<T>& rhs);			//subtract rhs and update this object
	Vector3<T>		operator*(const Vector3<T>& rhs) const;		//multiplication: v' = M * v
	Matrix3<T>		operator*(const Matrix3<T>& rhs) const;		//multiplication: M3 = M1 * M2
	Matrix3<T>&		operator*=(const Matrix3<T>& rhs);			//multiplication: M1' = M1 * M2
	bool			operator==(const Matrix3<T>& rhs) const;	//exact compare, no epsilon
	bool			operator!=(const Matrix3<T>& rhs) const;	//exact compare, no epsilon
	T				operator[](int index) const;				//subscript operator v[0], v[1]
	T&				operator[](int index);						//subscript operator v[0], v[1]

	////friends functions
	//friend Matrix3 operator-(const Matrix3& m);                     //unary operator (-)
	//friend Matrix3 operator*(double scalar, const Matrix3& m);       //pre-multiplication
	//friend Vector3d operator*(const Vector3d& vec, const Matrix3& m); //pre-multiplication
	//friend std::ostream& operator<<(std::ostream& os, const Matrix3& m);

protected:

private:
	T m[9];

};

/**
* Declaration of Matrix4 class.
*/
template <typename T>
class Matrix4 {
public:
	//constructors
	Matrix4();  //init with identity
	Matrix4(const T src[16]);
	Matrix4(T m00, T m01, T m02, T m03,							//1st column
		T m04, T m05, T m06, T m07,								//2nd column
		T m08, T m09, T m10, T m11,								//3rd column
		T m12, T m13, T m14, T m15);							//4th column

	void			set(const T src[16]);
	void			set(T m00, T m01, T m02, T m03,				//1st column
		T m04, T m05, T m06, T m07,				//2nd column
		T m08, T m09, T m10, T m11,				//3rd column
		T m12, T m13, T m14, T m15);			//4th column
	void			setRow(int index, const T row[4]);
	void			setRow(int index, const Vector4<T>& v);
	void			setRow(int index, const Vector3<T>& v);
	void			setColumn(int index, const T col[4]);
	void			setColumn(int index, const Vector4<T>& v);
	void			setColumn(int index, const Vector3<T>& v);

	const T*		get() const;
	const T*		getTranspose();								//return transposed matrix
	T				getDeterminant() const;
	//Matrix3<T>		getRotationMatrix() const;					//return 3x3 rotation part
	Vector3<T>		getAngle() const;							//return (pitch, yaw, roll)
	//Vector3d		getTranslationVector() const;				//return 1*3 translation part

	Matrix4&		identity();
	Matrix4&		transpose();								//transpose itself and return reference
	Matrix4&		invert();									//check best inverse method before inverse
	//Matrix4&		invertEuclidean();							//inverse of Euclidean transform matrix
	//Matrix4&		invertAffine();								//inverse of affine transform matrix
	//Matrix4&		invertProjective();							//inverse of projective matrix using partitioning
	//Matrix4&		invertGeneral();							//inverse of generic matrix

	////transform matrix
	//Matrix4&    translate(double x, double y, double z);		//translation by (x,y,z)
	//Matrix4&    translate(const Vector3d& v);					//
	//Matrix4&    rotate(double angle, const Vector3d& axis);	//rotate angle(degree) along the given axix
	//Matrix4&    rotate(double angle, double x, double y, double z);
	//Matrix4&    rotateX(double angle);						//rotate on X-axis with degree
	//Matrix4&    rotateY(double angle);						//rotate on Y-axis with degree
	//Matrix4&    rotateZ(double angle);						//rotate on Z-axis with degree
	//Matrix4&    scale(double scale);							//uniform scale
	//Matrix4&    scale(double sx, double sy, double sz);		//scale by (sx, sy, sz) on each axis
	//Matrix4&    lookAt(double tx, double ty, double tz);		//face object to the target direction
	//Matrix4&    lookAt(double tx, double ty, double tz, double ux, double uy, double uz);
	//Matrix4&    lookAt(const Vector3d& target);
	//Matrix4&    lookAt(const Vector3d& target, const Vector3d& up);
	////@@Matrix4&    skew(double angle, const Vector3d& axis); //

	//operators
	Matrix4<T>		operator+(const Matrix4<T>& rhs) const;		//add rhs
	Matrix4<T>		operator-(const Matrix4<T>& rhs) const;		//subtract rhs
	Matrix4<T>&		operator+=(const Matrix4<T>& rhs);			//add rhs and update this object
	Matrix4<T>&		operator-=(const Matrix4<T>& rhs);			//subtract rhs and update this object
	Vector4<T>		operator*(const Vector4<T>& rhs) const;		//multiplication: v' = M * v
	Vector3<T>		operator*(const Vector3<T>& rhs) const;		//multiplication: v' = M * v
	Matrix4<T>		operator*(const Matrix4<T>& rhs) const;		//multiplication: M3 = M1 * M2
	Matrix4<T>&		operator*=(const Matrix4<T>& rhs);			//multiplication: M1' = M1 * M2
	bool			operator==(const Matrix4<T>& rhs) const;	//exact compare, no epsilon
	bool			operator!=(const Matrix4<T>& rhs) const;	//exact compare, no epsilon
	T				operator[](int index) const;				//subscript operator v[0], v[1]
	T&				operator[](int index);						//subscript operator v[0], v[1]

	////friends functions
	//friend Matrix4 operator-(const Matrix4& m);                     //unary operator (-)
	//friend Matrix4 operator*(double scalar, const Matrix4& m);       //pre-multiplication
	//friend Vector3d operator*(const Vector3d& vec, const Matrix4& m); //pre-multiplication
	//friend Vector4d operator*(const Vector4d& vec, const Matrix4& m); //pre-multiplication
	//friend std::ostream& operator<<(std::ostream& os, const Matrix4& m);

protected:

private:
	T       getCofactor(T m0, T m1, T m2,
		T m3, T m4, T m5,
		T m6, T m7, T m8) const;

	T m[16];
	T tm[16];                                       //transpose m

};

#include "LIB_matrices.tpp"
