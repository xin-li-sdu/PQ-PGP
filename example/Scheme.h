#ifndef LIBE_SCHEME_H
#define LIBE_SCHEME_H

#include "params.h"
#include "Sampling.h"
#include "FFT.h"
#include "Random.h"
#include "Algebra.h"


//GNTRU
void GNTRU_Keygen(ZZ_pX* GNTRU_PublicKey, ZZX* GNTRU_PrivateKey);
void GNTRU_CompletePrivateKey(mat_ZZ& B, const ZZX * const GNTRU_PrivateKey);
void GNTRU_GPV(RR_t * v, const RR_t * const c, const RR_t s, const GNTRU_MSK_Data * const GNTRU_MSKD);
void GNTRU_CompleteMSK(GNTRU_MSK_Data * GNTRU_MSKD, ZZX * GNTRU_MSK);
void GNTRU_CompleteMPK(GNTRU_MPK_Data * GNTRU_MPKD, ZZ_pX * GNTRU_MPK);
void GNTRU_IBE_Extract(ZZX GNTRU_SK_id[3], vec_ZZ id, const GNTRU_MSK_Data * const GNTRU_MSKD);
unsigned long GNTRU_IBE_Verify_Key(const ZZX GNTRU_SK_id[3], const vec_ZZ id, const GNTRU_MPK_Data * const GNTRU_MPKD);
void GNTRU_IBE_Encrypt(long GNTRU_C[3][N0], const long m[N0], const long id0[N0], const GNTRU_MPK_Data * const GNTRU_MPKD);
void GNTRU_IBE_Decrypt(long message[N0], const long GNTRU_C[3][N0], const CC_t * const GNTRU_SKid1_FFT, const CC_t * const GNTRU_SKid2_FFT);

#endif
