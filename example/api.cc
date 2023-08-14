#include "HISE.h"
#include "Scheme.h"
#include "convert.h"
#include "base64.h"
static void byte_copy(uint8_t* out, uint8_t* in, size_t len)
{
    for(int i = 0; i < len; i++)
    {
        out[i] = in[i];
    }
}

static void MPK_complete(GNTRU_MPK_Data* GNTRU_MPKD, char* MPK){
    ZZ_pX GNTRU_MPK[2];
    uint8_t tmp[N0*8];
    uint64_to_8(tmp, MPK, strlen(MPK));
    uint8_to_ZZpX(GNTRU_MPK[0], tmp + N0*0, N0);
    uint8_to_ZZpX(GNTRU_MPK[1], tmp + N0*4, N0);
    GNTRU_CompleteMPK(GNTRU_MPKD, GNTRU_MPK);
}
void Api_Keygen(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD, char* MPK){
    ZZ_pX GNTRU_MPK[2];
    uint8_t tmp[N0*8];
    HISE_Keygen_Api(GNTRU_MPKD, GNTRU_MSKD, GNTRU_MPK);
    ZZpX_to_uint8(tmp + N0*0, GNTRU_MPK[0], N0);
    ZZpX_to_uint8(tmp + N0*4, GNTRU_MPK[1], N0);
    uint8_to_64(MPK, tmp, N0*8);
}

void Api_Enc(char* MPK, char* c, char* m0, size_t m_len){
    GNTRU_MPK_Data* GNTRU_MPKD = new GNTRU_MPK_Data;
    MPK_complete(GNTRU_MPKD, MPK);

    size_t c_len, t = sizeof(long);
    uint8_t d[64],c0[100000], m[100000], tmp0[3][8*512], tmp[100000];
    long GNTRU_C[3][N0];
    for(int i = 0; i < m_len; i++){
        m[i] = m0[i];
    }

    HISE_Encrypt(c0, c_len, d, GNTRU_C, GNTRU_MPKD, m, m_len);

    long_to_uint8(tmp0[0], GNTRU_C[0], N0);
    long_to_uint8(tmp0[1], GNTRU_C[1], N0);
    long_to_uint8(tmp0[2], GNTRU_C[2], N0);

    byte_copy(tmp              , d      , 64   );
    byte_copy(tmp + 64         , tmp0[0], t*N0 );
    byte_copy(tmp + 64 + 1*t*N0, tmp0[1], t*N0 );
    byte_copy(tmp + 64 + 2*t*N0, tmp0[2], t*N0 );
    byte_copy(tmp + 64 + 3*t*N0, c0     , c_len);

    uint8_to_64(c,tmp,64+3*t*N0+c_len);
    printf("len = %d\n",strlen(c));
//    for(int i = 0; i < 64+3*t*N0+c_len; i++){
//        printf("%02x",tmp[i]);
//    }
//    printf("\n");

//    c[64+3*t*N0+c_len] = 0;
}

int Api_Dec(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD, char* m, char* c0, size_t c0_len){
    size_t t = sizeof(long);
    uint8_t d[64],c[100000], m0[100000], tmp0[3][8*512], tmp[100000];
    size_t c1_len = uint64_to_8(tmp, c0, c0_len);
    size_t c_len = (c1_len-64-3*t*N0);
    memset(d, 0, 64);
    long GNTRU_C[3][N0];

//    uint64_to_8(tmp, c0);
//    for(int i = 0; i < 64+3*t*N0+c_len; i++){
//        printf("%02x",tmp[i]);
//    }
//    printf("\n");
    printf("c0_len = %d\n",c0_len);

    byte_copy(d      , tmp              , 64   );
    byte_copy(tmp0[0], tmp + 64         , t*N0 );
    byte_copy(tmp0[1], tmp + 64 + 1*t*N0, t*N0 );
    byte_copy(tmp0[2], tmp + 64 + 2*t*N0, t*N0 );
    byte_copy(c      , tmp + 64 + 3*t*N0, c_len);

    uint8_to_long(GNTRU_C[0], tmp0[0], N0);
    uint8_to_long(GNTRU_C[1], tmp0[1], N0);
    uint8_to_long(GNTRU_C[2], tmp0[2], N0);

    int m_len;
    HISE_Decrypt(m0, m_len, d, GNTRU_C, GNTRU_MPKD, GNTRU_MSKD, c, c_len);

    for(int i = 0; i < m_len; i++){
        m[i] = m0[i];
    }
    return m_len;
}

