/*
 * png.h - Minimal PNG stub for ReportMason project
 * This is a stub implementation to allow compilation without PNG library
 */

#ifndef PNG_H
#define PNG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> // For size_t
#include <setjmp.h> // For jmp_buf
#include <stdio.h>  // For FILE

/* PNG types */
typedef struct png_struct_def png_struct;
typedef struct png_info_def png_info;
typedef png_struct *png_structp;
typedef png_info *png_infop;
typedef unsigned char png_byte;
typedef unsigned int png_uint_32;
typedef int png_int_32;
typedef unsigned short png_uint_16;
typedef short png_int_16;
typedef unsigned char png_byte;

/* PNG color types */
#define PNG_COLOR_TYPE_GRAY 0
#define PNG_COLOR_TYPE_RGB 2
#define PNG_COLOR_TYPE_PALETTE 3
#define PNG_COLOR_TYPE_GRAY_ALPHA 4
#define PNG_COLOR_TYPE_RGB_ALPHA 6

/* PNG interlace types */
#define PNG_INTERLACE_NONE 0
#define PNG_INTERLACE_ADAM7 1

/* PNG compression types */
#define PNG_COMPRESSION_TYPE_BASE 0
#define PNG_COMPRESSION_TYPE_DEFAULT PNG_COMPRESSION_TYPE_BASE

/* PNG filter types */
#define PNG_FILTER_TYPE_BASE 0
#define PNG_FILTER_TYPE_DEFAULT PNG_FILTER_TYPE_BASE

/* PNG error codes */
#define PNG_ERROR_CODE 0
#define PNG_WARNING_CODE 1

/* Additional types */
typedef png_struct **png_structpp;
typedef png_info **png_infopp;
typedef png_byte *png_bytep;
typedef png_byte **png_bytepp;
typedef size_t png_size_t;
typedef const char *png_const_charp;

/* Additional constants */
#define PNG_COLOR_MASK_ALPHA 0x04
#define PNG_LIBPNG_VER_STRING "1.6.37"
#define PNG_INFO_tRNS 0x10

/* Stub function declarations */
png_structp png_create_read_struct(const char *user_png_ver, void *error_ptr, void (*error_fn)(png_structp, png_const_charp), void (*warn_fn)(png_structp, png_const_charp));
png_structp png_create_write_struct(const char *user_png_ver, void *error_ptr, void (*error_fn)(png_structp, png_const_charp), void (*warn_fn)(png_structp, png_const_charp));
png_infop png_create_info_struct(png_structp png_ptr);
void png_destroy_read_struct(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);
void png_destroy_write_struct(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr);
void png_init_io(png_structp png_ptr, FILE *fp);
void png_set_read_fn(png_structp png_ptr, void *io_ptr, void (*read_data_fn)(png_structp, png_bytep, png_size_t));
void png_set_write_fn(png_structp png_ptr, void *io_ptr, void (*write_data_fn)(png_structp, png_bytep, png_size_t), void (*flush_data_fn)(png_structp));
void png_read_info(png_structp png_ptr, png_infop info_ptr);
void png_write_info(png_structp png_ptr, png_infop info_ptr);
void png_read_image(png_structp png_ptr, png_bytepp image);
void png_write_image(png_structp png_ptr, png_bytepp image);
void png_read_end(png_structp png_ptr, png_infop info_ptr);
void png_write_end(png_structp png_ptr, png_infop info_ptr);
png_uint_32 png_get_IHDR(png_structp png_ptr, png_infop info_ptr, png_uint_32 *width, png_uint_32 *height, int *bit_depth, int *color_type, int *interlace_type, int *compression_type, int *filter_type);
void png_set_IHDR(png_structp png_ptr, png_infop info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth, int color_type, int interlace_type, int compression_type, int filter_type);
void* png_get_io_ptr(png_structp png_ptr);
void png_destroy_read_struct(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);

/* Additional types */
typedef png_struct **png_structpp;
typedef png_info **png_infopp;
typedef png_byte *png_bytep;
typedef png_byte **png_bytepp;
typedef size_t png_size_t;
typedef const char *png_const_charp;

/* Additional constants */
#define PNG_COLOR_MASK_ALPHA 0x04
#define PNG_LIBPNG_VER_STRING "1.6.37"

/* Additional function declarations */
png_uint_32 png_get_image_width(png_structp png_ptr, png_infop info_ptr);
png_uint_32 png_get_image_height(png_structp png_ptr, png_infop info_ptr);
png_byte png_get_color_type(png_structp png_ptr, png_infop info_ptr);
png_byte png_get_bit_depth(png_structp png_ptr, png_infop info_ptr);
png_byte png_get_channels(png_structp png_ptr, png_infop info_ptr);
png_size_t png_get_rowbytes(png_structp png_ptr, png_infop info_ptr);
void png_set_palette_to_rgb(png_structp png_ptr);
void png_set_expand_gray_1_2_4_to_8(png_structp png_ptr);
png_uint_32 png_get_valid(png_structp png_ptr, png_infop info_ptr, png_uint_32 flag);
void png_set_tRNS_to_alpha(png_structp png_ptr);
void png_set_packing(png_structp png_ptr);
void png_read_update_info(png_structp png_ptr, png_infop info_ptr);
jmp_buf* png_jmpbuf(png_structp png_ptr);
void png_set_pHYs(png_structp png_ptr, png_infop info_ptr, png_uint_32 res_x, png_uint_32 res_y, int unit);

/* Additional constants */
#define PNG_RESOLUTION_UNKNOWN 0
#define PNG_RESOLUTION_METER 1

#ifdef __cplusplus
}
#endif

#endif /* PNG_H */
