#ifndef CAIRO_H
#define CAIRO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Version macros */
#define CAIRO_VERSION_ENCODE(major, minor, micro) (((major) << 16) | ((minor) << 8) | (micro))
#define CAIRO_VERSION CAIRO_VERSION_ENCODE(1, 18, 0)
#define CAIRO_VERSION_MAJOR 1
#define CAIRO_VERSION_MINOR 18
#define CAIRO_VERSION_MICRO 0
#define CAIRO_VERSION_STRING "1.18.0"

/* MIME type constants */
#define CAIRO_MIME_TYPE_JPEG "image/jpeg"
#define CAIRO_MIME_TYPE_PNG "image/png"
#define CAIRO_MIME_TYPE_URI "text/x-uri"
#define CAIRO_MIME_TYPE_JP2 "image/jp2"
#define CAIRO_MIME_TYPE_JBIG2 "image/jbig2"
#define CAIRO_MIME_TYPE_CCITT_FAX "image/g3fax"
#define CAIRO_MIME_TYPE_UNIQUE_ID "text/x-unique-id"
#define CAIRO_MIME_TYPE_JBIG2_GLOBAL_ID "text/x-jbig2-global-id"
#define CAIRO_MIME_TYPE_JBIG2_GLOBAL "text/x-jbig2-global"
#define CAIRO_MIME_TYPE_CCITT_FAX_PARAMS "text/x-ccitt-fax-params"

/* Tag constants */
#define CAIRO_TAG_DEST "dest"
#define CAIRO_TAG_LINK "link"
#define CAIRO_TAG_CONTENT_REF "content-ref"
#define CAIRO_TAG_CONTENT "content"

/* Basic Cairo types */
typedef struct _cairo cairo_t;
typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo_pattern cairo_pattern_t;
typedef struct _cairo_matrix cairo_matrix_t;
typedef struct _cairo_font_face cairo_font_face_t;
typedef struct _cairo_scaled_font cairo_scaled_font_t;
typedef struct _cairo_font_options cairo_font_options_t;
typedef struct _cairo_path cairo_path_t;
typedef struct _cairo_rectangle cairo_rectangle_t;
typedef struct _cairo_glyph cairo_glyph_t;
typedef struct _cairo_text_cluster cairo_text_cluster_t;
typedef struct _cairo_user_data_key cairo_user_data_key_t;
typedef struct _cairo_text_extents cairo_text_extents_t;
typedef struct _cairo_font_extents cairo_font_extents_t;

/* Boolean type */
typedef int cairo_bool_t;

/* Status types */
typedef enum {
    CAIRO_STATUS_SUCCESS = 0,
    CAIRO_STATUS_NO_MEMORY,
    CAIRO_STATUS_INVALID_RESTORE,
    CAIRO_STATUS_INVALID_POP_GROUP,
    CAIRO_STATUS_NO_CURRENT_POINT,
    CAIRO_STATUS_INVALID_MATRIX,
    CAIRO_STATUS_INVALID_STATUS,
    CAIRO_STATUS_NULL_POINTER,
    CAIRO_STATUS_INVALID_STRING,
    CAIRO_STATUS_INVALID_PATH_DATA,
    CAIRO_STATUS_READ_ERROR,
    CAIRO_STATUS_WRITE_ERROR,
    CAIRO_STATUS_SURFACE_FINISHED,
    CAIRO_STATUS_SURFACE_TYPE_MISMATCH,
    CAIRO_STATUS_PATTERN_TYPE_MISMATCH,
    CAIRO_STATUS_INVALID_CONTENT,
    CAIRO_STATUS_INVALID_FORMAT,
    CAIRO_STATUS_INVALID_VISUAL,
    CAIRO_STATUS_FILE_NOT_FOUND,
    CAIRO_STATUS_INVALID_DASH,
    CAIRO_STATUS_INVALID_DSC_COMMENT,
    CAIRO_STATUS_INVALID_INDEX,
    CAIRO_STATUS_CLIP_NOT_REPRESENTABLE,
    CAIRO_STATUS_TEMP_FILE_ERROR,
    CAIRO_STATUS_INVALID_STRIDE,
    CAIRO_STATUS_FONT_TYPE_MISMATCH,
    CAIRO_STATUS_USER_FONT_IMMUTABLE,
    CAIRO_STATUS_USER_FONT_ERROR,
    CAIRO_STATUS_NEGATIVE_COUNT,
    CAIRO_STATUS_INVALID_CLUSTERS,
    CAIRO_STATUS_INVALID_SLANT,
    CAIRO_STATUS_INVALID_WEIGHT,
    CAIRO_STATUS_INVALID_SIZE,
    CAIRO_STATUS_USER_FONT_NOT_IMPLEMENTED,
    CAIRO_STATUS_DEVICE_TYPE_MISMATCH,
    CAIRO_STATUS_DEVICE_ERROR,
    CAIRO_STATUS_INVALID_MESH_CONSTRUCTION,
    CAIRO_STATUS_DEVICE_FINISHED,
    CAIRO_STATUS_JBIG2_GLOBAL_MISSING,
    CAIRO_STATUS_PNG_ERROR,
    CAIRO_STATUS_FREETYPE_ERROR,
    CAIRO_STATUS_WIN32_GDI_ERROR,
    CAIRO_STATUS_TAG_ERROR,
    CAIRO_STATUS_LAST_STATUS
} cairo_status_t;

