//========================================================================
//
// SplashTypes.h
//
// Type definitions for Splash graphics library
//
// This file is licensed under the GPLv2 or later
//
//========================================================================

#ifndef SPLASHTYPES_H
#define SPLASHTYPES_H

#include "config.h"
#include <cstring>
#include <cmath>
#include <string>

//------------------------------------------------------------------------
// antialiasing
//------------------------------------------------------------------------

#define splashAASize 4

#ifndef SPOT_NCOMPS
#    define SPOT_NCOMPS 4
#endif

// max number of components in any SplashColor
constexpr std::size_t splashMaxColorComps = SPOT_NCOMPS + 4;

// Basic types
typedef unsigned char SplashColorComp;
typedef unsigned char* SplashColorPtr;
typedef unsigned char SplashColor[splashMaxColorComps];
typedef const unsigned char* SplashColorConstPtr;
typedef double SplashCoord;

// Color mode component counts
extern int splashColorModeNComps[];

// BGR8 color component access macros
#define splashBGR8R(p) ((p)[2])
#define splashBGR8G(p) ((p)[1])
#define splashBGR8B(p) ((p)[0])

// Utility macros
#define splashAvg(a, b) (((a) + (b)) / 2)
#define splashDist(x0, y0, x1, y1) sqrt(((x1) - (x0)) * ((x1) - (x0)) + ((y1) - (y0)) * ((y1) - (y0)))
// splashFontFraction macro is defined in SplashFont.h
#define splashFontFractionScale 64
#define splashCheckDet(m00, m01, m10, m11, epsilon) (fabs((m00) * (m11) - (m01) * (m10)) > (epsilon))
#define splashColorEqual(c0, c1) (memcmp((c0), (c1), splashMaxColorComps) == 0)
#define splashClearColor(c) memset((c), 0, splashMaxColorComps)
#define splashColorCopy(c0, c1) memcpy((c0), (c1), splashMaxColorComps)
// Thin line mode constants
#define splashThinLineShape splashThinLineSolid

// Path flags
#define splashPathFirst 0x01
#define splashPathLast 0x02
#define splashPathCurve 0x04
#define splashPathClosed 0x08

// Color modes
enum SplashColorMode {
    splashModeMono1, // 1 bit per component, 8 pixels per byte
    splashModeMono8, // 8 bits per component, 1 byte per pixel
    splashModeRGB8,  // 8 bits per component, 3 bytes per pixel
    splashModeBGR8,  // 8 bits per component, 3 bytes per pixel (BGR order)
    splashModeXBGR8, // 8 bits per component, 4 bytes per pixel (XBGR order)
    splashModeCMYK8, // 8 bits per component, 4 bytes per pixel
    splashModeDeviceN8 // 8 bits per component, DeviceN color space
};

// Line cap styles
enum SplashLineCap { splashLineCapButt, splashLineCapRound, splashLineCapProjecting };

// Line join styles
enum SplashLineJoin { splashLineJoinMiter, splashLineJoinRound, splashLineJoinBevel };

// Fill rules
enum SplashFillRule { splashFillEvenOdd, splashFillWinding };

// Thin line modes
enum SplashThinLineMode { splashThinLineDefault, splashThinLineSolid, splashThinLineDashed };

// Screen function types
enum SplashFunctionType {
    splashScreenDispersed,
    splashScreenClustered,
    splashScreenStochasticClustered
};

// Screen parameters
struct SplashScreenParams
{
    int size;
    int dotRadius;
    SplashFunctionType function;
    void* funcData;
    double gamma;
    double blackThreshold;
    double whiteThreshold;
    int type;
};

// Image file formats
enum SplashImageFileFormat {
    splashFormatPBM,
    splashFormatPGM,
    splashFormatPPM,
    splashFormatBMP8,
    splashFormatBMP24,
    splashFormatBMP32,
    splashFormatJpeg,
    splashFormatJpegCMYK,
    splashFormatPng,
    splashFormatTiff,
    splashFormatTiffSep
};

// Write image parameters
struct WriteImgParams
{
    int quality;
    int progressive;
    int tiffCompression;
    int tiffPredictor;
    int jpegOptimize;
    int jpegProgressive;
    int jpegQuality;
    int pngCompression;
    int pngInterlaced;
    int bmpCompression;
    int bmpRle;
};

// Conversion modes
enum ConversionMode { conversionModeDefault, conversionModeRGB, conversionModeCMYK };

// Blend function pointer type
typedef void (*SplashBlendFunc)(SplashColorPtr cSrc, SplashColorPtr cDest, SplashColorPtr cBlend, SplashColorMode mode);

// Blend function constants
enum SplashBlendFuncType {
    splashBlendNone = 0,
    splashBlendNormal,
    splashBlendMultiply,
    splashBlendScreen,
    splashBlendOverlay,
    splashBlendSoftLight,
    splashBlendHardLight,
    splashBlendColorDodge,
    splashBlendColorBurn,
    splashBlendDarken,
    splashBlendLighten,
    splashBlendDifference,
    splashBlendExclusion
};

// Image source types
enum SplashImageSourceType { splashImageSrcFile, splashImageSrcData };

// Font source type - forward declaration only
class SplashFontSrc;

// Error codes
enum SplashError {
    splashOk = 0,
    splashErrNoCurPt = 1,
    splashErrEmptyPath = 2,
    splashErrBogusPath = 3,
    splashErrNoSave = 4,
    splashErrOpenFile = 5,
    splashErrNoGlyph = 6,
    splashErrModeMismatch = 7,
    splashErrSingularMatrix = 8,
    splashErrBadArg = 9,
    splashErrZeroImage = 254,
    splashErrGeneric = 255
};

#endif // SPLASHTYPES_H
