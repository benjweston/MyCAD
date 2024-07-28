#pragma once

#include <cmath>

template <typename T>
static const T EPSILON() { return (T)(0.00000001); }

template <typename T>
static const T PI() { return (T)(acos(-1)); }

template <typename T>
static const T TAU() { return (T)(2 * acos(-1)); }
