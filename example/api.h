#include "params.h"

void Api_Keygen(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD, char* MPK);

void Api_Enc(char* MPK, char* c, char* m0, size_t m_len);
int  Api_Dec(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD, char* m0, char* c0, size_t c0_len);

void Api_Sign(GNTRU_MSK_Data* GNTRU_MSKD, char* Sign ,char* m0, size_t m_len);
int  Api_Vrfy(char* MPK, char* Sign ,char* m0, size_t m_len);

void Api_Signcrypt(char* MPK, GNTRU_MSK_Data* GNTRU_MSKD, char* c, char* m0, size_t m_len);
int  Api_Unsigncrypt(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD, char* MPK, char* m, char* c0, size_t c0_len);