#ifndef CRYPTOHI_H
#define CRYPTOHI_H

// Minimal stub for cryptohi.h (NSS crypto high-level interface)
// This file would normally contain crypto high-level function declarations

// Function declarations
SECStatus SECOID_SetAlgorithmID(PLArenaPool *poolp, SECAlgorithmID *aid, SECOidTag tag, void *params);

#endif
