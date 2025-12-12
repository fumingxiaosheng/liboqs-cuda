#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "kem.h"
#include "params.h"
#include "pke.h"
#include "poly.h"
#include "cpucycles.h"
#include "speed.h"
#include "randombytes.h"
#include "pack.h"
#include "symmetric_crypto.h"
#include "batchkeygen.h"
#include "reduce.h"

#define NTESTS 10000

uint64_t t[NTESTS];
int16_t num16[NTESTS];



void test_speed_kem()
{
  printf("\n");

  printf("FPTRU-%d-%d-KEM\n\n", FPTRU_N, FPTRU_Q);

  unsigned int i;
  unsigned char pk[FPTRU_KEM_PUBLICKEYBYTES*N_KEYBATCH], sk[FPTRU_KEM_SECRETKEYBYTES*N_KEYBATCH], ct[FPTRU_KEM_CIPHERTEXTBYTES];
  unsigned char k1[FPTRU_SHAREDKEYBYTES], k2[FPTRU_SHAREDKEYBYTES];

  /*for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    crypto_kem_keygen(pk, sk);
  }
  print_results("FPTRU_kem_keygen_onetime: ", t, NTESTS);*/

  for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    for(int j = 0; j <N_KEYBATCH; j++){
      crypto_kem_keygen(pk, sk);
    }
  }
  print_results("FPTRU_kem_keygen_serial_N_KEYBATCH: ", t, NTESTS);

  for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    crypto_kem_keygen_fixedbatch(pk, sk);
  }
  print_results("FPTRU_kem_keygen_batch_N_KEYBATCH: ", t, NTESTS);

  /*for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    crypto_kem_encaps(ct, k1, pk);
  }
  print_results("FPTRU_kem_encaps: ", t, NTESTS);

  for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    crypto_kem_decaps(k2, ct, sk);
  }
  print_results("FPTRU_kem_decaps: ", t, NTESTS);*/

/* test inverse speed*/
// #if !defined(N_KEYBATCH)
// #define N_KEYBATCH (10)
// #endif
//   int j,k;
//   poly f[N_KEYBATCH], finv[N_KEYBATCH];
//   poly fs[N_KEYBATCH], v;
//   poly g[N_KEYBATCH], ginv[N_KEYBATCH];
//   for(k=0;k<N_KEYBATCH;k++){
//     for (int j = 0; j < FPTRU_N; j++){
//       f[k].coeffs[j] = rand() % 9;
//       f[k].coeffs[j] = f[k].coeffs[j] - 4;
//       g[k].coeffs[j] = f[k].coeffs[j];
//     }
//   }

//   for (i = 0; i < NTESTS; i++)
//   {
//     t[i] = cpucycles();
//     for( j = 0; j < FPTRU_N; j++) fs[0].coeffs[j] = f[0].coeffs[j];
//     for (k = 1; k < N_KEYBATCH; k++) poly_mul_q1(&fs[k],&fs[k-1],&f[k]);
//     poly_inverse(&v,&fs[N_KEYBATCH-1]);
//     for(k=N_KEYBATCH-1;k>=2;k--) {
//       poly_mul_q1(&finv[k],&v,&f[k-1]);
//       for(j = k-2; j >= 0; j--){
//         poly_mul_q1(&finv[k],&finv[k],&f[j]);
//       }
//       poly_mul_q1(&v,&v,&f[k]);
//     }
//     poly_mul_q1(&finv[1],&v,&f[0]);
//     poly_mul_q1(&finv[0],&v,&f[1]);
//   }
//   print_results("poly_inverse_batch: ", t, NTESTS);
  
//   for (i = 0; i < NTESTS; i++)
//   {
//     t[i] = cpucycles();
//     for(k=0;k<N_KEYBATCH;k++){
//       poly_inverse(&ginv[k],&g[k]);
//     }
//   }
//   print_results("poly_inverse: ", t, NTESTS);
}

