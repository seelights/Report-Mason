/*
 * poppler-config.h
 * Poppler Configuration Header
 */

#ifndef POPPLER_CONFIG_H
#define POPPLER_CONFIG_H

// Version information
#define POPPLER_VERSION "24.08.0"
#define POPPLER_VERSION_MAJOR 24
#define POPPLER_VERSION_MINOR 8
#define POPPLER_VERSION_MICRO 0

// Feature detection - all optional libraries disabled
#undef HAVE_CAIRO
#define HAVE_CAIRO 0
#undef HAVE_FREETYPE
#define HAVE_FREETYPE 0
#undef HAVE_OPENJPEG
#define HAVE_OPENJPEG 0
#undef HAVE_JPEG
#define HAVE_JPEG 0
#undef HAVE_PNG
#define HAVE_PNG 0
#undef HAVE_TIFF
#define HAVE_TIFF 0
#undef HAVE_NSS3
#define HAVE_NSS3 0
#undef HAVE_GPGME
#define HAVE_GPGME 0
#undef HAVE_CURL
#define HAVE_CURL 0
#undef HAVE_FT2BUILD_H
#define HAVE_FT2BUILD_H 0
#undef HAVE_JPEGLIB_H
#define HAVE_JPEGLIB_H 0

// Platform specific
#ifdef _WIN32
#define POPPLER_WIN32 1
#endif

// GCC printf format checking
#ifdef __GNUC__
#define GCC_PRINTF_FORMAT(fmt_index, va_index) \
    __attribute__((__format__(__printf__, fmt_index, va_index)))
#else
#define GCC_PRINTF_FORMAT(fmt_index, va_index)
#endif

#endif // POPPLER_CONFIG_H
