/* config.h - Simplified configuration for ReportMason */

#ifndef CONFIG_H
#define CONFIG_H

/* Basic feature definitions */
#define ENABLE_LIBCURL 1
#define ENABLE_LIBJPEG 1
#define ENABLE_LIBOPENJPEG 1
#define ENABLE_LIBTIFF 1
#define ENABLE_LIBPNG 1
#define ENABLE_ZLIB_UNCOMPRESS 1
#define ENABLE_SIGNATURES 1
#define HAVE_CAIRO 1
#define HAVE_DCT_DECODER 1
#define HAVE_JPX_DECODER 1

/* System headers */
#define HAVE_DIRENT_H 1
#define HAVE_DLFCN_H 1
#define HAVE_FCNTL_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1

/* Compiler features */
#define HAVE_STD_IS_TRIVIALLY_COPYABLE 1
#define HAVE_STD_IS_TRIVIALLY_DESTRUCTIBLE 1

/* Math functions */
#define HAVE_ATAN2 1
#define HAVE_CEIL 1
#define HAVE_FLOOR 1
#define HAVE_FMOD 1
#define HAVE_FREXP 1
#define HAVE_ISNAN 1
#define HAVE_ISINF 1
#define HAVE_MODF 1
#define HAVE_POW 1
#define HAVE_RINT 1
#define HAVE_SQRT 1

/* String functions */
#define HAVE_STRCHR 1
#define HAVE_STRCSPN 1
#define HAVE_STRPBRK 1
#define HAVE_STRRCHR 1
#define HAVE_STRSPN 1
#define HAVE_STRSTR 1
#define HAVE_STRTOK_R 1

/* Memory functions */
#define HAVE_MEMSET 1

/* File operations */
#define HAVE_FSEEKO 1
#define HAVE_FTELLO 1

/* Threading */
#define HAVE_PTHREAD 1

/* Version information */
#define POPPLER_VERSION "24.08.0"
#define POPPLER_VERSION_MAJOR 24
#define POPPLER_VERSION_MINOR 8
#define POPPLER_VERSION_MICRO 0

/* Default signature backend */
#define DEFAULT_SIGNATURE_BACKEND "NSS3"

/* Architecture */
#define SIZEOF_INT 4
#define SIZEOF_LONG 8
#define SIZEOF_LONG_LONG 8
#define SIZEOF_SIZE_T 8
#define SIZEOF_UNSIGNED_INT 4
#define SIZEOF_UNSIGNED_LONG 8
#define SIZEOF_UNSIGNED_LONG_LONG 8

/* Endianness */
#define WORDS_BIGENDIAN 0

/* Windows specific */
#ifdef _WIN32
#define HAVE_WIN32 1
#define HAVE_WIN32_CODECS 1
#endif

/* Optional libraries - disabled for now */
#define HAVE_FT2BUILD_H 0
#define HAVE_JPEGLIB_H 0
#define HAVE_FREETYPE 0
#define HAVE_JPEG 0
#define HAVE_PNG 0
#define HAVE_TIFF 0
#define HAVE_OPENJPEG 0

/* Text output features */
#define TEXTOUT_WORD_LIST 1

/* GCC printf format checking */
#ifdef __GNUC__
#define GCC_PRINTF_FORMAT(fmt_index, va_index) \
    __attribute__((__format__(__printf__, fmt_index, va_index)))
#else
#define GCC_PRINTF_FORMAT(fmt_index, va_index)
#endif

#endif /* CONFIG_H */