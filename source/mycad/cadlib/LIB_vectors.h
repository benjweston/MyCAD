#pragma once

//#include "LIB_constants.h"

#include <iostream>
#include <vector>

/**
* Declaration of Vector2 class.
*/
template <typename T>
class Vector2 {
public:
	T x;                 /*!< x coordinate, code 10 */
	T y;                 /*!< y coordinate, code 20 */

	//ctors
	Vector2() : x(0), y(0) {};
	Vector2(T ix, T iy) : x(ix), y(iy) {};

	//utils functions
	void        set(T ix, T iy);
	T			length() const;								//
	T			fDistance(const Vector2& vec) const;		//fDistance between two vectors
	Vector2&	normalize();								//
	T			dot(const Vector2& vec) const;				//dot product
	bool        equal(const Vector2& vec, double e) const;	//compare with epsilon

	//operators
	Vector2		operator-() const;							//unary operator (negate)
	Vector2		operator+(const Vector2& rhs) const;		//add rhs
	Vector2		operator-(const Vector2& rhs) const;		//subtract rhs
	Vector2&	operator+=(const Vector2& rhs);				//add rhs and update this object
	Vector2&	operator-=(const Vector2& rhs);				//subtract rhs and update this object
	Vector2		operator*(const T scale) const;				//scale
	Vector2		operator*(const Vector2& rhs) const;		//multiply each element
	Vector2&	operator*=(const T scale);					//scale and update this object
	Vector2&	operator*=(const Vector2& rhs);				//multiply each element and update this object
	Vector2		operator/(const T scale) const;				//inverse scale
	Vector2&	operator/=(const T scale);					//scale and update this object
	bool        operator==(const Vector2& rhs) const;		//exact compare, no epsilon
	bool        operator!=(const Vector2& rhs) const;		//exact compare, no epsilon
	bool        operator<(const Vector2& rhs) const;		//comparison for sort
	T			operator[](int index) const;				//subscript operator v[0], v[1]
	T&			operator[](int index);						//subscript operator v[0], v[1]

	friend Vector2 operator*(const T a, const Vector2 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector2& vec);
};

/**
* Declaration of Vector3 class.
*/
template <typename T>
class Vector3 {
public:
	T x;                 /*!< x coordinate, code 10 */
	T y;                 /*!< y coordinate, code 20 */
	T z;                 /*!< z coordinate, code 30 */

	//ctors
	Vector3() : x(0), y(0), z(0) {};
	Vector3(T ix, T iy, T iz) : x(ix), y(iy), z(iz) {};

	//utils functions
	void        set(T ix, T iy, T iz);
	T		length() const;                         //
	T      fDistance(const Vector3& vec) const;     //fDistance between two vectors
	T      angle(const Vector3& vec) const;        //angle between two vectors
	Vector3&   normalize();                            //
	T      dot(const Vector3& vec) const;          //dot product
	Vector3    cross(const Vector3& vec) const;        //cross product
	bool        equal(const Vector3& vec, T e) const; //compare with epsilon

	//operators
	Vector3    operator-() const;                      //unary operator (negate)
	Vector3    operator+(const Vector3& rhs) const;   //add rhs
	Vector3    operator-(const Vector3& rhs) const;   //subtract rhs
	Vector3&   operator+=(const Vector3& rhs);        //add rhs and update this object
	Vector3&   operator-=(const Vector3& rhs);        //subtract rhs and update this object
	Vector3    operator*(const T scale) const;    //scale
	Vector3    operator*(const Vector3& rhs) const;   //multiplay each element
	Vector3&   operator*=(const T scale);         //scale and update this object
	Vector3&	operator*=(const Vector3& rhs);        //product each element and update this object
	Vector3    operator/(const T scale) const;    //inverse scale
	Vector3&   operator/=(const T scale);         //scale and update this object
	bool		operator==(const Vector3& rhs) const;  //exact compare, no epsilon
	bool        operator!=(const Vector3& rhs) const;  //exact compare, no epsilon
	bool        operator<(const Vector3& rhs) const;   //comparison for sort
	T      operator[](int index) const;			//subscript operator v[0], v[1]
	T&     operator[](int index);                  //subscript operator v[0], v[1]

	friend Vector3 operator*(const T a, const Vector3 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector3& vec);
};

/**
* Declaration of Vector4 class.
*/
template <typename T>
class Vector4 {
public:
	T x;
	T y;
	T z;
	T w;

	//ctors
	Vector4() : x(0), y(0), z(0), w(0) {};
	Vector4(T ix, T iy, T iz, T iw) : x(ix), y(iy), z(iz), w(iw) {};

	//utils functions
	void        set(T ix, T iy, T iz, T iw);
	T       length() const;                         //
	T       fDistance(const Vector4& vec) const;     //fDistance between two vectors
	Vector4&    normalize();                            //
	T       dot(const Vector4& vec) const;          //dot product
	bool        equal(const Vector4& vec, T e) const; //compare with epsilon

	//operators
	Vector4     operator-() const;                      //unary operator (negate)
	Vector4     operator+(const Vector4& rhs) const;    //add rhs
	Vector4     operator-(const Vector4& rhs) const;    //subtract rhs
	Vector4&    operator+=(const Vector4& rhs);         //add rhs and update this object
	Vector4&    operator-=(const Vector4& rhs);         //subtract rhs and update this object
	Vector4     operator*(const T scale) const;     //scale
	Vector4     operator*(const Vector4& rhs) const;    //multiply each element
	Vector4&    operator*=(const T scale);          //scale and update this object
	Vector4&    operator*=(const Vector4& rhs);         //multiply each element and update this object
	Vector4     operator/(const T scale) const;     //inverse scale
	Vector4&    operator/=(const T scale);          //scale and update this object
	bool        operator==(const Vector4& rhs) const;   //exact compare, no epsilon
	bool        operator!=(const Vector4& rhs) const;   //exact compare, no epsilon
	bool        operator<(const Vector4& rhs) const;    //comparison for sort
	T       operator[](int index) const;            //subscript operator v[0], v[1]
	T&      operator[](int index);                  //subscript operator v[0], v[1]

	friend Vector4 operator*(const T a, const Vector4 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector4& vec);
};

#include "LIB_vectors.tpp"
