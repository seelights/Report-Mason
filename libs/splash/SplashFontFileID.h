/*
 * SplashFontFileID.h
 * Font file ID interface for ReportMason project
 * This is a minimal implementation to resolve compilation issues
 */

#ifndef SPLASHFONTFILEID_H
#define SPLASHFONTFILEID_H

class SplashFontFileID {
public:
    SplashFontFileID();
    virtual ~SplashFontFileID();
    
    // Comparison operators
    virtual bool matches(SplashFontFileID* id) = 0;
    virtual int hash() = 0;
};

#endif // SPLASHFONTFILEID_H
