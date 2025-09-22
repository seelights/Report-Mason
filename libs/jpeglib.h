#ifndef JPEGLIB_H
#define JPEGLIB_H

#include "jconfig.h"
#include "jmorecfg.h"
#include "jerror.h"

/* Error handler type */
typedef void (*jpeg_error_handler_t)(void);

/* Message table type - moved to jerror.h to avoid conflicts */

/* JPEG types */
typedef struct jpeg_common_struct jpeg_common_struct;
typedef struct jpeg_decompress_struct jpeg_decompress_struct;
typedef struct jpeg_compress_struct jpeg_compress_struct;
typedef struct jpeg_error_mgr jpeg_error_mgr;
typedef struct jpeg_source_mgr jpeg_source_mgr;
typedef struct jpeg_destination_mgr jpeg_destination_mgr;
typedef struct jpeg_memory_mgr jpeg_memory_mgr;
typedef struct jpeg_progress_mgr jpeg_progress_mgr;

/* Pointer types */
typedef jpeg_common_struct *j_common_ptr;
typedef jpeg_decompress_struct *j_decompress_ptr;
typedef jpeg_compress_struct *j_compress_ptr;

/* Complete structure definitions */
struct jpeg_error_mgr {
    void (*error_exit)(j_common_ptr cinfo);
    void (*emit_message)(j_common_ptr cinfo, int msg_level);
    void (*output_message)(j_common_ptr cinfo);
    void (*format_message)(j_common_ptr cinfo, char *buffer);
    void (*reset_error_mgr)(j_common_ptr cinfo);
    int msg_code;
    union {
        int i[8];
        char s[JMSG_STR_PARM_MAX];
    } msg_parm;
    int trace_level;
    long num_warnings;
    const char * const *jpeg_message_table;
    int last_jpeg_message;
    int addon_message_table;
    int first_addon_message;
    int last_addon_message;
    jmp_buf setjmp_buffer;
};

struct jpeg_source_mgr {
    const JOCTET *next_input_byte;
    size_t bytes_in_buffer;
    void (*init_source)(j_decompress_ptr cinfo);
    boolean (*fill_input_buffer)(j_decompress_ptr cinfo);
    void (*skip_input_data)(j_decompress_ptr cinfo, long num_bytes);
    boolean (*resync_to_restart)(j_decompress_ptr cinfo, int desired);
    void (*term_source)(j_decompress_ptr cinfo);
};

struct jpeg_memory_mgr {
    void *(*alloc_small)(j_common_ptr cinfo, int pool_id, size_t sizeofobject);
    void *(*alloc_large)(j_common_ptr cinfo, int pool_id, size_t sizeofobject);
    JSAMPARRAY (*alloc_sarray)(j_common_ptr cinfo, int pool_id, JDIMENSION samplesperrow, JDIMENSION numrows);
    JBLOCKARRAY (*alloc_barray)(j_common_ptr cinfo, int pool_id, JDIMENSION blocksperrow, JDIMENSION numrows);
    void (*free_small)(j_common_ptr cinfo, void *object, int pool_id);
    void (*free_large)(j_common_ptr cinfo, void *object, int pool_id);
    void (*free_sarray)(j_common_ptr cinfo, JSAMPARRAY object, int pool_id);
    void (*free_barray)(j_common_ptr cinfo, JBLOCKARRAY object, int pool_id);
    void (*request_virt_sarray)(j_common_ptr cinfo, int pool_id, boolean pre_zero, JDIMENSION samplesperrow, JDIMENSION numrows, JDIMENSION maxaccess);
    void (*request_virt_barray)(j_common_ptr cinfo, int pool_id, boolean pre_zero, JDIMENSION blocksperrow, JDIMENSION numrows, JDIMENSION maxaccess);
    void (*realize_virt_arrays)(j_common_ptr cinfo);
    JSAMPARRAY (*access_virt_sarray)(j_common_ptr cinfo, JSAMPARRAY object, JDIMENSION start_row, JDIMENSION num_rows, boolean writable);
    JBLOCKARRAY (*access_virt_barray)(j_common_ptr cinfo, JBLOCKARRAY object, JDIMENSION start_row, JDIMENSION num_rows, boolean writable);
    void (*free_pool)(j_common_ptr cinfo, int pool_id);
    void (*self_destruct)(j_common_ptr cinfo);
    long max_memory_to_use;
    long max_alloc_chunk;
};

struct jpeg_common_struct {
    struct jpeg_error_mgr *err;
    struct jpeg_memory_mgr *mem;
    struct jpeg_progress_mgr *progress;
    void *client_data;
    boolean is_decompressor;
    int global_state;
};

