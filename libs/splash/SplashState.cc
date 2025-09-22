//========================================================================
//
// SplashState.cc
//
// State implementation for Splash graphics library
//
// This file is licensed under the GPLv2 or later
//
//========================================================================

#include "SplashState.h"
#include "SplashPath.h"
#include "SplashPattern.h"
#include "SplashScreen.h"
#include "SplashClip.h"
#include "SplashBitmap.h"

SplashState::SplashState(int width, int height, bool vectorAntialias, SplashScreenParams *screenParams)
    : strokePattern(nullptr),
      fillPattern(nullptr),
      screen(nullptr),
      blendFunc(nullptr),
      strokeAlpha(1),
      fillAlpha(1),
      multiplyPatternAlpha(false),
      patternStrokeAlpha(1),
      patternFillAlpha(1),
      lineWidth(1),
      lineCap(splashLineCapButt),
      lineJoin(splashLineJoinMiter),
      miterLimit(10),
      flatness(1),
      lineDashPhase(0),
      strokeAdjust(false),
      clip(nullptr),
      softMask(nullptr),
      deleteSoftMask(false),
      inNonIsolatedGroup(false),
      fillOverprint(false),
      strokeOverprint(false),
      overprintMode(0),
      overprintMask(0xffffffff),
      overprintAdditive(false),
      next(nullptr)
{
    // Initialize matrix to identity
    matrix[0] = 1; matrix[1] = 0; matrix[2] = 0;
    matrix[3] = 1; matrix[4] = 0; matrix[5] = 0;
    
    // Initialize transfer functions to identity
    for (int i = 0; i < 256; ++i) {
        rgbTransferR[i] = rgbTransferG[i] = rgbTransferB[i] = i;
        grayTransfer[i] = i;
        cmykTransferC[i] = cmykTransferM[i] = cmykTransferY[i] = cmykTransferK[i] = i;
    }
    for (int i = 0; i < SPOT_NCOMPS + 4; ++i) {
        for (int j = 0; j < 256; ++j) {
            deviceNTransfer[i][j] = j;
        }
    }
}

SplashState::SplashState(int width, int height, bool vectorAntialias, SplashScreen *screenA)
    : strokePattern(nullptr),
      fillPattern(nullptr),
      screen(screenA),
      blendFunc(nullptr),
      strokeAlpha(1),
      fillAlpha(1),
      multiplyPatternAlpha(false),
      patternStrokeAlpha(1),
      patternFillAlpha(1),
      lineWidth(1),
      lineCap(splashLineCapButt),
      lineJoin(splashLineJoinMiter),
      miterLimit(10),
      flatness(1),
      lineDashPhase(0),
      strokeAdjust(false),
      clip(nullptr),
      softMask(nullptr),
      deleteSoftMask(false),
      inNonIsolatedGroup(false),
      fillOverprint(false),
      strokeOverprint(false),
      overprintMode(0),
      overprintMask(0xffffffff),
      overprintAdditive(false),
      next(nullptr)
{
    // Initialize matrix to identity
    matrix[0] = 1; matrix[1] = 0; matrix[2] = 0;
    matrix[3] = 1; matrix[4] = 0; matrix[5] = 0;
    
    // Initialize transfer functions to identity
    for (int i = 0; i < 256; ++i) {
        rgbTransferR[i] = rgbTransferG[i] = rgbTransferB[i] = i;
        grayTransfer[i] = i;
        cmykTransferC[i] = cmykTransferM[i] = cmykTransferY[i] = cmykTransferK[i] = i;
    }
    for (int i = 0; i < SPOT_NCOMPS + 4; ++i) {
        for (int j = 0; j < 256; ++j) {
            deviceNTransfer[i][j] = j;
        }
    }
}

SplashState::SplashState(const SplashState *state)
    : strokePattern(state->strokePattern),
      fillPattern(state->fillPattern),
      screen(state->screen),
      blendFunc(state->blendFunc),
      strokeAlpha(state->strokeAlpha),
      fillAlpha(state->fillAlpha),
      multiplyPatternAlpha(state->multiplyPatternAlpha),
      patternStrokeAlpha(state->patternStrokeAlpha),
      patternFillAlpha(state->patternFillAlpha),
      lineWidth(state->lineWidth),
      lineCap(state->lineCap),
      lineJoin(state->lineJoin),
      miterLimit(state->miterLimit),
      flatness(state->flatness),
      lineDash(state->lineDash),
      lineDashPhase(state->lineDashPhase),
      strokeAdjust(state->strokeAdjust),
      clip(state->clip),
      softMask(state->softMask),
      deleteSoftMask(false),
      inNonIsolatedGroup(state->inNonIsolatedGroup),
      fillOverprint(state->fillOverprint),
      strokeOverprint(state->strokeOverprint),
      overprintMode(state->overprintMode),
      overprintMask(state->overprintMask),
      overprintAdditive(state->overprintAdditive),
      next(nullptr)
{
    // Copy matrix
    for (int i = 0; i < 6; ++i) {
        matrix[i] = state->matrix[i];
    }
    
    // Copy transfer functions
    for (int i = 0; i < 256; ++i) {
        rgbTransferR[i] = state->rgbTransferR[i];
        rgbTransferG[i] = state->rgbTransferG[i];
        rgbTransferB[i] = state->rgbTransferB[i];
        grayTransfer[i] = state->grayTransfer[i];
        cmykTransferC[i] = state->cmykTransferC[i];
        cmykTransferM[i] = state->cmykTransferM[i];
        cmykTransferY[i] = state->cmykTransferY[i];
        cmykTransferK[i] = state->cmykTransferK[i];
    }
    for (int i = 0; i < SPOT_NCOMPS + 4; ++i) {
        for (int j = 0; j < 256; ++j) {
            deviceNTransfer[i][j] = state->deviceNTransfer[i][j];
        }
    }
}

SplashState::~SplashState()
{
    // Cleanup
}

void SplashState::setStrokePattern(SplashPattern *strokePatternA)
{
    strokePattern = strokePatternA;
}

void SplashState::setFillPattern(SplashPattern *fillPatternA)
{
    fillPattern = fillPatternA;
}

void SplashState::setScreen(SplashScreen *screenA)
{
    screen = screenA;
}

void SplashState::setLineDash(std::vector<SplashCoord> &&lineDashA, SplashCoord lineDashPhaseA)
{
    lineDash = std::move(lineDashA);
    lineDashPhase = lineDashPhaseA;
}

void SplashState::setSoftMask(SplashBitmap *softMaskA)
{
    softMask = softMaskA;
}

void SplashState::setTransfer(unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char *gray)
{
    if (red) {
        for (int i = 0; i < 256; ++i) {
            rgbTransferR[i] = red[i];
        }
    }
    if (green) {
        for (int i = 0; i < 256; ++i) {
            rgbTransferG[i] = green[i];
        }
    }
    if (blue) {
        for (int i = 0; i < 256; ++i) {
            rgbTransferB[i] = blue[i];
        }
    }
    if (gray) {
        for (int i = 0; i < 256; ++i) {
            grayTransfer[i] = gray[i];
        }
    }
}
