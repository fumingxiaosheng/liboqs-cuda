#ifndef KEM_H
#define KEM_H

#include "params.h"
#include "batchkeygen.h"

int crypto_kem_keygen(unsigned char *pk, unsigned char *sk);
int crypto_kem_keygen_fixedbatch(unsigned char *pk,
                      unsigned char *sk);
//   int crypto_kem_keygen_fixedbatch(unsigned char *pk, unsigned char sk[FPTRU_KEM_SECRETKEYBYTES*N_KEYBATCH]);
int crypto_kem_encaps(unsigned char *ct,
                      unsigned char *k,
                      const unsigned char *pk);
int crypto_kem_decaps(unsigned char *k,
                      const unsigned char *ct,
                      const unsigned char *sk);

#endif
