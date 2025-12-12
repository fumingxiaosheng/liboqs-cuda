#include "params.h"

#if !defined(N_KEYBATCH)
#define N_KEYBATCH (32)
#endif

void crypto_pke_keygen_batch(unsigned char *pk,
                       unsigned char *sk,
                       const unsigned char coins[FPTRU_COIN_BYTES*N_KEYBATCH]);
// void crypto_pke_keygen_batch(unsigned char pk[FPTRU_KEM_PUBLICKEYBYTES*N_KEYBATCH],
//                        unsigned char sk[FPTRU_KEM_SECRETKEYBYTES*N_KEYBATCH],
//                        const unsigned char coins[FPTRU_COIN_BYTES*N_KEYBATCH]);
void keygen_batchtest();