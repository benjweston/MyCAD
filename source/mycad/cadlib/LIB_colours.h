#pragma once

#include <iostream>

// Color constants.
//const COLORREF rgbRed = 0x000000FF;
//const COLORREF rgbGreen = 0x0000FF00;
//const COLORREF rgbBlue = 0x00FF0000;
//const COLORREF rgbBlack = 0x00000000;
//const COLORREF rgbWhite = 0x00FFFFFF;

//#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define RGBA(r,g,b,a)       ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(b))<<24)))

const unsigned int dxfColors[][3] = {
	{  0,  0,  0}, //unused
	{255,  0,  0}, //1 red
	{255,255,  0}, //2 yellow
	{  0,255,  0}, //3 green
	{  0,255,255}, //4 cyan
	{  0,  0,255}, //5 blue
	{255,  0,255}, //6 magenta
	{  0,  0,  0}, //7 black or white
	{128,128,128}, //8 50% gray
	{192,192,192}, //9 75% gray
	{255,  0,  0}, //10
	{255,127,127},
	{204,  0,  0},
	{204,102,102},
	{153,  0,  0},
	{153, 76, 76}, //15
	{127,  0,  0},
	{127, 63, 63},
	{ 76,  0,  0},
	{ 76, 38, 38},
	{255, 63,  0}, //20
	{255,159,127},
	{204, 51,  0},
	{204,127,102},
	{153, 38,  0},
	{153, 95, 76}, //25
	{127, 31,  0},
	{127, 79, 63},
	{ 76, 19,  0},
	{ 76, 47, 38},
	{255,127,  0}, //30
	{255,191,127},
	{204,102,  0},
	{204,153,102},
	{153, 76,  0},
	{153,114, 76}, //35
	{127, 63,  0},
	{127, 95, 63},
	{ 76, 38,  0},
	{ 76, 57, 38},
	{255,191,  0}, //40
	{255,223,127},
	{204,153,  0},
	{204,178,102},
	{153,114,  0},
	{153,133, 76}, //45
	{127, 95,  0},
	{127,111, 63},
	{ 76, 57,  0},
	{ 76, 66, 38},
	{255,255,  0}, //50
	{255,255,127},
	{204,204,  0},
	{204,204,102},
	{153,153,  0},
	{153,153, 76}, //55
	{127,127,  0},
	{127,127, 63},
	{ 76, 76,  0},
	{ 76, 76, 38},
	{191,255,  0}, //60
	{223,255,127},
	{153,204,  0},
	{178,204,102},
	{114,153,  0},
	{133,153, 76}, //65
	{ 95,127,  0},
	{111,127, 63},
	{ 57, 76,  0},
	{ 66, 76, 38},
	{127,255,  0}, //70
	{191,255,127},
	{102,204,  0},
	{153,204,102},
	{ 76,153,  0},
	{114,153, 76}, //75
	{ 63,127,  0},
	{ 95,127, 63},
	{ 38, 76,  0},
	{ 57, 76, 38},
	{ 63,255,  0}, //80
	{159,255,127},
	{ 51,204,  0},
	{127,204,102},
	{ 38,153,  0},
	{ 95,153, 76}, //85
	{ 31,127,  0},
	{ 79,127, 63},
	{ 19, 76,  0},
	{ 47, 76, 38},
	{  0,255,  0}, //90
	{127,255,127},
	{  0,204,  0},
	{102,204,102},
	{  0,153,  0},
	{ 76,153, 76}, //95
	{  0,127,  0},
	{ 63,127, 63},
	{  0, 76,  0},
	{ 38, 76, 38},
	{  0,255, 63}, //100
	{127,255,159},
	{  0,204, 51},
	{102,204,127},
	{  0,153, 38},
	{ 76,153, 95}, //105
	{  0,127, 31},
	{ 63,127, 79},
	{  0, 76, 19},
	{ 38, 76, 47},
	{  0,255,127}, //110
	{127,255,191},
	{  0,204,102},
	{102,204,153},
	{  0,153, 76},
	{ 76,153,114}, //115
	{  0,127, 63},
	{ 63,127, 95},
	{  0, 76, 38},
	{ 38, 76, 57},
	{  0,255,191}, //120
	{127,255,223},
	{  0,204,153},
	{102,204,178},
	{  0,153,114},
	{ 76,153,133}, //125
	{  0,127, 95},
	{ 63,127,111},
	{  0, 76, 57},
	{ 38, 76, 66},
	{  0,255,255}, //130
	{127,255,255},
	{  0,204,204},
	{102,204,204},
	{  0,153,153},
	{ 76,153,153}, //135
	{  0,127,127},
	{ 63,127,127},
	{  0, 76, 76},
	{ 38, 76, 76},
	{  0,191,255}, //140
	{127,223,255},
	{  0,153,204},
	{102,178,204},
	{  0,114,153},
	{ 76,133,153}, //145
	{  0, 95,127},
	{ 63,111,127},
	{  0, 57, 76},
	{ 38, 66, 76},
	{  0,127,255}, //150
	{127,191,255},
	{  0,102,204},
	{102,153,204},
	{  0, 76,153},
	{ 76,114,153}, //155
	{  0, 63,127},
	{ 63, 95,127},
	{  0, 38, 76},
	{ 38, 57, 76},
	{  0, 66,255}, //160
	{127,159,255},
	{  0, 51,204},
	{102,127,204},
	{  0, 38,153},
	{ 76, 95,153}, //165
	{  0, 31,127},
	{ 63, 79,127},
	{  0, 19, 76},
	{ 38, 47, 76},
	{  0,  0,255}, //170
	{127,127,255},
	{  0,  0,204},
	{102,102,204},
	{  0,  0,153},
	{ 76, 76,153}, //175
	{  0,  0,127},
	{ 63, 63,127},
	{  0,  0, 76},
	{ 38, 38, 76},
	{ 63,  0,255}, //180
	{159,127,255},
	{ 50,  0,204},
	{127,102,204},
	{ 38,  0,153},
	{ 95, 76,153}, //185
	{ 31,  0,127},
	{ 79, 63,127},
	{ 19,  0, 76},
	{ 47, 38, 76},
	{127,  0,255}, //190
	{191,127,255},
	{102,  0,204},
	{153,102,204},
	{ 76,  0,153},
	{114, 76,153}, //195
	{ 63,  0,127},
	{ 95, 63,127},
	{ 38,  0, 76},
	{ 57, 38, 76},
	{191,  0,255}, //200
	{223,127,255},
	{153,  0,204},
	{178,102,204},
	{114,  0,153},
	{133, 76,153}, //205
	{ 95,  0,127},
	{111, 63,127},
	{ 57,  0, 76},
	{ 66, 38, 76},
	{255,  0,255}, //210
	{255,127,255},
	{204,  0,204},
	{204,102,204},
	{153,  0,153},
	{153, 76,153}, //215
	{127,  0,127},
	{127, 63,127},
	{ 76,  0, 76},
	{ 76, 38, 76},
	{255,  0,191}, //220
	{255,127,223},
	{204,  0,153},
	{204,102,178},
	{153,  0,114},
	{153, 76,133}, //225
	{127,  0, 95},
	{127, 63, 11},
	{ 76,  0, 57},
	{ 76, 38, 66},
	{255,  0,127}, //230
	{255,127,191},
	{204,  0,102},
	{204,102,153},
	{153,  0, 76},
	{153, 76,114}, //235
	{127,  0, 63},
	{127, 63, 95},
	{ 76,  0, 38},
	{ 76, 38, 57},
	{255,  0, 63}, //240
	{255,127,159},
	{204,  0, 51},
	{204,102,127},
	{153,  0, 38},
	{153, 76, 95}, //245
	{127,  0, 31},
	{127, 63, 79},
	{ 76,  0, 19},
	{ 76, 38, 47},
	{ 51, 51, 51}, //250
	{ 91, 91, 91},
	{132,132,132},
	{173,173,173},
	{214,214,214},
	{255,255,255}  //255
};