/* Content types */
typedef enum {
    CAIRO_CONTENT_COLOR = 0x1000,
    CAIRO_CONTENT_ALPHA = 0x2000,
    CAIRO_CONTENT_COLOR_ALPHA = 0x3000
} cairo_content_t;

/* Pattern types */
typedef enum {
    CAIRO_PATTERN_TYPE_SOLID,
    CAIRO_PATTERN_TYPE_SURFACE,
    CAIRO_PATTERN_TYPE_LINEAR,
    CAIRO_PATTERN_TYPE_RADIAL,
    CAIRO_PATTERN_TYPE_MESH,
    CAIRO_PATTERN_TYPE_RASTER_SOURCE
} cairo_pattern_type_t;

/* Fill rule types */
typedef enum {
    CAIRO_FILL_RULE_WINDING,
    CAIRO_FILL_RULE_EVEN_ODD
} cairo_fill_rule_t;

/* Operator types */
typedef enum {
    CAIRO_OPERATOR_CLEAR,
    CAIRO_OPERATOR_SOURCE,
    CAIRO_OPERATOR_OVER,
    CAIRO_OPERATOR_IN,
    CAIRO_OPERATOR_OUT,
    CAIRO_OPERATOR_ATOP,
    CAIRO_OPERATOR_DEST,
    CAIRO_OPERATOR_DEST_OVER,
    CAIRO_OPERATOR_DEST_IN,
    CAIRO_OPERATOR_DEST_OUT,
    CAIRO_OPERATOR_DEST_ATOP,
    CAIRO_OPERATOR_XOR,
    CAIRO_OPERATOR_ADD,
    CAIRO_OPERATOR_SATURATE,
    CAIRO_OPERATOR_MULTIPLY,
    CAIRO_OPERATOR_SCREEN,
    CAIRO_OPERATOR_OVERLAY,
    CAIRO_OPERATOR_DARKEN,
    CAIRO_OPERATOR_LIGHTEN,
    CAIRO_OPERATOR_COLOR_DODGE,
    CAIRO_OPERATOR_COLOR_BURN,
    CAIRO_OPERATOR_HARD_LIGHT,
    CAIRO_OPERATOR_SOFT_LIGHT,
    CAIRO_OPERATOR_DIFFERENCE,
    CAIRO_OPERATOR_EXCLUSION,
    CAIRO_OPERATOR_HSL_HUE,
    CAIRO_OPERATOR_HSL_SATURATION,
    CAIRO_OPERATOR_HSL_COLOR,
    CAIRO_OPERATOR_HSL_LUMINOSITY
} cairo_operator_t;

/* Extend types */
typedef enum {
    CAIRO_EXTEND_NONE,
    CAIRO_EXTEND_REPEAT,
    CAIRO_EXTEND_REFLECT,
    CAIRO_EXTEND_PAD
} cairo_extend_t;

/* Filter types */
typedef enum {
    CAIRO_FILTER_FAST,
    CAIRO_FILTER_GOOD,
    CAIRO_FILTER_BEST,
    CAIRO_FILTER_NEAREST,
    CAIRO_FILTER_BILINEAR,
    CAIRO_FILTER_GAUSSIAN
} cairo_filter_t;

