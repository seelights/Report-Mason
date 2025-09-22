/*
 * fterrors.h
 * FreeType error codes
 */

#ifndef FTERRORS_H_
#define FTERRORS_H_

/* Basic error codes */
#define FT_Err_Ok                    0x00
#define FT_Err_Cannot_Open_Resource  0x01
#define FT_Err_Unknown_File_Format   0x02
#define FT_Err_Invalid_File_Format   0x03
#define FT_Err_Invalid_Version       0x04
#define FT_Err_Lower_Module_Version  0x05
#define FT_Err_Invalid_Argument      0x06
#define FT_Err_Unimplemented_Feature 0x07
#define FT_Err_Invalid_Table         0x08
#define FT_Err_Invalid_Offset        0x09
#define FT_Err_Array_Too_Large       0x0A
#define FT_Err_Missing_Module        0x0B
#define FT_Err_Missing_Property      0x0C

/* Error handling macros */
#define FT_ERROR_BASE(x)  ( (x) & 0xFF )
#define FT_ERROR_MODULE(x)  ( (x) & 0xFF00 )

#endif /* FTERRORS_H_ */