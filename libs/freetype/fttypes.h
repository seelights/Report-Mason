/*
 * FreeType basic types for ReportMason project
 * This is a minimal type definition to resolve compilation issues
 */

#ifndef FTTYPES_H_
#define FTTYPES_H_

/* Always define types - freetype.h will include this file */

/* Basic integer types */
typedef signed char   FT_Int8;
typedef unsigned char FT_UInt8;
typedef signed short  FT_Int16;
typedef unsigned short FT_UInt16;
typedef signed int    FT_Int32;
typedef unsigned int  FT_UInt32;

/* Position and size types */
typedef long FT_Pos;
typedef long FT_Fixed;
typedef short FT_F2Dot14;
typedef short FT_F26Dot6;

/* Additional integer types */
typedef short FT_Short;
typedef unsigned short FT_UShort;
typedef int FT_Int;
typedef unsigned int FT_UInt;
typedef long FT_Long;
typedef unsigned long FT_ULong;
typedef unsigned char FT_Byte;
typedef int FT_Bool;

/* Character and string types */
typedef char FT_Char;
typedef char FT_String;

/* Error handling */
typedef int FT_Error;

/* Generic structure for user data */
typedef struct FT_Generic_
{
    void* data;
    void (*finalizer)(void*);
} FT_Generic;

/* Vector and geometry types */
typedef struct FT_Vector_
{
    FT_Pos x;
    FT_Pos y;
} FT_Vector;

/* Matrix structure */
typedef struct FT_Matrix_
{
    FT_Fixed xx, xy;
    FT_Fixed yx, yy;
} FT_Matrix;

/* Bitmap structure */
typedef struct FT_Bitmap_
{
    unsigned int rows;
    unsigned int width;
    int pitch;
    unsigned char* buffer;
    unsigned short num_grays;
    unsigned char pixel_mode;
    unsigned char palette_mode;
    void* palette;
} FT_Bitmap;

/* Outline structure */
typedef struct FT_Outline_
{
    short n_contours;
    short n_points;
    FT_Vector* points;
    char* tags;
    short* contours;
    int flags;
} FT_Outline;

/* Glyph format enumeration */
typedef enum FT_Glyph_Format_
{
    FT_GLYPH_FORMAT_NONE = 0,
    FT_GLYPH_FORMAT_COMPOSITE,
    FT_GLYPH_FORMAT_BITMAP,
    FT_GLYPH_FORMAT_OUTLINE,
    FT_GLYPH_FORMAT_PLOTTER
} FT_Glyph_Format;

/* Bitmap size structure */
#ifndef FT_BITMAP_SIZE_DEFINED
#define FT_BITMAP_SIZE_DEFINED
typedef struct FT_Bitmap_Size_
{
    FT_Short height;
    FT_Short width;
    FT_Pos size;
    FT_Pos x_ppem;
    FT_Pos y_ppem;
} FT_Bitmap_Size;
#endif

/* Glyph metrics structure */
#ifndef FT_GLYPH_METRICS_DEFINED
#define FT_GLYPH_METRICS_DEFINED
typedef struct FT_Glyph_Metrics_
{
    FT_Pos width;
    FT_Pos height;
    FT_Pos horiBearingX;
    FT_Pos horiBearingY;
    FT_Pos horiAdvance;
    FT_Pos vertBearingX;
    FT_Pos vertBearingY;
    FT_Pos vertAdvance;
} FT_Glyph_Metrics;
#endif

/* Size metrics structure */
#ifndef FT_SIZE_METRICS_DEFINED
#define FT_SIZE_METRICS_DEFINED
typedef struct FT_Size_Metrics_
{
    FT_UShort x_ppem;
    FT_UShort y_ppem;
    FT_Fixed x_scale;
    FT_Fixed y_scale;
    FT_Pos ascender;
    FT_Pos descender;
    FT_Pos height;
    FT_Pos max_advance;
} FT_Size_Metrics;
#endif

/* Render mode enumeration */
#ifndef FT_RENDER_MODE_DEFINED
#define FT_RENDER_MODE_DEFINED
typedef enum FT_Render_Mode_
{
  FT_RENDER_MODE_NORMAL = 0,
  FT_RENDER_MODE_LIGHT,
  FT_RENDER_MODE_MONO,
  FT_RENDER_MODE_LCD,
  FT_RENDER_MODE_LCD_V,
  FT_RENDER_MODE_SDF,
  FT_RENDER_MODE_MAX
} FT_Render_Mode;
#endif

/* Additional FreeType types needed for compilation */
typedef void* FT_Pointer;

/* Bounding box structure */
typedef struct FT_BBox_
{
  FT_Pos xMin, yMin, xMax, yMax;
} FT_BBox;

/* Memory management structure */
typedef struct FT_MemoryRec_* FT_Memory;

/* Stream structure */
typedef struct FT_StreamRec_* FT_Stream;

/* List structure */
typedef struct FT_ListRec_* FT_ListRec;

/* Driver structure */
typedef struct FT_DriverRec_* FT_Driver;

/* Face internal structure */
typedef struct FT_Face_InternalRec_* FT_Face_Internal;

/* Glyph slot structure */
typedef struct FT_GlyphSlotRec_* FT_GlyphSlot;

/* Size structure */
typedef struct FT_SizeRec_* FT_Size;

/* CharMap structure */
typedef struct FT_CharMapRec_* FT_CharMap;

/* Library structure */
typedef struct FT_LibraryRec_* FT_Library;

/* Module structure */
typedef struct FT_ModuleRec_* FT_Module;

/* Renderer structure */
typedef struct FT_RendererRec_* FT_Renderer;

/* Glyph structure */
typedef struct FT_GlyphRec_* FT_Glyph;

/* Stroker structure */
typedef struct FT_StrokerRec_* FT_Stroker;

/* Face structure */
typedef struct FT_FaceRec_* FT_Face;

/* Basic macros */
#define FT_BEGIN_HEADER
#define FT_END_HEADER

#endif /* FTTYPES_H_ */
