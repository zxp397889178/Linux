#include <ecdsa_secp256k1.h>

#include "BaseCode.h"
#include "Security.h"

//#include <openssl/sha.h>

#define SHA256_DIGEST_LENGTH 32



unsigned char * ECDSAWrapper::randomfill(unsigned char * blob, size_t len) {
	FILE *fr = fopen("/dev/urandom", "rb");
	fread((void *) blob, sizeof(unsigned char), len, fr);
	if (ferror(fr)) {
		LogWarn("there has been some error loading randomness\n");
	}
	fclose(fr);
	return blob;
}

unsigned char * ECDSAWrapper::sha256(unsigned char * md, const char * msg) {

    uint8_t hash[SHA256_DIGEST_LENGTH] = {0};
    size_t cbHash = sizeof(hash);
    ShaWrapper::SHA256((const uint8_t *)msg, strlen(msg), hash, cbHash);
    memcpy(md, hash, cbHash);
    return md;
}

unsigned char * ECDSAWrapper::readhex(unsigned char * dat, const char * hex) {
	int n = strlen(hex);
	int i;
	unsigned tmp;
	if (n % 2) {
		LogWarn("There must not be odd number of hex-characters\n");
		return dat;
	}
	for (i = 0; i < n/2; ++i) {
		if (1 != sscanf(hex + 2*i, "%02x", &tmp)) {
			LogWarn("bad hex string\n");
			return dat;
		}
		dat[i] = (unsigned char) tmp;
	}
	return dat;
}

void ECDSAWrapper::readhexEx(std::vector<uint8_t>& vchSig, const char * hex) {
	int n = strlen(hex);
	int i;
	unsigned tmp;
	if (n % 2) {
		LogWarn("There must not be odd number of hex-characters\n");
		return ;
	}
	for (i = 0; i < n/2; ++i) {
		if (1 != sscanf(hex + 2*i, "%02x", &tmp)) {
			LogWarn("bad hex string\n");
			return ;
		}
        vchSig.push_back((unsigned char) tmp);
	}
}

unsigned char * ECDSAWrapper::sign(unsigned char * compact_signature, const char * msg, const unsigned char * raw_priv) {
	unsigned char md[SHA256_DIGEST_LENGTH], seed[1024];
	secp256k1_context * ctx_sign;
	secp256k1_ecdsa_signature sig_t;
	
	ctx_sign = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
	if (! secp256k1_context_randomize(ctx_sign, randomfill(seed, 32))) printf("problem with randomization");
	secp256k1_ecdsa_sign(ctx_sign, &sig_t, sha256(md, msg), raw_priv, NULL, randomfill(seed, 1024));
	secp256k1_ecdsa_signature_serialize_compact(ctx_sign, compact_signature, &sig_t);
	secp256k1_context_destroy(ctx_sign);
	return compact_signature;
}

unsigned char * ECDSAWrapper::recovery_pubkey(unsigned char * pubkey, unsigned char * compact_signature, const char * msg) {

    LogWarn("[%s:%d]recovery_pubkey, pubkey[%s] compact_signature[%s] msg[%s]", __FILE__, __LINE__,  pubkey, compact_signature, msg);

	secp256k1_context * ctx;
	secp256k1_ecdsa_signature sig_t;
	unsigned char md[SHA256_DIGEST_LENGTH];
	secp256k1_pubkey pub_t;
	long l = 33;
	ctx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
	if (!secp256k1_ecdsa_recoverable_signature_parse_compact(ctx, (secp256k1_ecdsa_recoverable_signature*)&sig_t, compact_signature, 0)) {
		LogWarn("Given data failed to be parsed as signature\n");
		return pubkey;
	}
	if (!secp256k1_ecdsa_recover(ctx, &pub_t, (const secp256k1_ecdsa_recoverable_signature*)&sig_t, sha256(md, msg))) {
		LogWarn("Failed to restore public key\n");
		return pubkey;
	}

	secp256k1_ec_pubkey_serialize(ctx, pubkey, (size_t*)&l, &pub_t, SECP256K1_EC_COMPRESSED);
	return pubkey;
}

