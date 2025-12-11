// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include <oqs/kem_fptru.h>

#if defined(OQS_ENABLE_KEM_fptru_761)

OQS_KEM *OQS_KEM_fptru_761_new(void) {

	OQS_KEM *kem = OQS_MEM_malloc(sizeof(OQS_KEM));
	if (kem == NULL) {
		return NULL;
	}
	kem->method_name = OQS_KEM_alg_fptru_761;
	kem->alg_version = "FPTRU-761";

	kem->claimed_nist_level = 1;
	kem->ind_cca = true;

	kem->length_public_key = OQS_KEM_fptru_761_length_public_key;
	kem->length_secret_key = OQS_KEM_fptru_761_length_secret_key;
	kem->length_ciphertext = OQS_KEM_fptru_761_length_ciphertext;
	kem->length_shared_secret = OQS_KEM_fptru_761_length_shared_secret;
	kem->length_keypair_seed = OQS_KEM_fptru_761_length_keypair_seed;

	kem->keypair = OQS_KEM_fptru_761_keypair;
	kem->keypair_derand = OQS_KEM_fptru_761_keypair_derand;
	kem->encaps = OQS_KEM_fptru_761_encaps;
	kem->decaps = OQS_KEM_fptru_761_decaps;

	return kem;
}

#if defined(OQS_USE_CUPQC)
#if defined(OQS_ENABLE_KEM_fptru_761_cuda)
extern int cupqc_fptru_761_keypair(uint8_t *pk, uint8_t *sk);
extern int cupqc_fptru_761_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk);
extern int cupqc_fptru_761_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk);
#endif
#endif /* OQS_USE_CUPQC */

OQS_API OQS_STATUS OQS_KEM_fptru_761_keypair_derand(uint8_t *public_key, uint8_t *secret_key, const uint8_t *seed) {
#if defined(OQS_USE_CUPQC) && defined(OQS_ENABLE_KEM_fptru_761_cuda)
	return (OQS_STATUS) cupqc_fptru_761_keypair(public_key, secret_key);
#endif /* OQS_USE_CUPQC && OQS_ENABLE_KEM_fptru_761_cuda */
	// 默认使用参考实现
	return OQS_ERROR;
}

OQS_API OQS_STATUS OQS_KEM_fptru_761_keypair(uint8_t *public_key, uint8_t *secret_key) {
#if defined(OQS_USE_CUPQC) && defined(OQS_ENABLE_KEM_fptru_761_cuda)
	return (OQS_STATUS) cupqc_fptru_761_keypair(public_key, secret_key);
#endif /* OQS_USE_CUPQC && OQS_ENABLE_KEM_fptru_761_cuda */
	// 默认使用参考实现
	return OQS_ERROR;
}

OQS_API OQS_STATUS OQS_KEM_fptru_761_encaps(uint8_t *ciphertext, uint8_t *shared_secret, const uint8_t *public_key) {
#if defined(OQS_USE_CUPQC) && defined(OQS_ENABLE_KEM_fptru_761_cuda)
	return (OQS_STATUS) cupqc_fptru_761_enc(ciphertext, shared_secret, public_key);
#endif /* OQS_USE_CUPQC && OQS_ENABLE_KEM_fptru_761_cuda */
	// 默认使用参考实现
	return OQS_ERROR;
}

OQS_API OQS_STATUS OQS_KEM_fptru_761_decaps(uint8_t *shared_secret, const uint8_t *ciphertext, const uint8_t *secret_key) {
#if defined(OQS_USE_CUPQC) && defined(OQS_ENABLE_KEM_fptru_761_cuda)
	return (OQS_STATUS) cupqc_fptru_761_dec(shared_secret, ciphertext, secret_key);
#endif /* OQS_USE_CUPQC && OQS_ENABLE_KEM_fptru_761_cuda */
	// 默认使用参考实现
	return OQS_ERROR;
}

#endif