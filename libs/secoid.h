#ifndef SECOID_H
#define SECOID_H

// Minimal stub for secoid.h (NSS OID support)
// This file would normally contain OID definitions and functions

// Forward declarations
typedef struct SECOidDataStr SECOidData;

// Function declarations
SECOidTag SECOID_FindOidTag(SECItem *oid);

#endif // SECOID_H
