// SPDX-License-Identifier: MIT

#ifndef OQS_KEM_FPTRU_H
#define OQS_KEM_FPTRU_H

#include <oqs/oqs.h>

#if defined(OQS_ENABLE_KEM_fptru_761)
#define OQS_KEM_fptru_761_length_public_key 1237
#define OQS_KEM_fptru_761_length_secret_key 1650
#define OQS_KEM_fptru_761_length_ciphertext 952
#define OQS_KEM_fptru_761_length_shared_secret 32
#define OQS_KEM_fptru_761_length_keypair_seed 32
OQS_KEM *OQS_KEM_fptru_761_new(void);
OQS_API OQS_STATUS OQS_KEM_fptru_761_keypair(uint8_t *public_key, uint8_t *secret_key);
OQS_API OQS_STATUS OQS_KEM_fptru_761_keypair_derand(uint8_t *public_key, uint8_t *secret_key, const uint8_t *seed);
OQS_API OQS_STATUS OQS_KEM_fptru_761_encaps(uint8_t *ciphertext, uint8_t *shared_secret, const uint8_t *public_key);
OQS_API OQS_STATUS OQS_KEM_fptru_761_decaps(uint8_t *shared_secret, const uint8_t *ciphertext, const uint8_t *secret_key);
#endif

#endif