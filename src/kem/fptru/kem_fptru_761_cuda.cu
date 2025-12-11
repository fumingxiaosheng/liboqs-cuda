// SPDX-License-Identifier: MIT

#include <cuda_runtime.h>
#include <stdint.h>

#include "FPTRU/kem.h"
#include "FPTRU/params.h"
#include "FPTRU/kem.h"

#ifdef __cplusplus
extern "C" {
#endif

int cupqc_fptru_761_keypair(uint8_t *pk, uint8_t *sk) {
    // 调用FPTRU-761的CUDA实现
    fptru_keygen((unsigned char *)pk, (unsigned char *)sk);
    return 1;
}

int cupqc_fptru_761_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk) {
    // 调用FPTRU-761的CUDA实现
    fptru_encaps((unsigned char *)ct, (unsigned char *)ss, (unsigned char *)pk);
    return 1;
}

int cupqc_fptru_761_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk) {
    // 调用FPTRU-761的CUDA实现
    int res[BATCH_SIZE];
    unsigned char k1[FPTRU_SHAREDKEYBYTES * BATCH_SIZE] = {0};
    fptru_decaps((unsigned char *)ss, (unsigned char *)ct, (unsigned char *)sk,res,k1);
    return 1;
}

#ifdef __cplusplus
}
#endif