struct jpeg_decompress_struct {
    struct jpeg_error_mgr *err;
    struct jpeg_memory_mgr *mem;
    struct jpeg_progress_mgr *progress;
    void *client_data;
    boolean is_decompressor;
    int global_state;
    struct jpeg_source_mgr *src;
    JDIMENSION image_width;
    JDIMENSION image_height;
    int num_components;
    J_COLOR_SPACE jpeg_color_space;
    J_COLOR_SPACE out_color_space;
    JDIMENSION scale_num, scale_denom;
    JDIMENSION output_width;
    JDIMENSION output_height;
    int out_color_components;
    int output_components;
    int rec_outbuf_height;
    int actual_number_of_scans;
    boolean buffered_image;
    boolean raw_data_out;
    J_DCT_METHOD dct_method;
    boolean do_fancy_upsampling;
    boolean do_block_smoothing;
    boolean quantize_colors;
    J_DITHER_MODE dither_mode;
    boolean two_pass_quantize;
    int desired_number_of_colors;
    boolean enable_1pass_quant;
    boolean enable_external_quant;
    boolean enable_2pass_quant;
    JDIMENSION output_scanline;
    JDIMENSION input_scan_number;
    int input_i_scan_number;
    int output_scan_number;
    int output_i_scan_number;
    int coef_bits[DCTSIZE2];
    JQUANT_TBL *quant_tbl_ptrs[NUM_QUANT_TBLS];
    JHUFF_TBL *dc_huff_tbl_ptrs[NUM_HUFF_TBLS];
    JHUFF_TBL *ac_huff_tbl_ptrs[NUM_HUFF_TBLS];
    int data_precision;
    JDIMENSION max_h_samp_factor;
    JDIMENSION max_v_samp_factor;
    JDIMENSION min_DCT_h_scaled_size;
    JDIMENSION min_DCT_v_scaled_size;
    JDIMENSION total_iMCU_rows;
    JSAMPLE *sample_range_limit;
    int comps_in_scan;
    jpeg_component_info *cur_comp_info[MAX_COMPS_IN_SCAN];
    JDIMENSION MCUs_per_row;
    JDIMENSION MCU_rows_in_scan;
    int blocks_in_MCU;
    int MCU_membership[C_MAX_BLOCKS_IN_MCU];
    int Ss, Se, Ah, Al;
    int unread_marker;
    boolean saw_Adobe_marker;
    boolean saw_JFIF_marker;
    int Adobe_transform;
    struct jpeg_decomp_master *master;
    struct jpeg_d_main_controller *main;
    struct jpeg_d_coef_controller *coef;
    struct jpeg_d_post_controller *post;
    struct jpeg_input_controller *inputctl;
    struct jpeg_marker_reader *marker;
    struct jpeg_entropy_decoder *entropy;
    struct jpeg_inverse_dct *idct;
    struct jpeg_upsampler *upsample;
    struct jpeg_color_deconverter *cconvert;
    struct jpeg_color_quantizer *cquantize;
};

typedef jpeg_common_struct *j_common_ptr;
typedef jpeg_decompress_struct *j_decompress_ptr;
typedef jpeg_compress_struct *j_compress_ptr;

/* Sample array types */
typedef JSAMPLE *JSAMPROW;
typedef JSAMPROW *JSAMPARRAY;
typedef JSAMPARRAY *JSAMPIMAGE;

/* Pool types */
typedef enum {
    JPOOL_PERMANENT_VAL = 0,
    JPOOL_IMAGE_VAL = 1,
    JPOOL_NUMPOOLS_VAL = 2
} JPOOL_T;

/* Constants */
#define JPEG_MAX_DIMENSION 65500L
#define JPEG_SUSPENDED 0
#define JPEG_HEADER_OK 1
#define JPEG_HEADER_TABLES_ONLY 2
#define TRUE 1
#define FALSE 0

/* Function declarations */
extern void jpeg_std_error(jpeg_error_mgr *err);
extern void jpeg_std_warning(jpeg_error_mgr *err);
extern const char *jpeg_std_message(int code);
extern void jpeg_destroy_decompress(j_decompress_ptr cinfo);
extern int jpeg_read_header(j_decompress_ptr cinfo, boolean require_image);
extern boolean jpeg_start_decompress(j_decompress_ptr cinfo);
extern JDIMENSION jpeg_read_scanlines(j_decompress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION max_lines);
extern boolean jpeg_finish_decompress(j_decompress_ptr cinfo);
extern void jpeg_destroy_compress(j_compress_ptr cinfo);
extern void jpeg_start_compress(j_compress_ptr cinfo, boolean write_all_tables);
extern JDIMENSION jpeg_write_scanlines(j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines);
extern void jpeg_finish_compress(j_compress_ptr cinfo);
extern void jpeg_CreateDecompress(j_decompress_ptr cinfo, int version, size_t structsize);
extern void jpeg_CreateCompress(j_compress_ptr cinfo, int version, size_t structsize);
extern boolean jpeg_resync_to_restart(j_decompress_ptr cinfo, int desired);
extern void jpeg_mem_src(j_decompress_ptr cinfo, unsigned char *inbuffer, unsigned long insize);
extern void jpeg_abort_decompress(j_decompress_ptr cinfo);

#endif
