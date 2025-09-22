#ifndef GPGME_H
#define GPGME_H

#include <string>
#include <ctime>
#include <cstddef>

namespace GpgME {
       // Forward declarations
       class Error;
       class Signature;
       class VerificationResult;
       class SigningResult;
       class EngineInfo;
       class Data;
       class Key;
       class Subkey;
       class Context;
       
       class Error {
       public:
           Error() = default;
           bool isCanceled() const;
           bool error() const;
           operator bool() const { return error(); }
       };
    
           class Signature {
           public:
               Signature() = default;
               
               class Summary {
               public:
                   Summary() = default;
                   static const int Red = 1;
                   static const int Green = 2;
                   static const int Valid = 4;
                   
                   // Support bitwise operations
                   operator int() const { return 0; }
               };
               
               Key key(bool secret, bool publicKey) const;
               int hashAlgorithm() const;
               time_t creationTime() const;
               Summary summary() const;
           };
           
           class VerificationResult {
           public:
               VerificationResult() = default;
               int numSignatures() const;
               Signature signature(int index) const;
               Error error() const;
           };
           
           class SigningResult {
           public:
               SigningResult() = default;
               Error error() const;
           };
           
           class EngineInfo {
           public:
               EngineInfo() = default;
               std::string engineVersion() const;
           };
           
           class Data {
           public:
               Data() {}
               Data(const char* data, size_t size) {}
               ~Data() {}
               
               void write(const void* data, size_t size);
               void rewind();
               void setEncoding(int encoding);
               std::string toString() const;
               
               static const int BinaryEncoding = 0;
           };
           
           class UserID {
           public:
               UserID() = default;
               const char* id() const;
               const char* email() const;
           };
           
           class Key {
           public:
               Key() = default;
               
               const char* issuerName() const;
               const char* issuerSerial() const;
               UserID userID(int index) const;
               const char* primaryFingerprint() const;
               bool canSign() const;
               bool canEncrypt() const;
               bool canCertify() const;
               Subkey subkey(int index) const;
               bool isNull() const;
               bool isBad() const;
               bool isExpired() const;
               bool isRevoked() const;
           };
           
           class Subkey {
           public:
               Subkey() = default;
               
               time_t creationTime() const;
               time_t expirationTime() const;
               int length() const;
               int publicKeyAlgorithm() const;
               bool isCardKey() const;
               bool isSecret() const;
               
               static const int AlgoDSA = 0;
               static const int AlgoECC = 1;
               static const int AlgoECDH = 2;
               static const int AlgoECDSA = 3;
               static const int AlgoEDDSA = 4;
               static const int AlgoRSA = 5;
               static const int AlgoRSA_E = 6;
               static const int AlgoRSA_S = 7;
               static const int AlgoELG = 8;
               static const int AlgoELG_E = 9;
               static const int AlgoMax = 10;
               static const int AlgoUnknown = 11;
           };
           
           class Context {
           public:
               Context() = default;
               
               static Context* create(int protocol);
               
               void setOffline(bool offline);
               void setKeyListMode(int mode);
               void addSigningKey(const Key& key);
               Key key(const char* keyId, Error& error, bool secret);
               SigningResult sign(const Data& data, Data& signature, int mode);
               VerificationResult verifyDetachedSignature(const Data& signature, const Data& data);
               Error startKeyListing(const char* pattern, bool secretOnly);
               Key nextKey(Error& error);
               Error exportPublicKeys(const char* pattern, Data& data);
           };
           
           // Constants
           const int CMS = 0;
           const int GpgSMEngine = 1;
           
           // KeyListMode constants
           namespace KeyListMode {
               const int Local = 1;
               const int Validate = 2;
           }
           
           // SignatureMode constants
           namespace SignatureMode {
               const int Detached = 1;
           }
           
           // Functions
           void initializeLibrary();
           EngineInfo engineInfo(int engine);
           
           // Hash algorithm constants
           const int GPGME_MD_MD5 = 0;
           const int GPGME_MD_SHA1 = 1;
           const int GPGME_MD_MD2 = 2;
           const int GPGME_MD_SHA256 = 3;
           const int GPGME_MD_SHA384 = 4;
           const int GPGME_MD_SHA512 = 5;
           const int GPGME_MD_SHA224 = 6;
           const int GPGME_MD_NONE = 7;
           const int GPGME_MD_RMD160 = 8;
           const int GPGME_MD_TIGER = 9;
           const int GPGME_MD_HAVAL = 10;
           const int GPGME_MD_MD4 = 11;
           const int GPGME_MD_CRC32 = 12;
           const int GPGME_MD_CRC32_RFC1510 = 13;
           const int GPGME_MD_CRC24_RFC2440 = 14;
           
           // Global constants for compatibility
           const int GPGME_MD_MD5_GLOBAL = GPGME_MD_MD5;
           const int GPGME_MD_SHA1_GLOBAL = GPGME_MD_SHA1;
           const int GPGME_MD_MD2_GLOBAL = GPGME_MD_MD2;
           const int GPGME_MD_SHA256_GLOBAL = GPGME_MD_SHA256;
           const int GPGME_MD_SHA384_GLOBAL = GPGME_MD_SHA384;
           const int GPGME_MD_SHA512_GLOBAL = GPGME_MD_SHA512;
           const int GPGME_MD_SHA224_GLOBAL = GPGME_MD_SHA224;
           const int GPGME_MD_NONE_GLOBAL = GPGME_MD_NONE;
           const int GPGME_MD_RMD160_GLOBAL = GPGME_MD_RMD160;
           const int GPGME_MD_TIGER_GLOBAL = GPGME_MD_TIGER;
           const int GPGME_MD_HAVAL_GLOBAL = GPGME_MD_HAVAL;
           const int GPGME_MD_MD4_GLOBAL = GPGME_MD_MD4;
           const int GPGME_MD_CRC32_GLOBAL = GPGME_MD_CRC32;
           const int GPGME_MD_CRC32_RFC1510_GLOBAL = GPGME_MD_CRC32_RFC1510;
           const int GPGME_MD_CRC24_RFC2440_GLOBAL = GPGME_MD_CRC24_RFC2440;
}

// Global constants for compatibility (outside namespace)
const int GPGME_MD_MD5 = GpgME::GPGME_MD_MD5;
const int GPGME_MD_SHA1 = GpgME::GPGME_MD_SHA1;
const int GPGME_MD_MD2 = GpgME::GPGME_MD_MD2;
const int GPGME_MD_SHA256 = GpgME::GPGME_MD_SHA256;
const int GPGME_MD_SHA384 = GpgME::GPGME_MD_SHA384;
const int GPGME_MD_SHA512 = GpgME::GPGME_MD_SHA512;
const int GPGME_MD_SHA224 = GpgME::GPGME_MD_SHA224;
const int GPGME_MD_NONE = GpgME::GPGME_MD_NONE;
const int GPGME_MD_RMD160 = GpgME::GPGME_MD_RMD160;
const int GPGME_MD_TIGER = GpgME::GPGME_MD_TIGER;
const int GPGME_MD_HAVAL = GpgME::GPGME_MD_HAVAL;
const int GPGME_MD_MD4 = GpgME::GPGME_MD_MD4;
const int GPGME_MD_CRC32 = GpgME::GPGME_MD_CRC32;
const int GPGME_MD_CRC32_RFC1510 = GpgME::GPGME_MD_CRC32_RFC1510;
const int GPGME_MD_CRC24_RFC2440 = GpgME::GPGME_MD_CRC24_RFC2440;

#endif // GPGME_H