//========================================================================
//
// SplashScreen.cc
//
//========================================================================

#include "SplashScreen.h"

SplashScreen::SplashScreen(const SplashScreenParams *params)
{
    screenParams = params;
    mat = nullptr;
    size = params->size;
    sizeM1 = size - 1;
    log2Size = 0;
    int s = size;
    while (s > 1) {
        s >>= 1;
        log2Size++;
    }
    minVal = params->blackThreshold;
    maxVal = params->whiteThreshold;
}

SplashScreen::SplashScreen(const SplashScreen *screen)
{
    screenParams = screen->screenParams;
    mat = nullptr;
    size = screen->size;
    sizeM1 = screen->sizeM1;
    log2Size = screen->log2Size;
    minVal = screen->minVal;
    maxVal = screen->maxVal;
}

SplashScreen::~SplashScreen()
{
    if (mat) {
        delete[] mat;
    }
}

void SplashScreen::createMatrix()
{
    if (mat) {
        delete[] mat;
    }
    mat = new unsigned char[size * size];
    
    // Simple implementation - create a basic threshold matrix
    for (int i = 0; i < size * size; i++) {
        mat[i] = (i * 255) / (size * size);
    }
}