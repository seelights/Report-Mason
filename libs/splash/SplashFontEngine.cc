//========================================================================
//
// SplashFontEngine.cc
//
// Font engine implementation for Splash graphics library
//
// This file is licensed under the GPLv2 or later
//
//========================================================================

#include "SplashFontEngine.h"
#include "SplashFontFile.h"
#include "SplashFontFileID.h"
#include "SplashFont.h"

SplashFontEngine::SplashFontEngine(bool enableFreeType, bool enableFreeTypeHinting, bool enableSlightHinting, bool aa)
{
    // Initialize font cache
    for (int i = 0; i < 16; i++) {
        fontCache[i] = nullptr;
    }
    
    // Initialize FreeType engine (stub for now)
    ftEngine = nullptr;
}

SplashFontEngine::~SplashFontEngine()
{
    // Clean up font cache
    for (int i = 0; i < 16; i++) {
        if (fontCache[i]) {
            delete fontCache[i];
        }
    }
    
    // Clean up FreeType engine (stub - no actual cleanup needed)
    // ftEngine is nullptr, so no deletion needed
}

SplashFontFile* SplashFontEngine::getFontFile(SplashFontFileID* id)
{
    // Search font cache for matching font file
    for (int i = 0; i < 16; i++) {
        if (fontCache[i]) {
            SplashFontFile* fontFile = fontCache[i]->getFontFile();
            const SplashFontFileID& fontFileID = fontFile->getID();
            if (fontFile && const_cast<SplashFontFileID&>(fontFileID).matches(const_cast<SplashFontFileID*>(id))) {
                return fontFile;
            }
        }
    }
    return nullptr;
}

SplashFont* SplashFontEngine::getFont(SplashFontFile* fontFile, const SplashCoord* textMat, const SplashCoord* ctm)
{
    // Search font cache for matching font
    for (int i = 0; i < 16; i++) {
        if (fontCache[i] && fontCache[i]->matches(fontFile, textMat, ctm)) {
            return fontCache[i];
        }
    }
    
    // Create new font and add to cache
    SplashFont* font = fontFile->makeFont(const_cast<SplashCoord*>(textMat), ctm);
    if (font) {
        // Find empty slot in cache
        for (int i = 0; i < 16; i++) {
            if (!fontCache[i]) {
                fontCache[i] = font;
                break;
            }
        }
    }
    
    return font;
}

bool SplashFontEngine::getAA()
{
    // Return anti-aliasing setting (stub implementation)
    return false;
}

void SplashFontEngine::setAA(bool aa)
{
    // Set anti-aliasing (stub implementation)
    // No-op for now
}

SplashFontFile* SplashFontEngine::loadType1Font(SplashFontFileID* idA, SplashFontSrc* src, const char** enc)
{
    // Stub implementation
    return nullptr;
}

SplashFontFile* SplashFontEngine::loadType1CFont(SplashFontFileID* idA, SplashFontSrc* src, const char** enc)
{
    // Stub implementation
    return nullptr;
}

SplashFontFile* SplashFontEngine::loadOpenTypeT1CFont(SplashFontFileID* idA, SplashFontSrc* src, const char** enc)
{
    // Stub implementation
    return nullptr;
}

SplashFontFile* SplashFontEngine::loadCIDFont(SplashFontFileID* idA, SplashFontSrc* src)
{
    // Stub implementation
    return nullptr;
}

SplashFontFile* SplashFontEngine::loadOpenTypeCFFFont(SplashFontFileID* idA, SplashFontSrc* src, int* codeToGID, int codeToGIDLen)
{
    // Stub implementation
    return nullptr;
}

SplashFontFile* SplashFontEngine::loadTrueTypeFont(SplashFontFileID* idA, SplashFontSrc* src, int* codeToGID, int codeToGIDLen, int faceIndex)
{
    // Stub implementation
    return nullptr;
}
