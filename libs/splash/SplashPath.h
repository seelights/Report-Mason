/*
 * @Author: seelights
 * @Date: 2025-09-21 11:13:58
 * @LastEditTime: 2025-09-21 18:22:02
 * @LastEditors: seelights
 * @Description: 
 * @FilePath: \ReportMason\libs\splash\SplashPath.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */
//========================================================================
//
// SplashPath.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2009, 2011, 2012, 2013, 2016, 2017, 2018, 2019, 2020, 2021, 2022 Oliver Sander <oliver.sander@tu-dresden.de>
// Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021, 2022 Adrian Johnson <ajohnson@redneon.com>
// Copyright (C) 2012 Fabio D'Urso <fabiodurso@hotmail.it>
// Copyright (C) 2018 Adam Reichold <adam.reichold@t-online.de>
// Copyright (C) 2019, 2020, 2021, 2022, 2023, 2024 g10 Code GmbH, Author: Sune Stolborg Vuorela <sune@vuorela.dk>
// Copyright (C) 2021 Hubert Figuiere <hub@figuiere.net>
// Copyright (C) 2021 Christian Persch <chpe@src.gnome.org>
// Copyright (C) 2021 Georgiy Sgibnev <georgiy@sgibnev.com>. Work sponsored by lab50.net.
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef SPLASHPATH_H
#define SPLASHPATH_H

#include "SplashTypes.h"
#include "poppler_private_export.h"

#include <vector>

//------------------------------------------------------------------------
// SplashPath
//------------------------------------------------------------------------

// SplashPathPoint structure
struct SplashPathPoint
{
    SplashCoord x, y;
    int curve; // 0 = line, 1 = curve, 2 = close
};

class POPPLER_PRIVATE_EXPORT SplashPath
{
public:
    // Create a new path.
    SplashPath();
    
    // Copy constructor
    SplashPath(const SplashPath& other);
    
    // Copy a path.
    SplashPath *copy() const;
    
    ~SplashPath();
    
    SplashPath &operator=(const SplashPath &) = delete;
    
    // Append <path> to <this>.
    void append(SplashPath *path);
    
    // Start a new subpath at (<x>, <y>).
    SplashError moveTo(SplashCoord x, SplashCoord y);
    
    // Add a line segment from the current point to (<x>, <y>).
    void lineTo(SplashCoord x, SplashCoord y);
    
    // Add a third-order Bezier curve from the current point to
    // (<x3>, <y3>), using (<x1>, <y1>) and (<x2>, <y2>) as the
    // control points.
    void curveTo(SplashCoord x1, SplashCoord y1, SplashCoord x2, SplashCoord y2, SplashCoord x3, SplashCoord y3);
    
    // Close the current subpath by adding a line segment back to
    // the starting point.
    void close(bool force = false);
    
    // Add a rectangle to the path.
    void rect(SplashCoord x, SplashCoord y, SplashCoord w, SplashCoord h);
    
    // Get the number of points in the path.
    int getLength() const { return length; }
    
    // Get the coordinates of the <i>-th point.
    SplashCoord getX(int i) const { return pts[i].x; }
    SplashCoord getY(int i) const { return pts[i].y; }
    
    // Get the type of the <i>-th point.
    int getCurve(int i) const { return pts[i].curve; }
    
    // Get the number of subpaths.
    int getNumSubpaths() const { return numSubpaths; }
    
    // Get the starting point of the <i>-th subpath.
    int getSubpathStart(int i) const { return subpaths[i]; }
    
    // Get the number of points in the <i>-th subpath.
    int getSubpathLength(int i) const;
    
    // Get the bounding box of the path.
    void getBBox(SplashCoord *xMin, SplashCoord *yMin, SplashCoord *xMax, SplashCoord *yMax);
    
    // Transform the path by the given matrix.
    void transform(SplashCoord *matrix);
    
    // Get the winding rule.
    int getWindingRule() const { return windingRule; }
    
    // Set the winding rule.
    void setWindingRule(int windingRuleA) { windingRule = windingRuleA; }
    
    // Get the flags array (for internal use)
    int* getFlags() { return flags; }
    const int* getFlags() const { return flags; }
    
    // Get the points array (for internal use)
    SplashPathPoint* getPts() { return pts; }
    const SplashPathPoint* getPts() const { return pts; }
    
    void* getHints() { return hints; }
    const void* getHints() const { return hints; }
    
    // Add stroke adjustment hint
    void addStrokeAdjustHint(int left, int right, int start, int end);
    
    // Reserve space for points
    void reserve(int size);
    
    // Offset the path
    void offset(SplashCoord dx, SplashCoord dy);
    
    
private:
    
    SplashPathPoint *pts;
    int *flags;
    int length, size;
    void *hints;
    int *subpaths;
    int numSubpaths, subpathSize;
    int windingRule;
    
    void grow(int nPts);
    void growSubpaths(int nSubpaths);
};

#endif
