/*
 * Copyright 2025 Nvidia Corporation
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include <cupqc.hpp>
#include <stdexcept>
#include <oqs/oqsconfig.h>
#include <atomic>
#include <cstring>
#include "cleanversion/main.h"

using namespace cupqc;

// 批处理大小
constexpr int BATCH_SIZE = 1000;

// 全局密钥存储 - 使用两个大数组
static uint8_t* pk_store = nullptr;
static uint8_t* sk_store = nullptr;
static std::atomic<int> current_index{0};  // 当前可用的密钥索引
static std::atomic<bool> store_initialized{false};
static std::atomic<bool> store_empty{true};


// Checks the return value from a CUDA API function
#define CUDA_CHECK(err) \
    if (err != cudaSuccess) { failure = true; goto cleanup; }

template<class MLKEM_Keygen>
__global__ void keygen_kernel(uint8_t *pk, uint8_t *sk, uint8_t *workspace, uint8_t *randombytes) {
    __shared__ uint8_t smem_ptr[MLKEM_Keygen::shared_memory_size];
    MLKEM_Keygen().execute(pk, sk, randombytes, workspace, smem_ptr);
}

// 批处理密钥生成函数
template<class MLKEM_Base>
int keypair_batch(int batch_size) {
    using MLKEM_Keygen = decltype(MLKEM_Base() + Function<function::Keygen>());

    bool failure = false;
    uint8_t *workspace = nullptr, *randombytes = nullptr;
    uint8_t *d_pk = nullptr, *d_sk = nullptr;

    // 如果存储未分配，分配一次性的存储空间
    if (pk_store == nullptr) {
        pk_store = new uint8_t[ML_KEM_512::public_key_size * BATCH_SIZE];
        sk_store = new uint8_t[ML_KEM_512::secret_key_size * BATCH_SIZE];
    }

    // 分配设备内存
    try {
        workspace   = make_workspace<MLKEM_Keygen>(batch_size);
        randombytes = get_entropy<MLKEM_Keygen>(batch_size);
    } catch (const std::runtime_error& ex) {
        failure = true;
        goto cleanup;
    }
    
    CUDA_CHECK(cudaMalloc((void**)&d_pk, MLKEM_Keygen::public_key_size * batch_size));
    CUDA_CHECK(cudaMalloc((void**)&d_sk, MLKEM_Keygen::secret_key_size * batch_size));

    // 运行批处理内核
    keygen_kernel<MLKEM_Keygen><<<batch_size, MLKEM_Keygen::BlockDim>>>(d_pk, d_sk, workspace, randombytes);

    // 等待GPU完成
    CUDA_CHECK(cudaDeviceSynchronize());

    // 复制数据到主机存储
    CUDA_CHECK(cudaMemcpy(pk_store, d_pk, MLKEM_Keygen::public_key_size * batch_size, cudaMemcpyDefault));
    CUDA_CHECK(cudaMemcpy(sk_store, d_sk, MLKEM_Keygen::secret_key_size * batch_size, cudaMemcpyDefault));

    // 重置索引并标记存储为非空
    current_index.store(0);
    store_empty.store(false);

cleanup:
    // 释放设备内存d
    if (d_pk != nullptr) cudaFree(d_pk);
    if (d_sk != nullptr) cudaFree(d_sk);
    if (workspace != nullptr) destroy_workspace(workspace);
    if (randombytes != nullptr) release_entropy(randombytes);
    return failure ? -1 : 0;
}

// 从密钥存储中获取密钥对
bool get_keypair_from_store(uint8_t *pk, uint8_t *sk) {
    if (store_empty.load()) {
        return false;
    }
    
    int index = current_index.fetch_add(1);
    if (index >= BATCH_SIZE) {
        // 所有密钥都已使用，标记存储为空
        store_empty.store(true);
        return false;
    }
    
    // 从存储中复制密钥
    memcpy(pk, pk_store + index * ML_KEM_512::public_key_size, ML_KEM_512::public_key_size);
    memcpy(sk, sk_store + index * ML_KEM_512::secret_key_size, ML_KEM_512::secret_key_size);
    
    return true;
}

template<class MLKEM_Base>
int keypair(uint8_t *pk, uint8_t *sk) {
    printf("batch fail!!!\n");
    using MLKEM_Keygen = decltype(MLKEM_Base() + Function<function::Keygen>());

    bool failure = false;
    uint8_t *workspace = nullptr, *randombytes=nullptr;
    uint8_t *d_pk = nullptr, *d_sk = nullptr;

    // Allocate device workspaces
    try {
        workspace   = make_workspace<MLKEM_Keygen>(1);
        randombytes = get_entropy<MLKEM_Keygen>(1);
    } catch (const std::runtime_error& ex) {
        failure = true;
        goto cleanup;
    }
    CUDA_CHECK(cudaMalloc((void**)&d_pk, MLKEM_Keygen::public_key_size));
    CUDA_CHECK(cudaMalloc((void**)&d_sk, MLKEM_Keygen::secret_key_size));

    // Run routine
    keygen_kernel<MLKEM_Keygen><<<1, MLKEM_Keygen::BlockDim>>>(d_pk, d_sk, workspace, randombytes);

    // Copy data back to the host
    CUDA_CHECK(cudaMemcpy(pk, d_pk, MLKEM_Keygen::public_key_size, cudaMemcpyDefault));
    CUDA_CHECK(cudaMemcpy(sk, d_sk, MLKEM_Keygen::secret_key_size, cudaMemcpyDefault));

cleanup:
    // Free device memory
    if (d_pk != nullptr) cudaFree(d_pk);
    if (d_sk != nullptr) cudaFree(d_sk);
    if (workspace != nullptr) destroy_workspace(workspace);
    if (randombytes != nullptr) release_entropy(randombytes);

    return failure ? -1 : 0;
}

template<class MLKEM_Encaps>
__global__ void encaps_kernel(uint8_t *ct, uint8_t *ss, const uint8_t *pk, uint8_t *workspace, uint8_t *randombytes) {
    __shared__ uint8_t smem_ptr[MLKEM_Encaps::shared_memory_size];
    MLKEM_Encaps().execute(ct, ss, pk, randombytes, workspace, smem_ptr);
}

template<class MLKEM_Base>
int encaps(uint8_t *ct, uint8_t *ss, const uint8_t *pk) {
    using MLKEM_Encaps = decltype(MLKEM_Base() + Function<function::Encaps>());

    bool failure = false;
    uint8_t *workspace = nullptr, *randombytes=nullptr;
    uint8_t *d_ct = nullptr, *d_ss = nullptr, *d_pk = nullptr;

    // Allocate device workspaces
    try {
        workspace   = make_workspace<MLKEM_Encaps>(1);
        randombytes = get_entropy<MLKEM_Encaps>(1);
    } catch (const std::runtime_error& ex) {
        failure = true;
        goto cleanup;
    }
    CUDA_CHECK(cudaMalloc((void**)&d_ct, MLKEM_Encaps::ciphertext_size));
    CUDA_CHECK(cudaMalloc((void**)&d_ss, MLKEM_Encaps::shared_secret_size));
    CUDA_CHECK(cudaMalloc((void**)&d_pk, MLKEM_Encaps::public_key_size));

    // Copy data to GPU
    CUDA_CHECK(cudaMemcpy(d_pk, pk, MLKEM_Encaps::public_key_size, cudaMemcpyDefault));

    // Run routine
    encaps_kernel<MLKEM_Encaps><<<1, MLKEM_Encaps::BlockDim>>>(d_ct, d_ss, d_pk, workspace, randombytes);

    // Copy data back to the host
    CUDA_CHECK(cudaMemcpy(ct, d_ct, MLKEM_Encaps::ciphertext_size, cudaMemcpyDefault));
    CUDA_CHECK(cudaMemcpy(ss, d_ss, MLKEM_Encaps::shared_secret_size, cudaMemcpyDefault));

cleanup:
    // Free device memory
    if (d_ct != nullptr) cudaFree(d_ct);
    if (d_ss != nullptr) cudaFree(d_ss);
    if (d_pk != nullptr) cudaFree(d_pk);
    if (workspace != nullptr) destroy_workspace(workspace);
    if (randombytes != nullptr) release_entropy(randombytes);

    return failure ? -1 : 0;
}

template<class MLKEM_Decaps>
__global__ void decaps_kernel(uint8_t *ss, const uint8_t *ct, const uint8_t *sk, uint8_t *workspace) {
    __shared__ uint8_t smem_ptr[MLKEM_Decaps::shared_memory_size];
    MLKEM_Decaps().execute(ss, ct, sk, workspace, smem_ptr);
}

template<class MLKEM_Base>
int decaps(uint8_t *ss, const uint8_t *ct, const uint8_t *sk) {
    using MLKEM_Decaps = decltype(MLKEM_Base() + Function<function::Decaps>());

    bool failure = false;
    uint8_t *workspace = nullptr;
    uint8_t *d_ct = nullptr, *d_ss = nullptr, *d_sk = nullptr;

    // Allocate device workspaces
    try {
        workspace = make_workspace<MLKEM_Decaps>(1);
    } catch (const std::runtime_error& ex) {
        failure = true;
        goto cleanup;
    }
    CUDA_CHECK(cudaMalloc((void**)&d_ct, MLKEM_Decaps::ciphertext_size));
    CUDA_CHECK(cudaMalloc((void**)&d_ss, MLKEM_Decaps::shared_secret_size));
    CUDA_CHECK(cudaMalloc((void**)&d_sk, MLKEM_Decaps::secret_key_size));

    // Copy data to GPU
    CUDA_CHECK(cudaMemcpy(d_sk, sk, MLKEM_Decaps::secret_key_size, cudaMemcpyDefault));
    CUDA_CHECK(cudaMemcpy(d_ct, ct, MLKEM_Decaps::ciphertext_size, cudaMemcpyDefault));

    // Run routine
    decaps_kernel<MLKEM_Decaps><<<1, MLKEM_Decaps::BlockDim>>>(d_ss, d_ct, d_sk, workspace);

    // Copy data back to the host
    CUDA_CHECK(cudaMemcpy(ss, d_ss, MLKEM_Decaps::shared_secret_size, cudaMemcpyDefault));

cleanup:
    // Free device memory
    if (d_ct != nullptr) cudaFree(d_ct);
    if (d_ss != nullptr) cudaFree(d_ss);
    if (d_sk != nullptr) cudaFree(d_sk);
    if (workspace != nullptr) destroy_workspace(workspace);

    return failure ? -1 : 0;
}

extern "C" {
    using KEM_512  = decltype(ML_KEM_512()  + Block());

#if defined(OQS_ENABLE_KEM_ml_kem_512_cuda)
    // 初始化密钥存储（如果需要）
    void initialize_store_if_needed() {
        if (!store_initialized.exchange(true)) {
            // 首次调用时预先生成一批密钥
            keypair_batch<KEM_512>(BATCH_SIZE);
            //printf("ML_KEM_512 keypair batch size: %d\n", BATCH_SIZE);
                        
            // 调用FPTRU-KEM的main_2函数进行测试
            main_2();
        }
    }
    int cupqc_ml_kem_512_keypair(uint8_t *pk, uint8_t *sk) {
        
        // 初始化密钥存储（如果需要）
        initialize_store_if_needed();
        
        // 尝试从密钥存储中获取密钥对
        if (get_keypair_from_store(pk, sk)) {
            return 0; // 成功从存储中获取
        }
        
        // 如果存储中没有可用密钥，生成一批新密钥
        if (keypair_batch<KEM_512>(BATCH_SIZE) != 0) {
            return -1; // 批处理生成失败
        }
        
        // 再次尝试从存储中获取密钥对
        if (get_keypair_from_store(pk, sk)) {
            return 0; // 成功从新生成的批处理中获取
        }
        // 如果仍然失败，回退到单次生成
        return keypair<KEM_512>(pk, sk);
    }
    int cupqc_ml_kem_512_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk) {
        return encaps<KEM_512>(ct, ss, pk);
    }
    int cupqc_ml_kem_512_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk) {
        return decaps<KEM_512>(ss, ct, sk);
    }
#endif
}