/* Line cap types */
typedef enum {
    CAIRO_LINE_CAP_BUTT,
    CAIRO_LINE_CAP_ROUND,
    CAIRO_LINE_CAP_SQUARE
} cairo_line_cap_t;

/* Line join types */
typedef enum {
    CAIRO_LINE_JOIN_MITER,
    CAIRO_LINE_JOIN_ROUND,
    CAIRO_LINE_JOIN_BEVEL
} cairo_line_join_t;

/* Surface types */
typedef enum {
    CAIRO_SURFACE_TYPE_IMAGE,
    CAIRO_SURFACE_TYPE_PDF,
    CAIRO_SURFACE_TYPE_PS,
    CAIRO_SURFACE_TYPE_XLIB,
    CAIRO_SURFACE_TYPE_XCB,
    CAIRO_SURFACE_TYPE_GLITZ,
    CAIRO_SURFACE_TYPE_QUARTZ,
    CAIRO_SURFACE_TYPE_WIN32,
    CAIRO_SURFACE_TYPE_BEOS,
    CAIRO_SURFACE_TYPE_DIRECTFB,
    CAIRO_SURFACE_TYPE_SVG,
    CAIRO_SURFACE_TYPE_OS2,
    CAIRO_SURFACE_TYPE_WIN32_PRINTING,
    CAIRO_SURFACE_TYPE_QUARTZ_IMAGE,
    CAIRO_SURFACE_TYPE_SCRIPT,
    CAIRO_SURFACE_TYPE_QT,
    CAIRO_SURFACE_TYPE_RECORDING,
    CAIRO_SURFACE_TYPE_VG,
    CAIRO_SURFACE_TYPE_GL,
    CAIRO_SURFACE_TYPE_DRM,
    CAIRO_SURFACE_TYPE_TEE,
    CAIRO_SURFACE_TYPE_XML,
    CAIRO_SURFACE_TYPE_SKIA,
    CAIRO_SURFACE_TYPE_SUBSURFACE,
    CAIRO_SURFACE_TYPE_COGL
} cairo_surface_type_t;

/* Format types */
typedef enum {
    CAIRO_FORMAT_INVALID = -1,
    CAIRO_FORMAT_ARGB32 = 0,
    CAIRO_FORMAT_RGB24 = 1,
    CAIRO_FORMAT_A8 = 2,
    CAIRO_FORMAT_A1 = 3,
    CAIRO_FORMAT_RGB16_565 = 4,
    CAIRO_FORMAT_RGB30 = 5
} cairo_format_t;

/* Complete structure definitions */
struct _cairo_matrix {
    double xx; double yx;
    double xy; double yy;
    double x0; double y0;
};

struct _cairo_text_extents {
    double x_bearing;
    double y_bearing;
    double width;
    double height;
    double x_advance;
    double y_advance;
};

struct _cairo_font_extents {
    double ascent;
    double descent;
    double height;
    double max_x_advance;
    double max_y_advance;
};

struct _cairo_user_data_key {
    int unused;
};

struct _cairo_rectangle {
    double x;
    double y;
    double width;
    double height;
};

struct _cairo_glyph {
    unsigned long index;
    double x;
    double y;
};

struct _cairo_text_cluster {
    int num_bytes;
    int num_glyphs;
};

/* Additional enums */
typedef enum {
    CAIRO_HINT_STYLE_DEFAULT,
    CAIRO_HINT_STYLE_NONE,
    CAIRO_HINT_STYLE_SLIGHT,
    CAIRO_HINT_STYLE_MEDIUM,
    CAIRO_HINT_STYLE_FULL
} cairo_hint_style_t;

typedef enum {
    CAIRO_HINT_METRICS_DEFAULT,
    CAIRO_HINT_METRICS_OFF,
    CAIRO_HINT_METRICS_ON
} cairo_hint_metrics_t;

typedef enum {
    CAIRO_ANTIALIAS_DEFAULT,
    CAIRO_ANTIALIAS_NONE,
    CAIRO_ANTIALIAS_GRAY,
    CAIRO_ANTIALIAS_SUBPIXEL,
    CAIRO_ANTIALIAS_FAST,
    CAIRO_ANTIALIAS_GOOD,
    CAIRO_ANTIALIAS_BEST
} cairo_antialias_t;

