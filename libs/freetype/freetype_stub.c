//========================================================================
//
// FreeType stub implementation
//
// This file provides stub implementations for FreeType functions
// used by Poppler
//
//========================================================================

#include "freetype.h"
#include <stdlib.h>
#include <string.h>

// Remove conflicting definitions
#undef FT_Err_Ok
#undef FT_Err_Invalid_Library_Handle
#undef FT_Err_Invalid_Face_Handle

#define FT_Err_Ok 0
#define FT_Err_Invalid_Library_Handle 1
#define FT_Err_Invalid_Face_Handle 2

FT_Error FT_Init_FreeType(FT_Library *alibrary)
{
    if (alibrary) {
        // Use a reasonable size for the library structure
        *alibrary = (FT_Library)malloc(1024);
        if (*alibrary) {
            memset(*alibrary, 0, 1024);
            return FT_Err_Ok;
        }
    }
    return FT_Err_Invalid_Library_Handle;
}

FT_Error FT_Done_FreeType(FT_Library library)
{
    if (library) {
        free(library);
        return FT_Err_Ok;
    }
    return FT_Err_Invalid_Library_Handle;
}

void FT_Library_Version(FT_Library library, FT_Int *amajor, FT_Int *aminor, FT_Int *apatch)
{
    (void)library; // Suppress unused parameter warning
    if (amajor) *amajor = 2;
    if (aminor) *aminor = 13;
    if (apatch) *apatch = 0;
}

FT_Error FT_New_Face(FT_Library library, const char *filepathname, FT_Long face_index, FT_Face *aface)
{
    (void)library; // Suppress unused parameter warning
    (void)filepathname; // Suppress unused parameter warning
    (void)face_index; // Suppress unused parameter warning
    
    if (aface) {
        // Use a reasonable size for the face structure
        *aface = (FT_Face)malloc(1024);
        if (*aface) {
            memset(*aface, 0, 1024);
            return FT_Err_Ok;
        }
    }
    return FT_Err_Invalid_Face_Handle;
}

FT_Error FT_New_Memory_Face(FT_Library library, const FT_Byte *file_base, FT_Long file_size, FT_Long face_index, FT_Face *aface)
{
    (void)library; // Suppress unused parameter warning
    (void)file_base; // Suppress unused parameter warning
    (void)file_size; // Suppress unused parameter warning
    (void)face_index; // Suppress unused parameter warning
    
    if (aface) {
        // Use a reasonable size for the face structure
        *aface = (FT_Face)malloc(1024);
        if (*aface) {
            memset(*aface, 0, 1024);
            return FT_Err_Ok;
        }
    }
    return FT_Err_Invalid_Face_Handle;
}

FT_Error FT_Done_Face(FT_Face face)
{
    if (face) {
        free(face);
        return FT_Err_Ok;
    }
    return FT_Err_Invalid_Face_Handle;
}

FT_UInt FT_Get_Name_Index(FT_Face face, const char *glyph_name)
{
    (void)face; // Suppress unused parameter warning
    (void)glyph_name; // Suppress unused parameter warning
    return 0;
}

// Stub for ft_new_face_from_file function
FT_Error ft_new_face_from_file(FT_Library library, const char *filepathname, FT_Long face_index, FT_Face *aface)
{
    return FT_New_Face(library, filepathname, face_index, aface);
}
