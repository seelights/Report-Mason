/*
 * openjpeg.h
 * 
 * OpenJPEG header for ReportMason project
 * This is a minimal implementation for basic JPEG2000 support
 */

#ifndef OPENJPEG_H
#define OPENJPEG_H

#ifdef __cplusplus
extern "C" {
#endif

/* OpenJPEG version */
#define OPJ_VERSION_MAJOR 2
#define OPJ_VERSION_MINOR 4
#define OPJ_VERSION_BUILD 0

/* Boolean constants */
#define OPJ_TRUE 1
#define OPJ_FALSE 0

/* Error codes */
#define OPJ_SUCCESS 0
#define OPJ_FAILURE -1
#define OPJ_EOF -2
#define OPJ_INVALID_PARAMETER -3
#define OPJ_OUT_OF_MEMORY -4
#define OPJ_INVALID_CALLBACK -5
#define OPJ_INVALID_OPERATION -6
#define OPJ_UNSUPPORTED_FORMAT -7
#define OPJ_INVALID_HEADER -8
#define OPJ_INVALID_DATA -9
#define OPJ_INVALID_STREAM -10
#define OPJ_INVALID_TILE -11
#define OPJ_INVALID_PRECINCT -12
#define OPJ_INVALID_PACKET -13
#define OPJ_INVALID_CODEC -14
#define OPJ_INVALID_IMAGE -15
#define OPJ_INVALID_METADATA -16
#define OPJ_INVALID_INDEX -17
#define OPJ_INVALID_LAYER -18
#define OPJ_INVALID_RESOLUTION -19
#define OPJ_INVALID_COMPONENT -20
#define OPJ_INVALID_TRANSFORM -21
#define OPJ_INVALID_WAVELET -22
#define OPJ_INVALID_QUANTIZATION -23
#define OPJ_INVALID_ENTROPY -24
#define OPJ_INVALID_MARKER -25
#define OPJ_INVALID_COLORSPACE -26
#define OPJ_INVALID_PROGRESSION -27
#define OPJ_INVALID_TILE_SIZE -28
#define OPJ_INVALID_PRECINCT_SIZE -29
#define OPJ_INVALID_PACKET_SIZE -30
#define OPJ_INVALID_CODEC_TYPE -31
#define OPJ_INVALID_IMAGE_TYPE -32
#define OPJ_INVALID_METADATA_TYPE -33
#define OPJ_INVALID_INDEX_TYPE -34
#define OPJ_INVALID_LAYER_TYPE -35
#define OPJ_INVALID_RESOLUTION_TYPE -36
#define OPJ_INVALID_COMPONENT_TYPE -37
#define OPJ_INVALID_TRANSFORM_TYPE -38
#define OPJ_INVALID_WAVELET_TYPE -39
#define OPJ_INVALID_QUANTIZATION_TYPE -40
#define OPJ_INVALID_ENTROPY_TYPE -41
#define OPJ_INVALID_MARKER_TYPE -42
#define OPJ_INVALID_COLORSPACE_TYPE -43
#define OPJ_INVALID_PROGRESSION_TYPE -44
#define OPJ_INVALID_TILE_SIZE_TYPE -45
#define OPJ_INVALID_PRECINCT_SIZE_TYPE -46
#define OPJ_INVALID_PACKET_SIZE_TYPE -47

/* Codec types */
typedef enum {
    OPJ_CODEC_UNKNOWN = -1,
    OPJ_CODEC_J2K = 0,
    OPJ_CODEC_JPT = 1,
    OPJ_CODEC_JP2 = 2,
    OPJ_CODEC_JPP = 3,
    OPJ_CODEC_JPX = 4
} OPJ_CODEC_FORMAT;

/* Image types */
typedef enum {
    OPJ_CLRSPC_UNKNOWN = -1,
    OPJ_CLRSPC_UNSPECIFIED = 0,
    OPJ_CLRSPC_SRGB = 1,
    OPJ_CLRSPC_GRAY = 2,
    OPJ_CLRSPC_SYCC = 3
} OPJ_COLOR_SPACE;

/* Progression order */
typedef enum {
    OPJ_PROG_UNKNOWN = -1,
    OPJ_LRCP = 0,
    OPJ_RLCP = 1,
    OPJ_RPCL = 2,
    OPJ_PCRL = 3,
    OPJ_CPRL = 4
} OPJ_PROG_ORDER;

/* Component types */
typedef enum {
    OPJ_COMPRESSION_UNKNOWN = -1,
    OPJ_COMPRESSION_NONE = 0,
    OPJ_COMPRESSION_LOSSLESS = 1,
    OPJ_COMPRESSION_LOSSY = 2
} OPJ_COMPRESSION_TYPE;

/* Basic types */
typedef unsigned char OPJ_UINT8;
typedef unsigned short OPJ_UINT16;
typedef int OPJ_INT32;
typedef unsigned int OPJ_UINT32;
typedef long long OPJ_INT64;
typedef unsigned long long OPJ_UINT64;
typedef long long OPJ_OFF_T;
typedef size_t OPJ_SIZE_T;
typedef int OPJ_BOOL;
typedef float OPJ_FLOAT32;
typedef double OPJ_FLOAT64;

/* Image component structure */
typedef struct {
    OPJ_INT32 dx;
    OPJ_INT32 dy;
    OPJ_INT32 w;
    OPJ_INT32 h;
    OPJ_INT32 x0;
    OPJ_INT32 y0;
    OPJ_INT32 prec;
    OPJ_INT32 bpp;
    OPJ_INT32 sgnd;
    OPJ_INT32 resno_decoded;
    OPJ_INT32 factor;
    OPJ_INT32 *data;
    OPJ_UINT32 alpha;
} opj_image_comp_t;

/* Image structure */
typedef struct {
    OPJ_UINT32 x0;
    OPJ_UINT32 y0;
    OPJ_UINT32 x1;
    OPJ_UINT32 y1;
    OPJ_UINT32 numcomps;
    OPJ_COLOR_SPACE color_space;
    opj_image_comp_t *comps;
    OPJ_UINT8 *icc_profile_buf;
    OPJ_UINT32 icc_profile_len;
} opj_image_t;

/* Codec structure */
typedef struct opj_codec {
    OPJ_BOOL is_decompressor;
    OPJ_CODEC_FORMAT codec_format;
    void *j2k_handle;
    void *jp2_handle;
    void *mj2_handle;
    void *jpt_handle;
} opj_codec_t;

/* Stream structure */
typedef struct opj_stream {
    void *user_data;
    OPJ_UINT64 user_data_length;
    OPJ_UINT64 user_data_size;
    OPJ_UINT64 user_data_pos;
    OPJ_BOOL free_user_data;
    void *user_data_read_fn;
    void *user_data_write_fn;
    void *user_data_skip_fn;
    void *user_data_seek_fn;
} opj_stream_t;

/* Parameters structure */
typedef struct {
    OPJ_UINT32 tile_size_on;
    OPJ_INT32 cp_tx0;
    OPJ_INT32 cp_ty0;
    OPJ_INT32 cp_tdx;
    OPJ_INT32 cp_tdy;
    OPJ_INT32 cp_disto_alloc;
    OPJ_INT32 cp_fixed_alloc;
    OPJ_INT32 cp_fixed_quality;
    OPJ_INT32 *cp_matrice;
    char *cp_comment;
    OPJ_UINT32 csty;
    OPJ_INT32 prog_order;
    OPJ_UINT32 numpocs;
    OPJ_INT32 POC[32];
    OPJ_INT32 numresolution;
    OPJ_INT32 cblockw_init;
    OPJ_INT32 cblockh_init;
    OPJ_INT32 mode;
    OPJ_INT32 irreversible;
    OPJ_INT32 roi_compno;
    OPJ_INT32 roi_shift;
    OPJ_INT32 res_spec;
    OPJ_INT32 prcw_init[33];
    OPJ_INT32 prch_init[33];
    char infile[4096];
    char outfile[4096];
    OPJ_INT32 index_on;
    char index[4096];
    OPJ_INT32 image_offset_x0;
    OPJ_INT32 image_offset_y0;
    OPJ_INT32 subsampling_dx;
    OPJ_INT32 subsampling_dy;
    OPJ_INT32 decod_format;
    OPJ_INT32 cod_format;
    OPJ_BOOL jpwl_correct;
    OPJ_INT32 jpwl_hprot_MH;
    OPJ_INT32 *jpwl_hprot_TPH_tileno;
    OPJ_INT32 *jpwl_hprot_TPH;
    OPJ_INT32 jpwl_pprot_tileno[16];
    OPJ_INT32 jpwl_pprot_packno[16];
    OPJ_INT32 jpwl_pprot[16];
    OPJ_INT32 jpwl_sens_size;
    OPJ_INT32 jpwl_sens_addr;
    OPJ_INT32 jpwl_sens_range;
    OPJ_INT32 jpwl_sens_MH;
    OPJ_INT32 *jpwl_sens_TPH_tileno;
    OPJ_INT32 *jpwl_sens_TPH;
    OPJ_BOOL cp_cinema;
    OPJ_INT32 max_comp_size;
    OPJ_INT32 cp_rsiz;
    char tp_on;
    char tp_flag;
    char tp_mct;
    OPJ_INT32 tcp_mct;
    OPJ_INT32 jpip_on;
    void *mct_data;
    void *mct_data_size;
    void *mct_data_capacity;
} opj_cparameters_t;

/* Decompression parameters */
typedef struct {
    OPJ_UINT32 cp_reduce;
    OPJ_UINT32 cp_layer;
    char infile[4096];
    char outfile[4096];
    OPJ_INT32 decod_format;
    OPJ_INT32 cod_format;
    OPJ_UINT32 DA_x0;
    OPJ_UINT32 DA_x1;
    OPJ_UINT32 DA_y0;
    OPJ_UINT32 DA_y1;
    OPJ_UINT32 flags;
    OPJ_BOOL m_verbose;
    OPJ_UINT32 tile_index;
    OPJ_UINT32 nb_tile_to_decode;
    OPJ_BOOL jpwl_correct;
    OPJ_INT32 jpwl_hprot_MH;
    OPJ_INT32 *jpwl_hprot_TPH_tileno;
    OPJ_INT32 *jpwl_hprot_TPH;
    OPJ_INT32 jpwl_pprot_tileno[16];
    OPJ_INT32 jpwl_pprot_packno[16];
    OPJ_INT32 jpwl_pprot[16];
    OPJ_INT32 jpwl_sens_size;
    OPJ_INT32 jpwl_sens_addr;
    OPJ_INT32 jpwl_sens_range;
    OPJ_INT32 jpwl_sens_MH;
    OPJ_INT32 *jpwl_sens_TPH_tileno;
    OPJ_INT32 *jpwl_sens_TPH;
    OPJ_BOOL cp_cinema;
    OPJ_INT32 max_comp_size;
    OPJ_INT32 cp_rsiz;
    OPJ_BOOL tp_on;
    OPJ_INT32 tp_flag;
    OPJ_INT32 tp_mct;
    OPJ_INT32 jpip_on;
} opj_dparameters_t;

/* Function declarations */
opj_codec_t* opj_create_decompress(OPJ_CODEC_FORMAT format);
opj_codec_t* opj_create_compress(OPJ_CODEC_FORMAT format);
void opj_destroy_codec(opj_codec_t *p_codec);
opj_image_t* opj_image_create(OPJ_UINT32 numcmpts, opj_image_comp_t *cmptparms, OPJ_COLOR_SPACE clrspc);
void opj_image_destroy(opj_image_t *image);
opj_stream_t* opj_stream_create_default_file_stream(const char *fname, OPJ_BOOL p_is_read_stream);
void opj_stream_destroy(opj_stream_t *p_stream);
OPJ_BOOL opj_setup_decoder(opj_codec_t *p_codec, opj_dparameters_t *parameters);
OPJ_BOOL opj_setup_encoder(opj_codec_t *p_codec, opj_cparameters_t *parameters, opj_image_t *image);
OPJ_BOOL opj_read_header(opj_stream_t *p_stream, opj_codec_t *p_codec, opj_image_t **p_image);
OPJ_BOOL opj_decode(opj_codec_t *p_codec, opj_stream_t *p_stream, opj_image_t *p_image);
OPJ_BOOL opj_end_decompress(opj_codec_t *p_codec, opj_stream_t *p_stream);
OPJ_BOOL opj_start_compress(opj_codec_t *p_codec, opj_image_t *p_image, opj_stream_t *p_stream);
OPJ_BOOL opj_encode(opj_codec_t *p_codec, opj_stream_t *p_stream);
OPJ_BOOL opj_end_compress(opj_codec_t *p_codec, opj_stream_t *p_stream);

/* Additional stream functions */
opj_stream_t* opj_stream_default_create(OPJ_BOOL p_is_input);
void opj_stream_set_user_data(opj_stream_t *p_stream, void *p_user_data, void (*p_function)(void *));
void opj_stream_set_read_function(opj_stream_t *p_stream, OPJ_SIZE_T (*p_function)(void *, OPJ_SIZE_T, void *));
void opj_stream_set_skip_function(opj_stream_t *p_stream, OPJ_OFF_T (*p_function)(OPJ_OFF_T, void *));
void opj_stream_set_seek_function(opj_stream_t *p_stream, OPJ_BOOL (*p_function)(OPJ_OFF_T, void *));
void opj_stream_set_user_data_length(opj_stream_t *p_stream, OPJ_UINT64 data_length);

/* Parameter functions */
void opj_set_default_decoder_parameters(opj_dparameters_t *parameters);
void opj_set_default_encoder_parameters(opj_cparameters_t *parameters);

/* Handler functions */
void opj_set_warning_handler(opj_codec_t *p_codec, void (*p_function)(const char *, void *), void *p_user_data);
void opj_set_error_handler(opj_codec_t *p_codec, void (*p_function)(const char *, void *), void *p_user_data);

/* Decode area function */
OPJ_BOOL opj_set_decode_area(opj_codec_t *p_codec, opj_image_t *p_image, OPJ_INT32 p_start_x, OPJ_INT32 p_start_y, OPJ_INT32 p_end_x, OPJ_INT32 p_end_y);

/* Additional constants */
#define OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG 0x0001

#ifdef __cplusplus
}
#endif

#endif /* OPENJPEG_H */
