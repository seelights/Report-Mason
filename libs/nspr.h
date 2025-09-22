/*
 * nspr.h - Minimal NSPR stub for ReportMason project
 */

#ifndef NSPR_H
#define NSPR_H

typedef int PRInt32;
typedef unsigned int PRUint32;
typedef long PRInt64;
typedef unsigned long PRUint64;

// Constants
#define PR_LANGUAGE_I_DEFAULT 0

// Function declarations
const char *PR_ErrorToString(int error, int language);

#endif