typedef enum {
    CAIRO_TEXT_CLUSTER_FLAG_BACKWARD = 0x00000001
} cairo_text_cluster_flags_t;

/* Function pointer types */
typedef void (*cairo_destroy_func_t)(void *data);
typedef cairo_status_t (*cairo_user_scaled_font_init_func_t)(cairo_scaled_font_t *scaled_font, cairo_t *cr, cairo_font_extents_t *extents);
typedef cairo_status_t (*cairo_user_scaled_font_render_glyph_func_t)(cairo_scaled_font_t *scaled_font, unsigned long glyph, cairo_t *cr, cairo_text_extents_t *extents);

/* Stub function declarations */
cairo_status_t cairo_status(cairo_t *cr);
const char* cairo_status_to_string(cairo_status_t status);
cairo_surface_t* cairo_image_surface_create(cairo_format_t format, int width, int height);
cairo_t* cairo_create(cairo_surface_t *target);
void cairo_destroy(cairo_t *cr);
void cairo_surface_destroy(cairo_surface_t *surface);
cairo_surface_type_t cairo_surface_get_type(cairo_surface_t *surface);
int cairo_image_surface_get_width(cairo_surface_t *surface);
int cairo_image_surface_get_height(cairo_surface_t *surface);
int cairo_image_surface_get_stride(cairo_surface_t *surface);
unsigned char* cairo_image_surface_get_data(cairo_surface_t *surface);

