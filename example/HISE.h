#include "params.h"

void HISE_Keygen(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD);
void HISE_Keygen_Api(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD, ZZ_pX GNTRU_MPK[2]);

void HISE_KEM_Encrypt(uint8_t* K, uint8_t* d, long GNTRU_C[3][N0], const GNTRU_MPK_Data* GNTRU_MPKD);
void HISE_KEM_Decrypt(bool& fail, uint8_t* K, uint8_t* d, long GNTRU_C[3][N0], const GNTRU_MPK_Data* GNTRU_MPKD, const GNTRU_MSK_Data * const GNTRU_MSKD);

void HISE_Encrypt(uint8_t* c, size_t& c_len, uint8_t* d ,long GNTRU_C[3][N0], GNTRU_MPK_Data* GNTRU_MPKD, uint8_t* m, size_t m_len);
void HISE_Decrypt(uint8_t* m, int& m_len, uint8_t* d ,long GNTRU_C[3][N0], GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data * const GNTRU_MSKD, uint8_t* c, size_t c_len);

void HISE_Sign(ZZX GNTRU_SK_id[3], uint8_t* m, size_t inlen, const GNTRU_MSK_Data * const GNTRU_MSKD);
void HISE_Verify(const GNTRU_MPK_Data * const GNTRU_MPKD, const ZZX* GNTRU_SK_id, uint8_t* m, size_t inlen,int& fail);

void HISE_ENC_SIG(uint8_t* c, size_t& c_len, uint8_t* d ,long GNTRU_C[3][N0], GNTRU_MPK_Data* GNTRU_MPKD, uint8_t* m, size_t m_len, const GNTRU_MSK_Data * const GNTRU_MSKD);
void HISE_DEC_VRFY(uint8_t* m, int& m_len, uint8_t* d ,long GNTRU_C[3][N0], GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data * const GNTRU_MSKD, uint8_t* c, size_t c_len, const GNTRU_MPK_Data * const GNTRU_MPKD_vrfy);