void Api_Sign(GNTRU_MSK_Data* GNTRU_MSKD, char* Sign ,char* m0, size_t m_len){
    ZZX GNTRU_SK_id[3];
    uint8_t m[100000],tmp[100000];
    for(int i = 0; i < m_len; i++){
        m[i] = m0[i];
    }
    HISE_Sign(GNTRU_SK_id, m, m_len, GNTRU_MSKD);

    uint8_t sk[2][512*4];
    ZZX_to_uint8 (sk[0], GNTRU_SK_id[1],  N0);
    ZZX_to_uint8 (sk[1], GNTRU_SK_id[2],  N0);
    byte_copy(tmp,        sk[0], N0*4);
    byte_copy(tmp + N0*4, sk[1], N0*4);
    uint8_to_64(Sign, tmp, N0*8);
    // Sign[N0*8]=0;
}

int Api_Vrfy(char* MPK, char* Sign ,char* m0, size_t m_len){
    GNTRU_MPK_Data* GNTRU_MPKD = new GNTRU_MPK_Data;
    MPK_complete(GNTRU_MPKD, MPK);

    uint8_t sk[2][512*4],tmp[100000];
    ZZX GNTRU_SK_id[3];
    uint64_to_8(tmp, Sign, strlen(Sign));
    byte_copy(sk[0], tmp,        N0*4);
    byte_copy(sk[1], tmp + N0*4, N0*4);
    uint8_to_ZZX (GNTRU_SK_id[1], sk[0],  N0);
    uint8_to_ZZX (GNTRU_SK_id[2], sk[1],  N0);

    uint8_t m[100000];
    for(int i = 0; i < m_len; i++){
        m[i] = m0[i];
    }

    int fail;
    HISE_Verify(GNTRU_MPKD, GNTRU_SK_id, m, m_len, fail);
    return fail;
}

void Api_Signcrypt(char* MPK, GNTRU_MSK_Data* GNTRU_MSKD, char* c, char* m0, size_t m_len){
    GNTRU_MPK_Data* GNTRU_MPKD = new GNTRU_MPK_Data;
    MPK_complete(GNTRU_MPKD, MPK);


    size_t c_len, t = sizeof(long);
    uint8_t d[64],c0[100000], m[100000], tmp0[3][8*512],tmp[100000];
    long GNTRU_C[3][N0];
    for(int i = 0; i < m_len; i++){
        m[i] = m0[i];
    }
    HISE_ENC_SIG(c0, c_len, d, GNTRU_C, GNTRU_MPKD, m, m_len, GNTRU_MSKD);

    long_to_uint8(tmp0[0], GNTRU_C[0], N0);
    long_to_uint8(tmp0[1], GNTRU_C[1], N0);
    long_to_uint8(tmp0[2], GNTRU_C[2], N0);

    byte_copy(tmp              , d      , 64   );
    byte_copy(tmp + 64         , tmp0[0], t*N0 );
    byte_copy(tmp + 64 + 1*t*N0, tmp0[1], t*N0 );
    byte_copy(tmp + 64 + 2*t*N0, tmp0[2], t*N0 );
    byte_copy(tmp + 64 + 3*t*N0, c0     , c_len);

    uint8_to_64(c,tmp,64+3*t*N0+c_len);
    printf("c_len1 = %zu\n",c_len);

//    c[64+3*t*N0+c_len] = 0;
}

int Api_Unsigncrypt(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD, char* MPK, char* m, char* c0, size_t c0_len){
    GNTRU_MPK_Data* GNTRU_MPKD_vrfy = new GNTRU_MPK_Data;
    MPK_complete(GNTRU_MPKD_vrfy, MPK);
    uint8_t d[64],c[100000], m0[100000], tmp0[3][8*512], tmp[100000];
    printf("c0_len = %zu\n",c0_len);
    if(c0_len % 4!=0) printf("nonono!!\n");
    size_t t = sizeof(long);
    uint64_to_8(tmp, c0, c0_len);
    size_t c1_len = (c0_len / 4) * 3;
    if (c0[c0_len - 1] == '=') {
        c1_len--;
        if (c0[c0_len - 2] == '=') {
            c1_len--;
        }
    }
    size_t c_len = (c1_len-64-3*t*N0);
    memset(d, 0, 64);
    long GNTRU_C[3][N0];
    printf("c_len = %zu\n",c_len);

    byte_copy(d      , tmp              , 64   );
    byte_copy(tmp0[0], tmp + 64         , t*N0 );
    byte_copy(tmp0[1], tmp + 64 + 1*t*N0, t*N0 );
    byte_copy(tmp0[2], tmp + 64 + 2*t*N0, t*N0 );
    byte_copy(c      , tmp + 64 + 3*t*N0, c_len);
    uint8_to_long(GNTRU_C[0], tmp0[0], N0);
    uint8_to_long(GNTRU_C[1], tmp0[1], N0);
    uint8_to_long(GNTRU_C[2], tmp0[2], N0);
    printf("okkkkkk\n");

    int m_len=0;
    HISE_DEC_VRFY(m0, m_len, d, GNTRU_C, GNTRU_MPKD, GNTRU_MSKD, c, c_len, GNTRU_MPKD_vrfy);
    for(int i = 0; i < m_len; i++){
        m[i] = m0[i];
    }
    return m_len;
}