/* Additional function declarations */
void cairo_font_face_destroy(cairo_font_face_t *font_face);
void cairo_matrix_init(cairo_matrix_t *matrix, double xx, double yx, double xy, double yy, double x0, double y0);
cairo_font_options_t* cairo_font_options_create(void);
void cairo_font_options_destroy(cairo_font_options_t *options);
void cairo_font_options_set_hint_style(cairo_font_options_t *options, cairo_hint_style_t hint_style);
void cairo_font_options_set_hint_metrics(cairo_font_options_t *options, cairo_hint_metrics_t hint_metrics);
cairo_scaled_font_t* cairo_scaled_font_create(cairo_font_face_t *font_face, const cairo_matrix_t *font_matrix, const cairo_matrix_t *ctm, const cairo_font_options_t *options);
void cairo_scaled_font_destroy(cairo_scaled_font_t *scaled_font);
void cairo_scaled_font_text_extents(cairo_scaled_font_t *scaled_font, const char *utf8, cairo_text_extents_t *extents);
cairo_font_face_t* cairo_scaled_font_get_font_face(cairo_scaled_font_t *scaled_font);
void cairo_show_text_glyphs(cairo_t *cr, const char *utf8, int utf8_len, const cairo_glyph_t *glyphs, int num_glyphs, const cairo_text_cluster_t *clusters, int num_clusters, cairo_text_cluster_flags_t cluster_flags);
cairo_pattern_t* cairo_pattern_create_mesh(void);
void cairo_mesh_pattern_begin_patch(cairo_pattern_t *pattern);
void cairo_mesh_pattern_end_patch(cairo_pattern_t *pattern);
void cairo_mesh_pattern_move_to(cairo_pattern_t *pattern, double x, double y);
void cairo_mesh_pattern_line_to(cairo_pattern_t *pattern, double x, double y);
void cairo_mesh_pattern_curve_to(cairo_pattern_t *pattern, double x1, double y1, double x2, double y2, double x3, double y3);
void cairo_mesh_pattern_set_control_point(cairo_pattern_t *pattern, unsigned int point_num, double x, double y);
void cairo_mesh_pattern_set_corner_color_rgb(cairo_pattern_t *pattern, unsigned int corner_num, double red, double green, double blue);
void cairo_mesh_pattern_set_corner_color_rgba(cairo_pattern_t *pattern, unsigned int corner_num, double red, double green, double blue, double alpha);
cairo_status_t cairo_surface_set_mime_data(cairo_surface_t *surface, const char *mime_type, const unsigned char *data, unsigned int length, cairo_destroy_func_t destroy, void *closure);
cairo_antialias_t cairo_get_antialias(cairo_t *cr);
void cairo_set_antialias(cairo_t *cr, cairo_antialias_t antialias);
void cairo_get_font_options(cairo_t *cr, cairo_font_options_t *options);
void cairo_set_font_options(cairo_t *cr, const cairo_font_options_t *options);
void cairo_matrix_init_identity(cairo_matrix_t *matrix);
cairo_status_t cairo_font_face_set_user_data(cairo_font_face_t *font_face, const cairo_user_data_key_t *key, void *user_data, cairo_destroy_func_t destroy);
void* cairo_font_face_get_user_data(cairo_font_face_t *font_face, const cairo_user_data_key_t *key);
cairo_surface_t* cairo_surface_reference(cairo_surface_t *surface);
void cairo_path_destroy(cairo_path_t *path);
void cairo_pattern_destroy(cairo_pattern_t *pattern);
cairo_t* cairo_reference(cairo_t *cr);
void cairo_get_matrix(cairo_t *cr, cairo_matrix_t *matrix);
cairo_surface_t* cairo_get_target(cairo_t *cr);
cairo_pattern_t* cairo_pattern_create_rgb(double red, double green, double blue);
cairo_pattern_t* cairo_pattern_create_rgba(double red, double green, double blue, double alpha);
cairo_pattern_t* cairo_pattern_reference(cairo_pattern_t *pattern);
cairo_font_face_t* cairo_user_font_face_create(void);
void cairo_user_font_face_set_init_func(cairo_font_face_t *font_face, cairo_user_scaled_font_init_func_t init_func);
void cairo_user_font_face_set_render_glyph_func(cairo_font_face_t *font_face, cairo_user_scaled_font_render_glyph_func_t render_glyph_func);
unsigned int cairo_font_face_get_reference_count(cairo_font_face_t *font_face);
void cairo_transform(cairo_t *cr, const cairo_matrix_t *matrix);
void cairo_matrix_transform_distance(const cairo_matrix_t *matrix, double *dx, double *dy);
void cairo_matrix_transform_point(const cairo_matrix_t *matrix, double *x, double *y);
void cairo_save(cairo_t *cr);
void cairo_restore(cairo_t *cr);
void cairo_set_dash(cairo_t *cr, const double *dashes, int num_dashes, double offset);
void cairo_set_line_join(cairo_t *cr, cairo_line_join_t line_join);
cairo_line_join_t cairo_get_line_join(cairo_t *cr);
void cairo_set_line_cap(cairo_t *cr, cairo_line_cap_t line_cap);
cairo_line_cap_t cairo_get_line_cap(cairo_t *cr);
void cairo_set_miter_limit(cairo_t *cr, double limit);
void cairo_user_to_device_distance(cairo_t *cr, double *dx, double *dy);
void cairo_device_to_user_distance(cairo_t *cr, double *dx, double *dy);
void cairo_set_line_width(cairo_t *cr, double width);
double cairo_get_line_width(cairo_t *cr);
cairo_pattern_type_t cairo_pattern_get_type(cairo_pattern_t *pattern);
void cairo_set_operator(cairo_t *cr, cairo_operator_t op);
void cairo_pattern_add_color_stop_rgba(cairo_pattern_t *pattern, double offset, double red, double green, double blue, double alpha);
void cairo_push_group(cairo_t *cr);
void cairo_stroke(cairo_t *cr);
cairo_pattern_t* cairo_pop_group_to_source(cairo_t *cr);
void cairo_set_fill_rule(cairo_t *cr, cairo_fill_rule_t fill_rule);
void cairo_set_source(cairo_t *cr, cairo_pattern_t *source);
void cairo_clip(cairo_t *cr);
void cairo_set_matrix(cairo_t *cr, const cairo_matrix_t *matrix);
void cairo_mask(cairo_t *cr, cairo_pattern_t *pattern);
void cairo_fill(cairo_t *cr);
cairo_pattern_t* cairo_pattern_create_linear(double x0, double y0, double x1, double y1);
cairo_pattern_t* cairo_pattern_create_radial(double cx0, double cy0, double radius0, double cx1, double cy1, double radius1);
void cairo_pattern_set_extend(cairo_pattern_t *pattern, cairo_extend_t extend);
void cairo_pattern_set_matrix(cairo_pattern_t *pattern, const cairo_matrix_t *matrix);
void cairo_clip_extents(cairo_t *cr, double *x1, double *y1, double *x2, double *y2);
void cairo_user_to_device(cairo_t *cr, double *x, double *y);
cairo_surface_t* cairo_get_group_target(cairo_t *cr);
void cairo_surface_get_device_offset(cairo_surface_t *surface, double *x_offset, double *y_offset);
void cairo_pattern_get_surface(cairo_pattern_t *pattern, cairo_surface_t **surface);
void cairo_push_group_with_content(cairo_t *cr, cairo_content_t content);
void cairo_set_source_rgb(cairo_t *cr, double red, double green, double blue);
void cairo_paint(cairo_t *cr);
void cairo_paint_with_alpha(cairo_t *cr, double alpha);
void cairo_matrix_init_translate(cairo_matrix_t *matrix, double tx, double ty);
void cairo_matrix_multiply(cairo_matrix_t *result, const cairo_matrix_t *a, const cairo_matrix_t *b);
void cairo_surface_mark_dirty(cairo_surface_t *surface);
cairo_pattern_t* cairo_pattern_create_for_surface(cairo_surface_t *surface);
cairo_status_t cairo_pattern_status(cairo_pattern_t *pattern);
void cairo_pattern_set_filter(cairo_pattern_t *pattern, cairo_filter_t filter);
void cairo_matrix_scale(cairo_matrix_t *matrix, double sx, double sy);
cairo_status_t cairo_matrix_invert(cairo_matrix_t *matrix);
void cairo_new_path(cairo_t *cr);
void cairo_move_to(cairo_t *cr, double x, double y);
void cairo_curve_to(cairo_t *cr, double x1, double y1, double x2, double y2, double x3, double y3);
void cairo_line_to(cairo_t *cr, double x, double y);
void cairo_close_path(cairo_t *cr);
void cairo_glyph_path(cairo_t *cr, const cairo_glyph_t *glyphs, int num_glyphs);
cairo_path_t* cairo_copy_path(cairo_t *cr);
void cairo_append_path(cairo_t *cr, const cairo_path_t *path);
void cairo_set_font_face(cairo_t *cr, cairo_font_face_t *font_face);
void cairo_set_font_matrix(cairo_t *cr, const cairo_matrix_t *matrix);
void cairo_rectangle(cairo_t *cr, double x, double y, double width, double height);
void cairo_matrix_init_scale(cairo_matrix_t *matrix, double sx, double sy);
cairo_pattern_t* cairo_get_source(cairo_t *cr);
void cairo_matrix_translate(cairo_matrix_t *matrix, double tx, double ty);
void cairo_pattern_get_matrix(cairo_pattern_t *pattern, cairo_matrix_t *matrix);
cairo_bool_t cairo_surface_has_show_text_glyphs(cairo_surface_t *surface);
int cairo_get_dash_count(cairo_t *cr);
void cairo_get_dash(cairo_t *cr, double *dashes, double *offset);
double cairo_get_miter_limit(cairo_t *cr);

