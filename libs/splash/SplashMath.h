//========================================================================
//
// SplashMath.h
//
// Math utilities for Splash graphics library
//
// This file is licensed under the GPLv2 or later
//
//========================================================================

#ifndef SPLASHMATH_H
#define SPLASHMATH_H

#include <cmath>

// Math utility functions
inline double splashAbs(double x) { return fabs(x); }
inline int splashAbs(int x) { return x < 0 ? -x : x; }

inline double splashSqrt(double x) { return sqrt(x); }
inline double splashPow(double x, double y) { return pow(x, y); }
inline double splashSin(double x) { return sin(x); }
inline double splashCos(double x) { return cos(x); }
inline double splashTan(double x) { return tan(x); }
inline double splashAtan2(double y, double x) { return atan2(y, x); }

inline double splashFloor(double x) { return floor(x); }
inline double splashCeil(double x) { return ceil(x); }
inline double splashRound(double x) { return floor(x + 0.5); }

// Constants
#define splashSqrt2 1.41421356237309504880
#define splashPI 3.14159265358979323846

#endif // SPLASHMATH_H
