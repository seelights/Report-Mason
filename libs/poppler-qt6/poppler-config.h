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

// Feature detection
#define HAVE_CAIRO 1
#define HAVE_FREETYPE 1
#define HAVE_OPENJPEG 1
#define HAVE_JPEG 1
#define HAVE_PNG 1
#define HAVE_TIFF 1
#define HAVE_NSS3 0
#define HAVE_GPGME 0

// Platform specific
#ifdef _WIN32
#define POPPLER_WIN32 1
#endif

#endif // POPPLER_CONFIG_H