/**
* Declaration of Colour3 class.
*/
template <typename T>
class Colour3 {
public:
	T r;                 /*!< r coordinate, code 10 */
	T g;                 /*!< g coordinate, code 20 */
	T b;                 /*!< b coordinate, code 30 */

	//ctors
	Colour3() : r(0), g(0), b(0) {};
	Colour3(T ir, T ig, T ib) : r(ir), g(ig), b(ib) {};

	//operators
	Colour3		operator-() const;						//unary operator (negate)
	Colour3		operator+(const Colour3& rhs) const;	//add rhs
	Colour3		operator-(const Colour3& rhs) const;	//subtract rhs
	Colour3&	operator+=(const Colour3& rhs);			//add rhs and update this object
	Colour3&	operator-=(const Colour3& rhs);			//subtract rhs and update this object
	Colour3		operator*(const T scale) const;			//scale
	Colour3		operator*(const Colour3& rhs) const;	//multiplay each element
	Colour3&	operator*=(const T scale);				//scale and update this object
	Colour3&	operator*=(const Colour3& rhs);			//product each element and update this object
	Colour3		operator/(const T scale) const;			//inverse scale
	Colour3&	operator/=(const T scale);				//scale and update this object
	bool		operator==(const Colour3& rhs) const;	//exact compare, no epsilon
	bool        operator!=(const Colour3& rhs) const;	//exact compare, no epsilon
	bool        operator<(const Colour3& rhs) const;	//comparison for sort
	T			operator[](int index) const;			//subscript operator v[0], v[1]
	T&			operator[](int index);					//subscript operator v[0], v[1]

