/*
 * jerror.h
 * 
 * JPEG error handling definitions
 * This is a minimal implementation for ReportMason project
 */

#ifndef JERROR_H
#define JERROR_H

#include <setjmp.h>

/* Forward declarations */
typedef struct jpeg_error_mgr jpeg_error_mgr;

/* Error handler function type */
typedef void (*jpeg_error_handler_t)(void);

/* Error codes */
#define JERR_UNKNOWN_MARKER    1
#define JERR_NO_SOI            2
#define JERR_NOT_JPEG          3
#define JERR_NO_EOI            4
#define JERR_BAD_HEADER_LENGTH 5
#define JERR_BAD_WIDTH         6
#define JERR_BAD_HEIGHT        7
#define JERR_TOO_MANY_COLORS   8
#define JERR_UNSUPPORTED_FORMAT 9
#define JERR_BAD_COMPONENT_ID  10
#define JERR_BAD_PRECISION     11
#define JERR_BAD_SAMPLING      12
#define JERR_BAD_QUANT_TABLE   13
#define JERR_BAD_PROGRESSION   14
#define JERR_BAD_HUFF_TABLE    15
#define JERR_BAD_RESTART       16
#define JERR_BAD_SOS_LENGTH    17
#define JERR_BAD_SOS_PARAMS    18
#define JERR_BAD_DATA_LENGTH   19
#define JERR_BAD_DATA          20
#define JERR_BAD_IMAGE_SIZE    21
#define JERR_BAD_COMPS         22
#define JERR_BAD_HUFFMAN       23
#define JERR_BAD_PARAM         24
#define JERR_BAD_DCT_COEF      25
#define JERR_BAD_COLORCONV     27
#define JERR_BAD_DOWNSAMPLE    28
#define JERR_BAD_BIT_WIDTH     29
#define JERR_BAD_VERSION       30
#define JERR_BAD_OPTION        31
#define JERR_BAD_SCAN_SCRIPT   32
#define JERR_BAD_COMPONENT     33
#define JERR_BAD_SCAN          34
#define JERR_BAD_OPTIMIZE      35
#define JERR_BAD_BLOCK_SIZE    36
#define JERR_BAD_COEFCOUNT     37
#define JERR_BAD_COEF          38
#define JERR_BAD_SUBBAND       39
#define JERR_BAD_PACKET        40
#define JERR_BAD_PACKET_LENGTH 41
#define JERR_BAD_FRAME         42
#define JERR_BAD_HEADER        43
#define JERR_BAD_PICTURE       44
#define JERR_BAD_STATE         45
#define JERR_BAD_STRUCTURE     46
#define JERR_BAD_TABLE_CLASS   47
#define JERR_BAD_TABLE_ID      48
#define JERR_BAD_BIT_SIZE      49
#define JERR_BAD_REFRESH       50
#define JERR_BAD_NUMCOMPS      51
#define JERR_BAD_NUMSCANS      52
#define JERR_BAD_NUMSOS        53
#define JERR_BAD_SOS           54
#define JERR_BAD_EOI           55
#define JERR_BAD_SOI           56
#define JERR_BAD_TEM           57
#define JERR_BAD_RESYNC        58
#define JERR_BAD_INPUT         59
#define JERR_BAD_OUTPUT        60
#define JERR_BAD_PARAMETER     61
#define JERR_BAD_BUFFER        62
#define JERR_BAD_POINTER       63
#define JERR_BAD_SIZE          64
#define JERR_BAD_COUNT         65
#define JERR_BAD_OFFSET        66
#define JERR_BAD_RANGE         67
#define JERR_BAD_PIXEL         68
#define JERR_BAD_COLOR         69
#define JERR_BAD_FORMAT        70
#define JERR_BAD_DIMENSION     71
#define JERR_BAD_SAMPLE        72
#define JERR_BAD_DEPTH         73
#define JERR_BAD_COLORSPACE    74
#define JERR_BAD_ALIGNMENT     75
#define JERR_BAD_CLIP          76
#define JERR_BAD_ROTATION      77
#define JERR_BAD_TRANSFORM     78
#define JERR_BAD_TRANSLATION   79
#define JERR_BAD_SCALE         80
#define JERR_BAD_SHEAR         81
#define JERR_BAD_MATRIX        82
#define JERR_BAD_VECTOR        83
#define JERR_BAD_POINT         84
#define JERR_BAD_LINE          85
#define JERR_BAD_RECT          86
#define JERR_BAD_CIRCLE        87
#define JERR_BAD_ELLIPSE       88
#define JERR_BAD_ARC           89
#define JERR_BAD_CURVE         90
#define JERR_BAD_PATH          91
#define JERR_BAD_REGION        92
#define JERR_BAD_CLIP_PATH     93
#define JERR_BAD_MASK          94
#define JERR_BAD_PATTERN       95
#define JERR_BAD_SHADING       96
#define JERR_BAD_IMAGE         97
#define JERR_BAD_TILE          98
#define JERR_BAD_MATTE         99
#define JERR_BAD_OPACITY       100
#define JERR_IMAGE_TOO_BIG     101

/* Warning codes */
#define JWRN_ADOBE_XFORM      101
#define JWRN_ARITH_BAD_CODE   102
#define JWRN_BOGUS_PROGRESSION 103
#define JWRN_EXTRANEOUS_DATA  104
#define JWRN_HIT_MARKER       105
#define JWRN_HUFF_BAD_CODE    106
#define JWRN_JFIF_MAJOR       107
#define JWRN_JPEG_EOF         108
#define JWRN_MUST_RESYNC      109
#define JWRN_NOT_SEQUENTIAL   110
#define JWRN_TOO_MUCH_DATA    111
#define JWRN_UNKNOWN_MARKER   112

/* Message table structure */
typedef struct {
    int code;
    const char *text;
} jpeg_message_table;

/* Error message table */
extern const jpeg_message_table jpeg_std_message_table[];

/* Error handler functions */
extern void jpeg_std_error(jpeg_error_mgr *err);
extern void jpeg_std_warning(jpeg_error_mgr *err);

/* Error handling functions */
void jpeg_std_error_handler(void);
void jpeg_std_warning_handler(void);

/* Error message lookup */
const char *jpeg_std_message(int code);

#endif /* JERROR_H */
