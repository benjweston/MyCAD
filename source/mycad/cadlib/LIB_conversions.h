#pragma once

#include <cmath>

template <typename T>
//static const T DEG2RAD() { return static_cast<T>(acos(-1) / 180.0); }
//static const T DEG2RAD() { return static_cast<T>(3.1415926535897932384626433832795 / 180.0); }
static const T DEG2RAD(T degrees) { return degrees * static_cast<T>(3.1415926535897932384626433832795 / 180.0); }

template <typename T>
//static const T RAD2DEG() { return static_cast<T>(180.0 / acos(-1)); }
//static const T RAD2DEG() { return static_cast<T>(180.0 / 3.1415926535897932384626433832795); }
static const T RAD2DEG(T radians) { return radians * static_cast<T>(180.0 / 3.1415926535897932384626433832795); }