	friend Colour3 operator*(const T a, const Colour3 vec);
	friend std::ostream& operator<<(std::ostream& os, const Colour3& vec);
};

template <typename T>
inline Colour3<T> Colour3<T>::operator-() const {
	return Colour3<T>(-r, -g, -b);
}

template <typename T>
inline Colour3<T> Colour3<T>::operator+(const Colour3& rhs) const {
	return Colour3<T>(r + rhs.r, g + rhs.g, b + rhs.b);
}

template <typename T>
inline Colour3<T> Colour3<T>::operator-(const Colour3& rhs) const {
	return Colour3<T>(r - rhs.r, g - rhs.g, b - rhs.b);
}

template <typename T>
inline Colour3<T>& Colour3<T>::operator+=(const Colour3& rhs) {
	r += rhs.r; g += rhs.g; b += rhs.b; return *this;
}

template <typename T>
inline Colour3<T>& Colour3<T>::operator-=(const Colour3& rhs) {
	r -= rhs.r; g -= rhs.g; b -= rhs.b; return *this;
}

template <typename T>
inline Colour3<T> Colour3<T>::operator*(const T a) const {
	return Colour3<T>(r*a, g*a, b*a);
}

template <typename T>
inline Colour3<T> Colour3<T>::operator*(const Colour3& rhs) const {
	return Colour3<T>(r*rhs.r, g*rhs.g, b*rhs.b);
}

template <typename T>
inline Colour3<T>& Colour3<T>::operator*=(const T a) {
	r *= a; g *= a; b *= a; return *this;
}

template <typename T>
inline Colour3<T>& Colour3<T>::operator*=(const Colour3& rhs) {
	r *= rhs.r; g *= rhs.g; b *= rhs.b; return *this;
}

template <typename T>
inline Colour3<T> Colour3<T>::operator/(const T a) const {
	return Colour3<T>(r / a, g / a, b / a);
}

template <typename T>
inline Colour3<T>& Colour3<T>::operator/=(const T a) {
	r /= a; g /= a; b /= a; return *this;
}

template <typename T>
inline bool Colour3<T>::operator==(const Colour3& rhs) const {
	return (r == rhs.r) && (g == rhs.g) && (b == rhs.b);
}

template <typename T>
inline bool Colour3<T>::operator!=(const Colour3& rhs) const {
	return (r != rhs.r) || (g != rhs.g) || (b != rhs.b);
}

template <typename T>
inline bool Colour3<T>::operator<(const Colour3& rhs) const {
	if (r < rhs.r) return true;
	if (r > rhs.r) return false;
	if (g < rhs.g) return true;
	if (g > rhs.g) return false;
	if (b < rhs.b) return true;
	if (b > rhs.b) return false;
	return false;
}

template <typename T>
inline T Colour3<T>::operator[](int index) const {
	return (&r)[index];
}

template <typename T>
inline T& Colour3<T>::operator[](int index) {
	return (&r)[index];
}

template <typename T>
inline Colour3<T> operator*(const T a, const Colour3<T> vec) {
	return Colour3<T>(a*vec.r, a*vec.g, a*vec.b);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Colour3<T>& vec) {
	os << "(" << vec.r << ", " << vec.g << ", " << vec.b << ")";
	return os;
}

/**
* Declaration of Colour4 class.
*/
template <typename T>
class Colour4 {
public:
	T r;
	T g;
	T b;
	T a;

	//ctors
	Colour4() : r(0), g(0), b(0), a(0) {};
	Colour4(T ir, T ig, T ib, T ia) : r(ir), g(ig), b(ib), a(ia) {};

