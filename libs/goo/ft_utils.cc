//========================================================================
//
// ft_utils.cc
//
// FreeType helper functions.
//
// This file is licensed under the GPLv2 or later
//
// Copyright (C) 2022 Adrian Johnson <ajohnson@redneon.com>
//
//========================================================================

#include <config.h>

#include "ft_utils.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <vector>
#endif

// Same as FT_New_Face() but handles UTF-8 filenames on Windows
FT_Error ft_new_face_from_file(FT_Library library, const char *filename_utf8, FT_Long face_index, FT_Face *aface)
{
#ifdef _WIN32
    // Convert UTF-8 to wide string for Windows
    int wlen = MultiByteToWideChar(CP_UTF8, 0, filename_utf8, -1, nullptr, 0);
    if (wlen == 0) {
        return FT_Err_Cannot_Open_Resource;
    }
    
    std::vector<wchar_t> wfilename(wlen);
    MultiByteToWideChar(CP_UTF8, 0, filename_utf8, -1, wfilename.data(), wlen);
    
    // Convert wide string back to UTF-8 for FreeType
    int utf8len = WideCharToMultiByte(CP_UTF8, 0, wfilename.data(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8len == 0) {
        return FT_Err_Cannot_Open_Resource;
    }
    
    std::vector<char> utf8filename(utf8len);
    WideCharToMultiByte(CP_UTF8, 0, wfilename.data(), -1, utf8filename.data(), utf8len, nullptr, nullptr);
    
    // Use the UTF-8 filename with FreeType
    return FT_New_Face(library, utf8filename.data(), face_index, aface);
#else
    // On non-Windows systems, just use the standard function
    return FT_New_Face(library, filename_utf8, face_index, aface);
#endif
}
