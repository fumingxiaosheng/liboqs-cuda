#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "params.h"
#include "kem.h"
#include "batchkeygen.h"

#define NTESTS 1000

void test_kem()
{
  // unsigned int i, j;
  // unsigned char k1[FPTRU_SHAREDKEYBYTES], k2[FPTRU_SHAREDKEYBYTES];
  // unsigned char pk[FPTRU_KEM_PUBLICKEYBYTES] = {0}, sk[FPTRU_KEM_SECRETKEYBYTES] = {0};
  // unsigned char ct[FPTRU_KEM_CIPHERTEXTBYTES] = {0};
  unsigned int i, j, k;
  unsigned char k1[FPTRU_SHAREDKEYBYTES*N_KEYBATCH], k2[FPTRU_SHAREDKEYBYTES*N_KEYBATCH];
  unsigned char pk[FPTRU_KEM_PUBLICKEYBYTES*N_KEYBATCH], sk[FPTRU_KEM_SECRETKEYBYTES*N_KEYBATCH];
  unsigned char ct[FPTRU_KEM_CIPHERTEXTBYTES*N_KEYBATCH];

  for (i = 0; i < NTESTS; i++)
  {
    crypto_kem_keygen_fixedbatch(pk, sk);
    for(k = 0; k < N_KEYBATCH; k++){
      crypto_kem_encaps(ct + k*FPTRU_KEM_CIPHERTEXTBYTES, k1 + k*FPTRU_SHAREDKEYBYTES, pk + k*FPTRU_KEM_PUBLICKEYBYTES);
      crypto_kem_decaps(k2 + k*FPTRU_SHAREDKEYBYTES, ct + k*FPTRU_KEM_CIPHERTEXTBYTES, sk + k*FPTRU_KEM_SECRETKEYBYTES);

      for (j = 0; j < FPTRU_SHAREDKEYBYTES*N_KEYBATCH; j++)
        if (k1[j] != k2[j])
        {
          printf("Round %d. Failure: Keys dont match: %hhx != %hhx!\n", i, k1[j], k2[j]);
          return;
        }
    }
    // crypto_kem_keygen(pk, sk);
    // crypto_kem_encaps(ct, k1, pk);
    // crypto_kem_decaps(k2, ct, sk);

    // for (j = 0; j < FPTRU_SHAREDKEYBYTES; j++)
    //   if (k1[j] != k2[j])
    //   {
    //     printf("Round %d. Failure: Keys dont match: %hhx != %hhx!\n", i, k1[j], k2[j]);
    //     return;
    //   }
  }

  printf("FPTRU-%d-KEM is correct!\n", FPTRU_N);

  printf("Test %d times.\n\n", NTESTS);
  printf("FPTRU_N = %d, FPTRU_Q = %d, FPTRU_Q2 = %d\n", FPTRU_N, FPTRU_Q, FPTRU_Q2);
  printf("KEM size: pk = %d bytes, ct = %d bytes, bandwidth = %d bytes\n\n",
         FPTRU_KEM_PUBLICKEYBYTES, FPTRU_KEM_CIPHERTEXTBYTES,
         FPTRU_KEM_PUBLICKEYBYTES + FPTRU_KEM_CIPHERTEXTBYTES);
}

int main()
{
  test_kem();
  return 0;
}
