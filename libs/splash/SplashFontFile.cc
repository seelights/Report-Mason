//========================================================================
//
// SplashFontFile.cc
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2006 Takashi Iwai <tiwai@suse.de>
// Copyright (C) 2008, 2022 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2019 Christian Persch <chpe@src.gnome.org>
// Copyright (C) 2022 Oliver Sander <oliver.sander@tu-dresden.de>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#include <config.h>

#include <cstdio>
#include <cstring>
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif
#include "goo/gmem.h"
#include "goo/GooString.h"
#include "SplashFontFile.h"
#include "SplashFontFileID.h"

//------------------------------------------------------------------------
// SplashFontFile
//------------------------------------------------------------------------

SplashFontFile::SplashFontFile(std::unique_ptr<SplashFontFileID> idA, SplashFontSrc *srcA)
{
    id = std::move(idA);
    src = srcA;
    src->ref();
    refCnt = 0;
    doAdjustMatrix = false;
}

SplashFontFile::~SplashFontFile()
{
    src->unref();
    // id is managed by unique_ptr, no need to delete manually
}

void SplashFontFile::incRefCnt()
{
    ++refCnt;
}

void SplashFontFile::decRefCnt()
{
    if (!--refCnt) {
        delete this;
    }
}

//

SplashFontSrc::SplashFontSrc()
{
    isFileFlag = false;
    refcnt = 1;
}

SplashFontSrc::~SplashFontSrc() = default;

void SplashFontSrc::ref()
{
    refcnt++;
}

void SplashFontSrc::unref()
{
    if (!--refcnt) {
        delete this;
    }
}

void SplashFontSrc::setFile(const std::string &file)
{
    isFileFlag = true;
    fileName = file;
}

void SplashFontSrc::setBuf(std::vector<unsigned char> &&bufA)
{
    isFileFlag = false;
    buf = std::move(bufA);
}

void SplashFontSrc::setBuf(char *bufA, int buflenA)
{
    isFileFlag = false;
    buf.resize(buflenA);
    std::memcpy(buf.data(), bufA, buflenA);
}

void SplashFontSrc::setBuf(unsigned char *bufA, int buflenA)
{
    isFileFlag = false;
    buf.resize(buflenA);
    std::memcpy(buf.data(), bufA, buflenA);
}
