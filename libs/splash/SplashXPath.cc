//========================================================================
//
// SplashXPath.cc
//
// XPath implementation for Splash graphics library
//
// This file is licensed under the GPLv2 or later
//
//========================================================================

#include "SplashXPath.h"
#include "SplashPath.h"

SplashXPath::SplashXPath(SplashPath *path, SplashCoord *matrix, SplashCoord flatness, bool closeSubpaths, bool adjustLines, int linePosI)
    : segs(nullptr), length(0), size(0)
{
    // Stub implementation
}

SplashXPath::~SplashXPath()
{
    if (segs) {
        delete[] segs;
    }
}

void SplashXPath::aaScale()
{
    // Stub implementation
}

void SplashXPath::sort()
{
    // Stub implementation
}

void SplashXPath::transform(SplashCoord *matrix, SplashCoord xi, SplashCoord yi, SplashCoord *xo, SplashCoord *yo)
{
    *xo = matrix[0] * xi + matrix[2] * yi + matrix[4];
    *yo = matrix[1] * xi + matrix[3] * yi + matrix[5];
}

void SplashXPath::strokeAdjust(SplashXPathAdjust *adjust, SplashCoord *xp, SplashCoord *yp)
{
    // Stub implementation
}

void SplashXPath::grow(int nSegs)
{
    // Stub implementation
}

void SplashXPath::addCurve(SplashCoord x0, SplashCoord y0, SplashCoord x1, SplashCoord y1, SplashCoord x2, SplashCoord y2, SplashCoord x3, SplashCoord y3, SplashCoord flatness, bool first, bool last, bool end0, bool end1)
{
    // Stub implementation
}

void SplashXPath::addSegment(SplashCoord x0, SplashCoord y0, SplashCoord x1, SplashCoord y1)
{
    // Stub implementation
}
