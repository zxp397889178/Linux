#ifndef ECDSA_SECP256K1_H
#define ECDSA_SECP256K1_H

#include <string>
#include <sstream>
#include <stdio.h>
#include <vector>

#include <secp256k1.h>
#include <secp256k1_recovery.h>

class ECDSAWrapper
{
public:
    static unsigned char * sha256(unsigned char * md, const char * msg);
    static unsigned char * randomfill(unsigned char * blob, size_t len);
    static unsigned char * readhex(unsigned char * dat, const char * hex);
    static void readhexEx(std::vector<uint8_t>& vchSig, const char * hex);
    static int verify(const std::vector<uint8_t>& vchSig, const char * msg, const std::vector<uint8_t>& vchPub);
    static unsigned char * recovery_pubkey(unsigned char * pubkey, unsigned char * compact_signature, const char * msg);
    static unsigned char * make_public(unsigned char * pub, const unsigned char * priv);
    static unsigned char * sign( unsigned char * compact_signature, const char * msg, const unsigned char * raw_priv);
    static int ecdsa_signature_parse_der_lax(
        const secp256k1_context* ctx,
        secp256k1_ecdsa_signature* sig,
        const unsigned char *input,
        size_t inputlen
    ) SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2) SECP256K1_ARG_NONNULL(3);
};
#endif
