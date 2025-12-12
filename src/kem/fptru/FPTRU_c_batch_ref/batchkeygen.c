#include <stdio.h>
#include <stdlib.h>
#include "params.h"
#include "poly.h"
#include "pack.h"
#include "inverse.h"
#include "cbd.h"
#include "batchkeygen.h"
#include "string.h"

#if (FPTRU_N == 653)
#include "poly_mul_n653q/radix_ntt_n653.h"
#elif (FPTRU_N == 761)
#include "poly_mul_n761q/radix_ntt_n761.h"
#elif (FPTRU_N == 1277)
#include "poly_mul_n1277q/radix_ntt_n1277.h"
#endif

void keygen_batchtest()
{
  int i, j;
  poly f[N_KEYBATCH], finv[N_KEYBATCH];
  poly fs[N_KEYBATCH], v;
  poly u[N_KEYBATCH], uinv[N_KEYBATCH];
  for(i=0;i<N_KEYBATCH;i++){
    for (int j = 0; j < FPTRU_N; j++){
      f[i].coeffs[j] = rand() % 9;
      f[i].coeffs[j] = f[i].coeffs[j] - 4;
      u[i].coeffs[j] = f[i].coeffs[j];
    }
    poly_inverse(&uinv[i],&u[i]);
  }

  /* calculate finv */
  for( j = 0; j < FPTRU_N; j++) fs[0].coeffs[j] = f[0].coeffs[j];
  for (i = 1; i < N_KEYBATCH; i++) poly_mul_q1(&fs[i],&fs[i-1],&f[i]);
  poly_inverse(&v,&fs[N_KEYBATCH-1]);
  for(i=N_KEYBATCH-1;i>=2;i--) {
    // poly_mul_q1(&finv[i],&v,&fs[i-1]);
    // poly_mul_q1(&finv[i],&v,&f[1]);
    // poly_mul_q1(&finv[i],&finv[i],&f[0]);
    poly_mul_q1(&finv[i],&v,&f[i-1]);
    for(j = i-2; j >= 0; j--){
      poly_mul_q1(&finv[i],&finv[i],&f[j]);
    }
    poly_mul_q1(&v,&v,&f[i]);
  }
  poly_mul_q1(&finv[1],&v,&f[0]);
  poly_mul_q1(&finv[0],&v,&f[1]);

  for(i=0;i<N_KEYBATCH;i++){
    for (int j = 0; j < FPTRU_N; j++){
      if(uinv[i].coeffs[j] != finv[i].coeffs[j]){
        printf("error\n");
        return;
      }
    }
  }
  printf("over\n");
  return;
}

// void crypto_pke_keygen_batch(unsigned char pk[FPTRU_KEM_PUBLICKEYBYTES*N_KEYBATCH],
//                        unsigned char sk[FPTRU_KEM_SECRETKEYBYTES*N_KEYBATCH],
//                        const unsigned char coins[FPTRU_COIN_BYTES*N_KEYBATCH])
void crypto_pke_keygen_batch(unsigned char *pk,
                       unsigned char *sk,
                       const unsigned char coins[FPTRU_COIN_BYTES*N_KEYBATCH])
{
  poly f[N_KEYBATCH], finv[N_KEYBATCH], g[N_KEYBATCH], h[N_KEYBATCH];
  poly fs[N_KEYBATCH], v;
  unsigned int i, j;

  for(i = 0; i < N_KEYBATCH; i++){
    poly_sample(&f[i], coins + i*FPTRU_COIN_BYTES);
    poly_sample(&g[i], coins + (FPTRU_COIN_BYTES / 2) + i*FPTRU_COIN_BYTES);
    poly_double(&f[i], &f[i]); //f=2f
    f[i].coeffs[0] += 1;
  } //HXW 2024-4-16:前面多个多项式的计算

  /* calculate finv */
  for(j = 0; j < FPTRU_N; j++) fs[0].coeffs[j] = f[0].coeffs[j];
  for(i = 1; i < N_KEYBATCH; i++) poly_mul_q1(&fs[i],&fs[i-1],&f[i]);
  poly_inverse(&v,&fs[N_KEYBATCH-1]);
  for(i=N_KEYBATCH-1;i>=2;i--) {
    // poly_mul_q1(&finv[i],&v,&fs[i-1]);
    // poly_mul_q1(&finv[i],&v,&f[1]);
    // poly_mul_q1(&finv[i],&finv[i],&f[0]);
    poly_mul_q1(&finv[i],&v,&f[i-1]);
    for(j = i-2; j > 0; j--){
      poly_mul_q1(&finv[i],&finv[i],&f[j]);
    }
    poly_mul_q1(&finv[i],&finv[i],&f[0]);//这个为什么不直接加过去
    poly_mul_q1(&v,&v,&f[i]);//v往前缩一下
  }
  poly_mul_q1(&finv[1],&v,&f[0]);
  poly_mul_q1(&finv[0],&v,&f[1]);


  for(i = 0; i < N_KEYBATCH; i++){
    poly_mul_q1(&h[i], &finv[i], &g[i]); // h=g*finv=g/f
    poly_fqcsubq(&h[i]);
    pack_pk(pk+i*FPTRU_KEM_PUBLICKEYBYTES, &h[i]);
    pack_sk(sk+i*FPTRU_KEM_SECRETKEYBYTES, &f[i]);
  }
}
