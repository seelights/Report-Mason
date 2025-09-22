//========================================================================
//
// SplashClip.cc
//
// Clip implementation for Splash graphics library
//
// This file is licensed under the GPLv2 or later
//
//========================================================================

#include "SplashClip.h"
#include "SplashPath.h"
#include "SplashBitmap.h"

SplashClip::SplashClip(SplashCoord x0, SplashCoord y0, SplashCoord x1, SplashCoord y1, bool antialiasA)
    : antialias(antialiasA), xMin(x0), yMin(y0), xMax(x1), yMax(y1),
      xMinI((int)floor(x0)), yMinI((int)floor(y0)), xMaxI((int)ceil(x1)), yMaxI((int)ceil(y1))
{
}

SplashClip::SplashClip(const SplashClip *clip)
    : antialias(clip->antialias), xMin(clip->xMin), yMin(clip->yMin), xMax(clip->xMax), yMax(clip->yMax),
      xMinI(clip->xMinI), yMinI(clip->yMinI), xMaxI(clip->xMaxI), yMaxI(clip->yMaxI)
{
    // Copy paths if any
}

SplashClip::~SplashClip()
{
    // Cleanup
}

void SplashClip::resetToRect(SplashCoord x0, SplashCoord y0, SplashCoord x1, SplashCoord y1)
{
    xMin = x0;
    yMin = y0;
    xMax = x1;
    yMax = y1;
    xMinI = (int)floor(x0);
    yMinI = (int)floor(y0);
    xMaxI = (int)ceil(x1);
    yMaxI = (int)ceil(y1);
}

SplashError SplashClip::clipToRect(SplashCoord x0, SplashCoord y0, SplashCoord x1, SplashCoord y1)
{
    if (x0 > xMin) xMin = x0;
    if (y0 > yMin) yMin = y0;
    if (x1 < xMax) xMax = x1;
    if (y1 < yMax) yMax = y1;
    
    xMinI = (int)floor(xMin);
    yMinI = (int)floor(yMin);
    xMaxI = (int)ceil(xMax);
    yMaxI = (int)ceil(yMax);
    
    return splashOk;
}

SplashError SplashClip::clipToPath(SplashPath *path, SplashCoord *matrix, SplashCoord flatness, bool eo)
{
    // Stub implementation
    return splashOk;
}

SplashClipResult SplashClip::testRect(int rectXMin, int rectYMin, int rectXMax, int rectYMax)
{
    if (rectXMax < xMinI || rectXMin > xMaxI || rectYMax < yMinI || rectYMin > yMaxI) {
        return splashClipAllOutside;
    }
    if (rectXMin >= xMinI && rectXMax <= xMaxI && rectYMin >= yMinI && rectYMax <= yMaxI) {
        return splashClipAllInside;
    }
    return splashClipPartial;
}

SplashClipResult SplashClip::testSpan(int spanXMin, int spanXMax, int spanY)
{
    if (spanXMax < xMinI || spanXMin > xMaxI || spanY < yMinI || spanY > yMaxI) {
        return splashClipAllOutside;
    }
    if (spanXMin >= xMinI && spanXMax <= xMaxI && spanY >= yMinI && spanY <= yMaxI) {
        return splashClipAllInside;
    }
    return splashClipPartial;
}

void SplashClip::clipAALine(SplashBitmap *aaBuf, int *x0, int *x1, int y, bool adjustVertLine)
{
    // Stub implementation
}

bool SplashClip::testClipPaths(int x, int y)
{
    // Stub implementation - assume all paths are inside
    return true;
}