#if (FPTRU_N == 653)
#include "poly_mul_n653q/radix_ntt_n653.h"
#elif (FPTRU_N == 761)
#include "poly_mul_n761q/radix_ntt_n761.h"
#elif (FPTRU_N == 1277)
#include "poly_mul_n1277q/radix_ntt_n1277.h"
#endif

void test_reduce(){
  int i=0;
  uint64_t hxw1,hxw2;

  hxw1 = cpucycles();
  for (i = 0; i < NTESTS; i++)
  {
    //t[i] = cpucycles();
    //printf("(%d,%d) ",i,barrett_reduce_int32_t_n653(i));

#if (FPTRU_N == 653)
    barrett_reduce_int32_t_n653(num16[i]);
#elif (FPTRU_N == 761)
    barrett_reduce_int32_t_n761(num16[i]);
#endif
  }
  hxw2 = cpucycles();
  //print_results("barret_reduction;", t, NTESTS);
  printf("barret_reduction:%lld\n",hxw2-hxw1);


  hxw1 = cpucycles();
  for (i = 0; i < NTESTS; i++)
  {
    //t[i] = cpucycles();
#if (FPTRU_N == 653)
  pseudomersenne_reduce_single_n653(num16[i]);
#elif (FPTRU_N == 761)
  pseudomersenne_reduce_single_n761(num16[i]);
#elif (FPTRU_N == 1277)
  pseudomersenne_reduce_single_n1277(num16[i]);
#endif

  }
  hxw2 = cpucycles();
  //print_results("pseudomersenne_reduce_single_n761;", t, NTESTS);
  printf("pseudomersenne_reduce_single:%lld\n",hxw2-hxw1);

  hxw1 = cpucycles();
  for (i = 0; i < NTESTS; i++)
  {
#if (FPTRU_N == 653)
  montgomery_reduce_n653(num16[i]);
#elif (FPTRU_N == 761)
  montgomery_reduce_n761(num16[i]);
#elif (FPTRU_N == 1277)
  montgomery_reduce_n1277(i);
#endif
  }
  hxw2 = cpucycles();
  //print_results("montgomery_reduce_n761;", t, NTESTS);
  printf("montgomery_reduce:%lld\n",hxw2-hxw1);

  /*hxw1 = cpucycles();
  for (i = 0; i < NTESTS; i++)
  {
    //t[i] = cpucycles();
    montgomery_reduce_int16_t(i);
  }
  hxw2 = cpucycles();
  //print_results("montgomery_reduce_n761;", t, NTESTS);
  printf("montgomery_reduce_int16_t:%lld\n",hxw2-hxw1);*/

  hxw1 = cpucycles();
  for (i = 0; i < NTESTS; i++)
  {
    //t[i] = cpucycles();
#if (FPTRU_N == 653)
  pseudomersenne_reduce_double_n653(num16[i]);
#elif (FPTRU_N == 761)
  pseudomersenne_reduce_double_n761(num16[i]);
#elif (FPTRU_N == 1277)
  pseudomersenne_reduce_double_n1277(i);
#endif
  }
  hxw2 = cpucycles();
  //print_results("pseudomersenne_reduce_double_n761;", t, NTESTS);
  printf("pseudomersenne_reduce_double_n761:%lld\n",hxw2-hxw1);

}

void read_file(){
   FILE *file = fopen("./data_process/random_int32_values.txt", "r");

    // 检查文件是否成功打开
    if (file == NULL) {
        perror("无法打开文件");
        return 1;
    }

    // 读取文件中的每一个数
    int number;
    int i = 0;
    while (fscanf(file, "%d", &number) == 1) {
        num16[i]=number;
    }

    // 关闭文件
    fclose(file);
}
int main()
{ printf("hxw\n");
  //read_file();
  //test_reduce();
  test_speed_kem();
  //keygen_batchtest();
  return 0;
}