	//operators
	Colour4     operator-() const;                      //unary operator (negate)
	Colour4     operator+(const Colour4& rhs) const;    //add rhs
	Colour4     operator-(const Colour4& rhs) const;    //subtract rhs
	Colour4&    operator+=(const Colour4& rhs);         //add rhs and update this object
	Colour4&    operator-=(const Colour4& rhs);         //subtract rhs and update this object
	Colour4     operator*(const T scale) const;			//scale
	Colour4     operator*(const Colour4& rhs) const;    //multiply each element
	Colour4&    operator*=(const T scale);				//scale and update this object
	Colour4&    operator*=(const Colour4& rhs);         //multiply each element and update this object
	Colour4     operator/(const T scale) const;			//inverse scale
	Colour4&    operator/=(const T scale);				//scale and update this object
	bool        operator==(const Colour4& rhs) const;   //exact compare, no epsilon
	bool        operator!=(const Colour4& rhs) const;   //exact compare, no epsilon
	bool        operator<(const Colour4& rhs) const;    //comparison for sort
	T			operator[](int index) const;			//subscript operator v[0], v[1]
	T&			operator[](int index);					//subscript operator v[0], v[1]

	friend Colour4 operator*(const T a, const Colour4 vec);
	friend std::ostream& operator<<(std::ostream& os, const Colour4& vec);
};

template <typename T>
inline Colour4<T> Colour4<T>::operator-() const {
	return Colour4<T>(-r, -g, -b, -a);
}

template <typename T>
inline Colour4<T> Colour4<T>::operator+(const Colour4& rhs) const {
	return Colour4<T>(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
}

template <typename T>
inline Colour4<T> Colour4<T>::operator-(const Colour4& rhs) const {
	return Colour4<T>(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
}

template <typename T>
inline Colour4<T>& Colour4<T>::operator+=(const Colour4& rhs) {
	r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a; return *this;
}

template <typename T>
inline Colour4<T>& Colour4<T>::operator-=(const Colour4& rhs) {
	r -= rhs.r; g -= rhs.g; b -= rhs.b; a -= rhs.a; return *this;
}

template <typename T>
inline Colour4<T> Colour4<T>::operator*(const T a) const {
	return Colour4<T>(r*a, g*a, b*a, a*a);
}

template <typename T>
inline Colour4<T> Colour4<T>::operator*(const Colour4& rhs) const {
	return Colour4<T>(r*rhs.r, g*rhs.g, b*rhs.b, a*rhs.a);
}

template <typename T>
inline Colour4<T>& Colour4<T>::operator*=(const T a) {
	r *= a; g *= a; b *= a; a *= a; return *this;
}

template <typename T>
inline Colour4<T>& Colour4<T>::operator*=(const Colour4& rhs) {
	r *= rhs.r; g *= rhs.g; b *= rhs.b; a *= rhs.a; return *this;
}

template <typename T>
inline Colour4<T> Colour4<T>::operator/(const T a) const {
	return Colour4<T>(r / a, g / a, b / a, a / a);
}

template <typename T>
inline Colour4<T>& Colour4<T>::operator/=(const T a) {
	r /= a; g /= a; b /= a; a /= a; return *this;
}

template <typename T>
inline bool Colour4<T>::operator==(const Colour4& rhs) const {
	return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
}

template <typename T>
inline bool Colour4<T>::operator!=(const Colour4& rhs) const {
	return (r != rhs.r) || (g != rhs.g) || (b != rhs.b) || (a != rhs.a);
}

template <typename T>
inline bool Colour4<T>::operator<(const Colour4& rhs) const {
	if (r < rhs.r) return true;
	if (r > rhs.r) return false;
	if (g < rhs.g) return true;
	if (g > rhs.g) return false;
	if (b < rhs.b) return true;
	if (b > rhs.b) return false;
	if (a < rhs.a) return true;
	if (a > rhs.a) return false;
	return false;
}

template <typename T>
inline T Colour4<T>::operator[](int index) const {
	return (&r)[index];
}

template <typename T>
inline T& Colour4<T>::operator[](int index) {
	return (&r)[index];
}

template <typename T>
inline Colour4<T> operator*(const T a, const Colour4<T> vec) {
	return Colour4<T>(a*vec.r, a*vec.g, a*vec.b, a*vec.a);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Colour4<T>& vec) {
	os << "(" << vec.r << ", " << vec.g << ", " << vec.b << ", " << vec.a << ")";
	return os;
}
