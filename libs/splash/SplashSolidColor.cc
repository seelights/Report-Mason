//========================================================================
//
// SplashSolidColor.cc
//
//========================================================================

#include "SplashPattern.h"

SplashSolidColor::SplashSolidColor(SplashColorConstPtr colorA)
{
    for (int i = 0; i < splashColorModeNComps[splashModeRGB8]; i++) {
        color[i] = colorA[i];
    }
}

SplashSolidColor::~SplashSolidColor()
{
}

bool SplashSolidColor::getColor(int x, int y, SplashColorPtr c)
{
    for (int i = 0; i < splashColorModeNComps[splashModeRGB8]; i++) {
        c[i] = color[i];
    }
    return true;
}
