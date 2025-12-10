#include <stdio.h>
#include <time.h>
#include <oqs/oqs.h>

void test_cuda_ml_kem() {
    printf("=== CUDA ML-KEM性能测试 ===\n");
    
    // 初始化liboqs
    OQS_init();
    
    // 创建ML-KEM-512实例（应该使用CUDA版本）
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_512);
    if (kem == NULL) {
        printf("无法创建ML-KEM-512实例\n");
        return;
    }
    
    printf("算法名称: %s\n", kem->method_name);
    printf("公钥长度: %zu\n", kem->length_public_key);
    printf("私钥长度: %zu\n", kem->length_secret_key);
    printf("密文长度: %zu\n", kem->length_ciphertext);
    
    // 分配内存
    uint8_t *public_key = malloc(kem->length_public_key);
    uint8_t *secret_key = malloc(kem->length_secret_key);
    uint8_t *ciphertext = malloc(kem->length_ciphertext);
    uint8_t *shared_secret_enc = malloc(kem->length_shared_secret);
    uint8_t *shared_secret_dec = malloc(kem->length_shared_secret);
    
    if (!public_key || !secret_key || !ciphertext || !shared_secret_enc || !shared_secret_dec) {
        printf("内存分配失败\n");
        goto cleanup;
    }
    
    // 测试密钥生成性能
    printf("\n=== 密钥生成性能测试 ===\n");
    int iterations = 10;
    clock_t start = clock();
    
    for (int i = 0; i < iterations; i++) {
        OQS_STATUS rc = kem->keypair(public_key, secret_key);
        if (rc != OQS_SUCCESS) {
            printf("密钥生成失败 (迭代 %d)\n", i);
            break;
        }
    }
    
    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("密钥生成 %d 次耗时: %.3f秒\n", iterations, time_used);
    printf("平均每次: %.3f毫秒\n", time_used/iterations*1000);
    
    // 测试封装/解封装性能
    printf("\n=== 封装/解封装性能测试 ===\n");
    start = clock();
    
    for (int i = 0; i < iterations; i++) {
        // 封装
        OQS_STATUS rc_enc = kem->encaps(ciphertext, shared_secret_enc, public_key);
        if (rc_enc != OQS_SUCCESS) {
            printf("封装失败 (迭代 %d)\n", i);
            break;
        }
        
        // 解封装
        OQS_STATUS rc_dec = kem->decaps(shared_secret_dec, ciphertext, secret_key);
        if (rc_dec != OQS_SUCCESS) {
            printf("解封装失败 (迭代 %d)\n", i);
            break;
        }
        
        // 验证共享密钥是否匹配
        int match = 1;
        for (size_t j = 0; j < kem->length_shared_secret; j++) {
            if (shared_secret_enc[j] != shared_secret_dec[j]) {
                match = 0;
                break;
            }
        }
        
        if (!match) {
            printf("共享密钥不匹配 (迭代 %d)\n", i);
            break;
        }
    }
    
    end = clock();
    time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("封装/解封装 %d 次耗时: %.3f秒\n", iterations, time_used);
    printf("平均每次: %.3f毫秒\n", time_used/iterations*1000);
    
cleanup:
    // 清理内存
    free(public_key);
    free(secret_key);
    free(ciphertext);
    free(shared_secret_enc);
    free(shared_secret_dec);
    OQS_KEM_free(kem);
    OQS_destroy();
}

int main() {
    test_cuda_ml_kem();
    return 0;
}