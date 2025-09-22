#ifndef SECDER_H
#define SECDER_H

// Minimal stub for secder.h (NSS DER support)
// This file would normally contain DER function declarations

// Constants
#define siBuffer 0

// Function declarations
SECItem *SEC_ASN1EncodeItem(void *arena, void *dest, void *src, SEC_ASN1Template *templatet);
SECStatus SECITEM_FreeItem(SECItem *zap, PRBool freeit);

#endif
