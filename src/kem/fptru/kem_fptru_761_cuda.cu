// SPDX-License-Identifier: MIT

#include <cuda_runtime.h>
#include <stdint.h>

#include "FPTRU/kem.h"
#include "FPTRU/params.h"
#include "FPTRU/kem.h"
#include "FPTRU/kernel.h"
#include "FPTRU/main.h"

#ifdef __cplusplus
extern "C" {
#endif
int res[BATCH_SIZE];
int cupqc_fptru_761_keypair(uint8_t *pk, uint8_t *sk) {
    // main_2();
    // printf("main2 finish\n");
    // // 调用FPTRU-761的CUDA实现
    fptru_keygen_with_batchsize((unsigned char *)pk, (unsigned char *)sk,1);
    //输出pk的内容
    //printf("outside complete\n");
    return 0;
}

int cupqc_fptru_761_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk) {
    // 调用FPTRU-761的CUDA实现
    //printf("cupqc_fptru_761_enc\n");
    fptru_encaps((unsigned char *)ct, (unsigned char *)ss, (unsigned char *)pk);
    //printf("Shared secret (ss) content (hex): ");
    // for (int i = 0; i < 32; i++) {
    //     //printf("%02x", ss[i]);
    // }
    // //printf("\n");

    // //分别输出ct和pk的内容
    // //printf("Ciphertext (ct) content (hex): ");
    // for (int i = 0; i < 128; i++) {
    //     //printf("%02x", ct[i]);
    // }
    // //printf("\n");
    
    // //printf("Public key (pk) content (hex): ");
    // for (int i = 0; i < 128; i++) {
    //     //printf("%02x", pk[i]);
    // }
    // //printf("\n");
    
    // //printf("cupqc_fptru_761_enc done\n");
    return 0;
}

int cupqc_fptru_761_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk) {
    // 调用FPTRU-761的CUDA实现
    //printf("cupqc_fptru_761_dec\n");
    fptru_decaps((unsigned char *)ss, (unsigned char *)ct, (unsigned char *)sk,res);
    // //输出ss里面的内容
    // //printf("Shared secret (ss) content (hex): ");
    // for (int i = 0; i < 32; i++) {
    //     //printf("%02x", ss[i]);
    // }
    // //输出ct的内容
    // //printf("\n");
    // //printf("Ciphertext (ct) content (hex): ");
    // for (int i = 0; i < 128; i++) {
    //     //printf("%02x", ct[i]);
    // }
    // //输出sk的内容
    // //printf("\n");
    // //printf("Secret key (sk) content (hex): ");
    // for (int i = 0; i < 128; i++) {
    //     //printf("%02x", sk[i]);
    // }
    // //printf("\n");
    
    // //printf("*res = %d\n", res[0]);
    // //printf("cupqc_fptru_761_dec done\n");
    return 0;
}

#ifdef __cplusplus
}
#endif