/* Additional functions needed by poppler */
void cairo_tag_begin(cairo_t *cr, const char *tag_name, const char *attributes);
void cairo_tag_end(cairo_t *cr, const char *tag_name);
cairo_pattern_t* cairo_pop_group(cairo_t *cr);
void cairo_device_to_user(cairo_t *cr, double *x, double *y);
cairo_surface_t* cairo_surface_create_similar(cairo_surface_t *other, cairo_content_t content, int width, int height);
cairo_status_t cairo_surface_status(cairo_surface_t *surface);
void cairo_scale(cairo_t *cr, double sx, double sy);
void cairo_translate(cairo_t *cr, double tx, double ty);
void cairo_show_glyphs(cairo_t *cr, const cairo_glyph_t *glyphs, int num_glyphs);
void cairo_surface_set_device_offset(cairo_surface_t *surface, double x_offset, double y_offset);
cairo_font_face_t* cairo_font_face_reference(cairo_font_face_t *font_face);
cairo_pattern_t* cairo_user_scaled_font_get_foreground_marker(cairo_scaled_font_t *scaled_font);
void cairo_user_font_face_set_render_color_glyph_func(cairo_font_face_t *font_face, cairo_user_scaled_font_render_glyph_func_t render_glyph_func);

#ifdef __cplusplus
}
#endif

#endif /* CAIRO_H */