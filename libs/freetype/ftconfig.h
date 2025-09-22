/*
 * FreeType configuration file for ReportMason project
 * This is a minimal configuration to resolve compilation issues
 */

#ifndef FTCONFIG_H_
#define FTCONFIG_H_

/* Basic FreeType configuration */
#define FT_CONFIG_CONFIG_H "ftconfig.h"

/* Platform detection */
#ifdef _WIN32
#define FT_WINDOWS_H
#endif

/* Module system */
#define FT_MODULE_ERRORS_H

/* Memory management */
#define FT_MEMORY_H

/* Basic macros */
#define FT_BEGIN_HEADER
#define FT_END_HEADER

/* Byte casting macro */
#define FT_STATIC_BYTE_CAST(type, value) ((type)(value))

/* Export macro for function declarations */
#define FT_EXPORT(x) x

/* Static cast macro */
#define FT_STATIC_CAST(type, value) ((type)(value))


#endif /* FTCONFIG_H_ */
