/*
 * @Author: seelights
 * @Date: 2025-09-21 14:40:34
 * @LastEditTime: 2025-09-21 16:53:43
 * @LastEditors: seelights
 * @Description: 
 * @FilePath: \ReportMason\libs\jconfig.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */
/*
 * jconfig.h
 * 
 * JPEG configuration definitions
 * This is a minimal implementation for ReportMason project
 */

#ifndef JCONFIG_H
#define JCONFIG_H

/* Basic configuration */
#define JPEG_LIB_VERSION 80

/* Data types */
typedef unsigned char JSAMPLE;
typedef short JCOEF;
typedef int JDIFF;
typedef unsigned char JOCTET;
typedef unsigned int JDIMENSION;
#ifdef _WIN32
// Avoid conflict with Windows system headers
#ifndef JPEG_BOOLEAN_DEFINED
#define JPEG_BOOLEAN_DEFINED
// Use unsigned char to match Windows system headers
typedef unsigned char boolean;
#endif
#else
#ifndef boolean
typedef int boolean;
#endif
#endif

/* JPEG constants */
#define DCTSIZE 8
#define DCTSIZE2 64

/* Additional JPEG types */
typedef JSAMPLE *JSAMPROW;
typedef JSAMPROW *JSAMPARRAY;
typedef JSAMPARRAY *JSAMPIMAGE;
typedef JCOEF JBLOCK[DCTSIZE2];
typedef JBLOCK *JBLOCKROW;
typedef JBLOCKROW *JBLOCKARRAY;
typedef JBLOCKARRAY *JBLOCKIMAGE;
typedef JCOEF *JCOEFPTR;
#define NUM_QUANT_TBLS 4
#define NUM_HUFF_TBLS 4
#define MAX_COMPS_IN_SCAN 4
#define C_MAX_BLOCKS_IN_MCU 10
#define JMSG_STR_PARM_MAX 80

/* JPEG enums */
typedef enum {
    JCS_UNKNOWN = 0,
    JCS_GRAYSCALE = 1,
    JCS_RGB = 2,
    JCS_YCbCr = 3,
    JCS_CMYK = 4,
    JCS_YCCK = 5,
    JCS_BG_RGB = 6,
    JCS_BG_YCC = 7
} J_COLOR_SPACE;

typedef enum {
    JDCT_ISLOW = 0,
    JDCT_IFAST = 1,
    JDCT_FLOAT = 2
} J_DCT_METHOD;

typedef enum {
    JDITHER_NONE = 0,
    JDITHER_ORDERED = 1,
    JDITHER_FS = 2
} J_DITHER_MODE;

/* JPEG quantization table */
typedef struct {
    unsigned int sent_table;
    unsigned int quantval[DCTSIZE2];
} JQUANT_TBL;

/* JPEG Huffman table */
typedef struct {
    unsigned int sent_table;
    unsigned char bits[17];
    unsigned char huffval[256];
    void *priv;
} JHUFF_TBL;

/* JPEG component info */
typedef struct {
    int component_id;
    int component_index;
    int h_samp_factor;
    int v_samp_factor;
    int quant_tbl_no;
    int dc_tbl_no;
    int ac_tbl_no;
    JDIMENSION width_in_blocks;
    JDIMENSION height_in_blocks;
    int DCT_h_scaled_size;
    int DCT_v_scaled_size;
    JDIMENSION downsampled_width;
    JDIMENSION downsampled_height;
    boolean component_needed;
    int MCU_width;
    int MCU_height;
    int MCU_blocks;
    int MCU_sample_width;
    int last_col_width;
    int last_row_height;
    JQUANT_TBL *quant_table;
    void *dct_table;
} jpeg_component_info;

/* Configuration options */
#define HAVE_PROTOTYPES 1
#define HAVE_UNSIGNED_CHAR 1
#define HAVE_UNSIGNED_SHORT 1
#define HAVE_STDDEF_H 1
#define HAVE_STDLIB_H 1
#define HAVE_LOCALE_H 1

/* Memory management */
#define NEED_SYS_TYPES_H 1
#define NEED_SHORT_EXTERNAL_NAMES 1

/* Feature support */
#define C_ARITH_CODING_SUPPORTED 1
#define D_ARITH_CODING_SUPPORTED 1
#define C_MULTISCAN_FILES_SUPPORTED 1
#define D_MULTISCAN_FILES_SUPPORTED 1
#define C_PROGRESSIVE_SUPPORTED 1
#define D_PROGRESSIVE_SUPPORTED 1
#define ENTROPY_OPT_SUPPORTED 1
#define INPUT_SMOOTHING_SUPPORTED 1

/* Color space support */
#define JCS_EXTENSIONS 1
#define JCS_ALPHA_EXTENSIONS 1

/* Platform specific */
#ifdef _WIN32
#define NEED_FAR_POINTERS 0
#define RIGHT_SHIFT_IS_UNSIGNED 1
#endif

/* Compiler specific */
#ifdef __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif

/* Error handling */
#define USE_SETJMP 1

/* Optimization */
#define USE_ACCURATE_ROUNDING 1

#endif /* JCONFIG_H */