// SPDX-License-Identifier: MIT

#include <cuda_runtime.h>
#include <stdint.h>

#include "FPTRU/kem.h"
#include "FPTRU/params.h"
#include "FPTRU/kem.h"
#include "FPTRU/kernel.h"
#include "FPTRU/main.h"

// 批处理大小
constexpr int KEYSTORE_SIZE = 1000;

// 全局密钥存储 - 使用两个大数组
static uint8_t pk_store[FPTRU_KEM_PUBLICKEYBYTES * KEYSTORE_SIZE];
static uint8_t sk_store[FPTRU_KEM_SECRETKEYBYTES * KEYSTORE_SIZE];
static std::atomic<int> current_index{0};  // 当前可用的密钥索引
static std::atomic<bool> store_initialized{false};
static std::atomic<bool> store_empty{true};

#ifdef __cplusplus
extern "C" {
#endif
int res[BATCH_SIZE];
int cupqc_fptru_761_keypair(uint8_t *pk, uint8_t *sk) {
    fptru_keygen_with_batchsize((unsigned char *)pk, (unsigned char *)sk,1);
    return 0;
}

bool get_keypair_from_store(uint8_t *pk, uint8_t *sk) {
    if (store_empty.load()) {
        // printf("0");
        return false;
    }
    
    int index = current_index.fetch_add(1);
    if (index >= KEYSTORE_SIZE) {
        // 所有密钥都已使用，标记存储为空
        store_empty.store(true);
        // printf("0");
        return false;
    }
    
    // 从存储中复制密钥
    memcpy(pk, pk_store + index * FPTRU_KEM_PUBLICKEYBYTES, FPTRU_KEM_PUBLICKEYBYTES);
    memcpy(sk, sk_store + index * FPTRU_KEM_SECRETKEYBYTES, FPTRU_KEM_SECRETKEYBYTES);
    // printf("1");
    return true;
}

void fill_keystore(){
    fptru_keygen_with_batchsize((unsigned char *)pk_store, (unsigned char *)sk_store, KEYSTORE_SIZE);
    store_empty.store(false);
    current_index.store(0);
    // printf("rerun\n");
}
void initialize_store_if_needed() {
    if (!store_initialized.exchange(true)) {
        fill_keystore();
    }
}

int cupqc_fptru_761_keypair_batch(uint8_t *pk, uint8_t *sk){
    initialize_store_if_needed();

    if (get_keypair_from_store(pk, sk)) {
        return 0;
    }
    
    fill_keystore();
    // 再次尝试从存储中获取密钥对
    if (get_keypair_from_store(pk, sk)) {
        return 0; // 成功从新生成的批处理中获取
    }
    fptru_keygen_with_batchsize((unsigned char *)pk, (unsigned char *)sk,1);
    // printf("fail\n");
    return 0;
    
}

int cupqc_fptru_761_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk) {
    fptru_encaps((unsigned char *)ct, (unsigned char *)ss, (unsigned char *)pk);
    return 0;
}

int cupqc_fptru_761_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk) {
    fptru_decaps((unsigned char *)ss, (unsigned char *)ct, (unsigned char *)sk,res);
    return 0;
}

#ifdef __cplusplus
}
#endif