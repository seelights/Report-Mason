/*
 * cairo-ft.h - Minimal Cairo FreeType stub for ReportMason project
 * This is a stub implementation to allow compilation without Cairo FreeType
 */

#ifndef CAIRO_FT_H
#define CAIRO_FT_H

#include "cairo.h"

#ifdef __cplusplus
extern "C" {
#endif

/* FreeType types */
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_LibraryRec_* FT_Library;

/* FreeType error codes */
typedef int FT_Error;

/* Cairo FreeType font face */
cairo_font_face_t* cairo_ft_font_face_create_for_ft_face(FT_Face face, int load_flags);
void cairo_ft_font_face_set_synthesize(cairo_font_face_t *font_face, unsigned int synth_flags);
void cairo_ft_font_face_unset_synthesize(cairo_font_face_t *font_face, unsigned int synth_flags);
unsigned int cairo_ft_font_face_get_synthesize(cairo_font_face_t *font_face);

/* FreeType function declarations */
FT_Error FT_Init_FreeType(FT_Library *alibrary);

#ifdef __cplusplus
}
#endif

#endif /* CAIRO_FT_H */