int ECDSAWrapper::ecdsa_signature_parse_der_lax(const secp256k1_context* ctx, secp256k1_ecdsa_signature* sig, const unsigned char *input, size_t inputlen) 
{
    size_t rpos, rlen, spos, slen;
    size_t pos = 0;
    size_t lenbyte;
    unsigned char tmpsig[64] = {0};
    int overflow = 0;

    /* Hack to initialize sig with a correctly-parsed but invalid signature. */
    secp256k1_ecdsa_signature_parse_compact(ctx, sig, tmpsig);

    /* Sequence tag byte */
    if (pos == inputlen || input[pos] != 0x30) {
        return 0;
    }
    pos++;

    /* Sequence length bytes */
    if (pos == inputlen) {
        return 0;
    }
    lenbyte = input[pos++];
    if (lenbyte & 0x80) {
        lenbyte -= 0x80;
        if (lenbyte > inputlen - pos) {
            return 0;
        }
        pos += lenbyte;
    }

    /* Integer tag byte for R */
    if (pos == inputlen || input[pos] != 0x02) {
        return 0;
    }
    pos++;

    /* Integer length for R */
    if (pos == inputlen) {
        return 0;
    }
    lenbyte = input[pos++];
    if (lenbyte & 0x80) {
        lenbyte -= 0x80;
        if (lenbyte > inputlen - pos) {
            return 0;
        }
        while (lenbyte > 0 && input[pos] == 0) {
            pos++;
            lenbyte--;
        }
        if (lenbyte >= sizeof(size_t)) {
            return 0;
        }
        rlen = 0;
        while (lenbyte > 0) {
            rlen = (rlen << 8) + input[pos];
            pos++;
            lenbyte--;
        }
    } else {
        rlen = lenbyte;
    }
    if (rlen > inputlen - pos) {
        return 0;
    }
    rpos = pos;
    pos += rlen;

    /* Integer tag byte for S */
    if (pos == inputlen || input[pos] != 0x02) {
        return 0;
    }
    pos++;

    /* Integer length for S */
    if (pos == inputlen) {
        return 0;
    }
    lenbyte = input[pos++];
    if (lenbyte & 0x80) {
        lenbyte -= 0x80;
        if (lenbyte > inputlen - pos) {
            return 0;
        }
        while (lenbyte > 0 && input[pos] == 0) {
            pos++;
            lenbyte--;
        }
        if (lenbyte >= sizeof(size_t)) {
            return 0;
        }
        slen = 0;
        while (lenbyte > 0) {
            slen = (slen << 8) + input[pos];
            pos++;
            lenbyte--;
        }
    } else {
        slen = lenbyte;
    }
    if (slen > inputlen - pos) {
        return 0;
    }
    spos = pos;

    /* Ignore leading zeroes in R */
    while (rlen > 0 && input[rpos] == 0) {
        rlen--;
        rpos++;
    }
    /* Copy R value */
    if (rlen > 32) {
        overflow = 1;
    } else if (rlen) {
        memcpy(tmpsig + 32 - rlen, input + rpos, rlen);
    }

    /* Ignore leading zeroes in S */
    while (slen > 0 && input[spos] == 0) {
        slen--;
        spos++;
    }
    /* Copy S value */
    if (slen > 32) {
        overflow = 1;
    } else if (slen) {
        memcpy(tmpsig + 64 - slen, input + spos, slen);
    }

    if (!overflow) {
        overflow = !secp256k1_ecdsa_signature_parse_compact(ctx, sig, tmpsig);
    }
    if (overflow) {
        memset(tmpsig, 0, 64);
        secp256k1_ecdsa_signature_parse_compact(ctx, sig, tmpsig);
    }
    return 1;
}

int ECDSAWrapper::verify(const std::vector<uint8_t>& vchSig, const char * msg, const std::vector<uint8_t>& vchPub)
{
    unsigned char md[SHA256_DIGEST_LENGTH];
	secp256k1_context * ctx_sign;
	secp256k1_ecdsa_signature sig_t;
	secp256k1_pubkey pub_t;
	ctx_sign = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
	if ( (int)vchPub[0] != 2 && (int)vchPub[0] != 3 && (int)vchPub[0] != 4) {
		LogWarn("invalid public key\n");
		return 0;
	}
	if (! secp256k1_ec_pubkey_parse(ctx_sign, &pub_t, &vchPub[0], vchPub.size())) {
		LogWarn("pubkey not parsed\n");
		return 0;
	}

	if (!ECDSAWrapper::ecdsa_signature_parse_der_lax(ctx_sign, &sig_t, &vchSig[0], vchSig.size())) {
		LogWarn("der signature not parsed\n");
		return 0;
	}
	secp256k1_ecdsa_signature_normalize(ctx_sign, &sig_t, &sig_t);

	return secp256k1_ecdsa_verify(ctx_sign, &sig_t, sha256(md, msg), &pub_t);
}

unsigned char * ECDSAWrapper::make_public(unsigned char * pub, const unsigned char * priv) {
	secp256k1_context * ctx;
	secp256k1_pubkey pub_t;
	ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
	if (! secp256k1_ec_pubkey_create(ctx, &pub_t, priv)) {
		LogWarn("problem with pubkey creation");
		return NULL;
	}

	size_t l = 33;
	secp256k1_ec_pubkey_serialize(ctx, pub, &l, &pub_t, SECP256K1_EC_COMPRESSED);
	if ( l != 33) {
		LogWarn("written %ld bytes\n", l);
		return NULL;
	}
	return pub;
}
