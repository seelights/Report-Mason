#ifndef PK11PUB_H
#define PK11PUB_H

// Minimal stub for pk11pub.h (NSS PK11 public interface)
// This file would normally contain PK11 function declarations

// Constants
#define CKM_SHA_1 0
#define CKM_INVALID_MECHANISM 0

// Function declarations
SECStatus PK11_HashBuf(SECOidTag hashAlg, unsigned char *out, unsigned char *in, int len);
SECStatus PK11_SetPasswordFunc(char *(*passwordFunc)(PK11SlotInfo *slot, PRBool retry, void *arg));
PK11SlotList *PK11_GetAllTokens(CK_MECHANISM_TYPE type, PRBool present, PRBool readOnly, void *wincx);
SECStatus PK11_NeedLogin(PK11SlotInfo *slot);
SECStatus PK11_Authenticate(PK11SlotInfo *slot, PRBool loadCerts, void *wincx);
SECKEYPrivateKeyList *PK11_ListPrivateKeysInSlot(PK11SlotInfo *slot);
CERTCertificate *PK11_GetCertFromPrivateKey(void *privKey);
SECStatus PK11_FreeSlotList(PK11SlotList *list);

#endif
