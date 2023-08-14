#include "fips202.h"
#include "params.h"
#include "convert.h"
#include "Scheme.h"
#include "csprng.hpp"
#include "lest.hpp"
#include "phantom.hpp"
#include "HISE.h"
#include "randombytes.h"
#include "aes_ccm.h"
#include <iostream>

using namespace phantom;
using namespace NTL;

uint8_t seed[N0];
uint8_t srd[N0];
/*some misc functions used for HISE*/

// void print(uint8_t* p){
//     for(int i = 0; i < N0/8; i++){
//         printf("%02x", p[i]);
//     }
//     printf("\n");
// }

bool check_mes(const long* m0, const long* m1)
{
    int cnt = 0;
    for(int i = 0; i < N0; i++)
    {
        if(m0[i]!=m1[i]) cnt++;
        if(cnt>32) return 0;
    }
    return 1;
}
bool check_uint8(uint8_t* d0, uint8_t* d1)
{
    for(int i = 0; i < N0/8; i++)
    {
        if(d0[i]!=d1[i])
        {
            return 0;
        }
    }
    return 1;
}

bool check_C(const long C0[3][N0], const long C1[3][N0])
{
    for(int i = 0; i < N0; i++)
    {
        if((C0[0][i]!=C1[0][i])||(C0[1][i]!=C1[1][i])||(C0[2][i]!=C1[2][i]))
        {
            return 0;
        }
    }
    return 1;
}

static void byte_copy(uint8_t* out, uint8_t* in, size_t len)
{
    for(int i = 0; i < len; i++)
    {
        out[i] = in[i];
    }
}

static void test_cb(size_t len, uint8_t* data)
{
    for (size_t i = 0; i < len; i++) 
    {
        data[i] = seed[i];
    }
}

void set_seed(const uint8_t* in, uint8_t inlen){
    for(int i = 1; i < inlen; i++)
    {
        seed[i] = in[i];
    }
}

void get_rand_drbg(long* buf, long num)
{
    std::unique_ptr<csprng> rng = std::unique_ptr<csprng>(csprng::make(0, &test_cb));
   for (size_t i = 0; i < num; i++){buf[i] = rng.get()->get<uint32_t>();}
}

void GNTRU_IBE_Encrypt_FO(long GNTRU_C[3][N0], const long m[N0], const long id0[N0], const GNTRU_MPK_Data * const GNTRU_MPKD, const uint8_t* rs){
//Encryption: (r * h1 + e1, r * h2 + e2, r * t + e3 + rd{q/2} * m)
//NOTE: error was uniformly chosen from {-1,0,1}    
    memset(seed, 0, N0);
    unsigned long i;
    long r[N0], e1[N0], e2[N0], e3[N0];
    CC_t r_FFT[N0], t_FFT[N0], aux1_FFT[N0], aux2_FFT[N0], aux3_FFT[N0];

    set_seed(rs,N0/8);
    
   get_rand_drbg(e1, N0);
    get_rand_drbg(e2, N0);
    get_rand_drbg(e3, N0);
    get_rand_drbg(r, N0);

    for(i = 0; i < N0; i++)
    {
        e1[i] = (e1[i] % 3) - 1;
       e2[i] = (e2[i] % 3) - 1;
        e3[i] = (e3[i] % 3) - 1;
        r[i] = (r[i] % 3) - 1;
    }
    MyIntFFT(r_FFT, r);
    MyIntFFT(t_FFT, id0);
     // printf("%d\n",2);
   for(i = 0; i < N0; i++)
    {
        aux1_FFT[i] = r_FFT[i]*((GNTRU_MPKD->h1_FFT)[i]);
        aux2_FFT[i] = r_FFT[i]*((GNTRU_MPKD->h2_FFT)[i]);
        aux3_FFT[i] = r_FFT[i]*t_FFT[i];
    }

    MyIntReverseFFT(GNTRU_C[0], aux1_FFT);
    MyIntReverseFFT(GNTRU_C[1], aux2_FFT);
    MyIntReverseFFT(GNTRU_C[2], aux3_FFT);

    for(i = 0; i < N0; i++)
    {
        GNTRU_C[0][i] = (GNTRU_C[0][i] + e1[i]               + q0/2)%q0 - (q0/2);
        GNTRU_C[1][i] = (GNTRU_C[1][i] + e2[i]               + q0/2)%q0 - (q0/2);
        GNTRU_C[2][i] = (GNTRU_C[2][i] + e3[i] + (q0/2)*m[i] + q0/2)%q0 - (q0/2);
    }
}

/*main part of HISE*/

void HISE_Keygen(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD){
    ZZX GNTRU_MSK[9];
    ZZ_pX GNTRU_MPK[2];
    GNTRU_Keygen(GNTRU_MPK, GNTRU_MSK);
    GNTRU_CompleteMSK(GNTRU_MSKD, GNTRU_MSK);
    GNTRU_CompleteMPK(GNTRU_MPKD, GNTRU_MPK);
    randombytes(srd, N0/8);
}

void HISE_Keygen_Api(GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data* GNTRU_MSKD, ZZ_pX GNTRU_MPK[2]){
    ZZX GNTRU_MSK[9];
    GNTRU_Keygen(GNTRU_MPK, GNTRU_MSK);
    GNTRU_CompleteMSK(GNTRU_MSKD, GNTRU_MSK);
    GNTRU_CompleteMPK(GNTRU_MPKD, GNTRU_MPK);
    randombytes(srd, N0/8);
}


void HISE_KEM_Encrypt(uint8_t* K, uint8_t* d, long GNTRU_C[3][N0],const GNTRU_MPK_Data* GNTRU_MPKD){
    long m[N0];
    uint8_t m_0[N0/8];
    long id[N0];
    
    for(int i = 0; i < N0; i++)
    {
        id[i] = 1;//id = 1^N0
    }
    for(int j = 0; j < N0; j++)
    {
        m[j] = (rand() % 2);//roll a random m
    }

    bits_to_uint8(m_0, m, N0);
    // print(m_0);
    GNTRU_IBE_Encrypt_FO(GNTRU_C, m, id, GNTRU_MPKD, m_0);
    sha3_256(K, m_0, N0/8);
    sha3_512(d, m_0, N0/8);
    // print(K);
    // print(d);
}

void HISE_Encrypt(uint8_t* c, size_t& c_len, uint8_t* d ,long GNTRU_C[3][N0], GNTRU_MPK_Data* GNTRU_MPKD, uint8_t* m, size_t m_len){
    uint8_t K[N0/8];
    memset(K, 0, N0/8);
    HISE_KEM_Encrypt(K, d, GNTRU_C, GNTRU_MPKD);
    c_len = ccm_encrypt(m, m_len, K, c) + 14;
}

void HISE_KEM_Decrypt(bool& fail, uint8_t* K, uint8_t* d, long GNTRU_C[3][N0], const GNTRU_MPK_Data* GNTRU_MPKD, const GNTRU_MSK_Data * const GNTRU_MSKD){
    long m[N0], GNTRU_C_tmp[3][N0];
    uint8_t m_1[N0/8], K_1[N0/8], d_1[N0/8];
    long id_1[N0];
    ZZX GNTRU_SK_id[3];
    
    for(int i = 0; i < N0; i++)
    {
        id_1[i] = 1;//id = 1^N0
    }
    vec_ZZ id;
    id.SetLength(N0);
    for(int i = 0; i < N0; i++)
    {
        id[i] = conv<ZZ>(id_1[i])%q1;
    }

    GNTRU_IBE_Extract(GNTRU_SK_id, id, GNTRU_MSKD);
    CC_t GNTRU_SKid1_FFT[N0];
    CC_t GNTRU_SKid2_FFT[N0];
    
    ZZXToFFT(GNTRU_SKid1_FFT, GNTRU_SK_id[1]);
    ZZXToFFT(GNTRU_SKid2_FFT, GNTRU_SK_id[2]);

    GNTRU_IBE_Decrypt(m, GNTRU_C, GNTRU_SKid1_FFT, GNTRU_SKid2_FFT);
    
    bits_to_uint8(m_1, m, N0);
    // print(m_1);
    
    GNTRU_IBE_Encrypt_FO(GNTRU_C_tmp, m, id_1, GNTRU_MPKD, m_1);
    uint8_t buf[5*(N0/8)], K_2[N0/8];
    memset(K_1, 0, N0/8);
    memset(K_2, 0, N0/8);
    byte_copy(buf, srd, N0/8);
    bits_to_uint8(buf + 1*(N0/8), GNTRU_C[0], N0/8);
    bits_to_uint8(buf + 2*(N0/8), GNTRU_C[1], N0/8);
    bits_to_uint8(buf + 3*(N0/8), GNTRU_C[2], N0/8);
    sha3_512(buf + 4*(N0/8), m_1, N0/8);
    sha3_256(K_2, buf, 5*(N0/8));

    sha3_256(K_1, m_1, N0/8);
    sha3_512(d_1, m_1, N0/8);
    if(check_uint8(d, d_1) && check_C(GNTRU_C, GNTRU_C_tmp))
    {
        byte_copy(K, K_1, N0/8);
        fail = 0;
    }
    else
    {
        byte_copy(K, K_2, N0/8);
        fail = 1;
    }
}//some problem here

void HISE_Decrypt(uint8_t* m, int& m_len, uint8_t* d ,long GNTRU_C[3][N0], GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data * const GNTRU_MSKD, uint8_t* c, size_t c_len){
    bool fail;
    uint8_t K[N0/8];
    memset(K, 0, N0/8);
    HISE_KEM_Decrypt(fail, K, d, GNTRU_C, GNTRU_MPKD, GNTRU_MSKD);
    if(fail) m_len = -1;
//    printf("OKKKKKKKKKKKKKK");
    m_len = ccm_decrypt(c, c_len, K, m);
}

void HISE_Sign(ZZX GNTRU_SK_id[3], uint8_t* m, size_t inlen, const GNTRU_MSK_Data * const GNTRU_MSKD){
    uint8_t* m_0 = new uint8_t[inlen + 1];
    long id_1[N0];
    // 0||m to id
    byte_copy(m_0 + 1, m, inlen);
    m_0[0] = '0';
    memset(seed, 0, N0);
    sha3_256(seed, m_0, inlen + 1);
    get_rand_drbg(id_1, N0);
    vec_ZZ id;
    id.SetLength(N0);
    for(int i = 0; i < N0; i++)
    {
        id[i] = conv<ZZ>(id_1[i]*10086)%q1;//一开始没乘
    }
    //std::cout<<id<<std::endl;

    GNTRU_IBE_Extract(GNTRU_SK_id, id, GNTRU_MSKD);
    delete[] m_0;
}

void HISE_Verify(const GNTRU_MPK_Data * const GNTRU_MPKD, const ZZX* GNTRU_SK_id, uint8_t* m, size_t inlen,int& fail){
    uint8_t m_0[100000];
    uint8_t id_0[N0/8];
    long id_1[N0], id0[N0];
    // 0||m to id
    byte_copy(m_0 + 1, m, N0);
    m_0[0] = '0';
    memset(seed, 0, N0);
    sha3_256(seed, m_0, inlen + 1);
    get_rand_drbg(id_1, N0);

    vec_ZZ id;
    id.SetLength(N0);
    for(int i = 0; i < N0; i++)
    {
        id[i] = conv<ZZ>(id_1[i]*10086)%q1;//一开始没乘
    }
    std::cout<<id<<std::endl;
    for(int i = 0; i < N0; i++)
    {
        id0[i] = conv<long int>(id[i]);
    }
    
    long GNTRU_C[3][N0], m1[N0], m_1[N0];
    for(int j = 0; j < N0; j++)
    {
        m_1[j] = (rand() % 2);
    }

    CC_t GNTRU_SKid1_FFT[N0];
    CC_t GNTRU_SKid2_FFT[N0];
    
    ZZXToFFT(GNTRU_SKid1_FFT, GNTRU_SK_id[1]);
    ZZXToFFT(GNTRU_SKid2_FFT, GNTRU_SK_id[2]);
    
    GNTRU_IBE_Encrypt(GNTRU_C, m_1, id0, GNTRU_MPKD);
    GNTRU_IBE_Decrypt(m1, GNTRU_C, GNTRU_SKid1_FFT, GNTRU_SKid2_FFT);
    for(int i=0;i<N0;i++)
    {
       // printf("(%ld  %ld)  ",m1[i],m_1[i]);
    }
    if(check_mes(m_1,m1))
    {
        fail = 0;
        printf("Signature verified.\n");
    }
    else
    {
        fail = -1;
        printf("ERROR: Wrong signature.\n");
    }
    // delete m_0;
}
void HISE_ENC_SIG(uint8_t* c, size_t& c_len, uint8_t* d ,long GNTRU_C[3][N0], GNTRU_MPK_Data* GNTRU_MPKD, uint8_t* m, size_t m_len, const GNTRU_MSK_Data * const GNTRU_MSKD){
    ZZX GNTRU_SK_id[3];
    HISE_Sign(GNTRU_SK_id, m, m_len, GNTRU_MSKD);
    uint8_t sk[2][512*4];
    ZZX_to_uint8(sk[0], GNTRU_SK_id[1], N0);
    ZZX_to_uint8(sk[1], GNTRU_SK_id[2], N0);
    uint8_t* m0 = new uint8_t[m_len + 8*N0];
    byte_copy(m0, m, m_len);
    byte_copy(m0 + m_len + 0*N0, sk[0], 4*N0);
    byte_copy(m0 + m_len + 4*N0, sk[1], 4*N0);

    HISE_Encrypt(c, c_len, d, GNTRU_C, GNTRU_MPKD, m0, m_len + 8*N0);
     delete[] m0;//lx加上的
}

void HISE_DEC_VRFY(uint8_t* m, int& m_len, uint8_t* d ,long GNTRU_C[3][N0], GNTRU_MPK_Data* GNTRU_MPKD, GNTRU_MSK_Data * const GNTRU_MSKD, uint8_t* c, size_t c_len, const GNTRU_MPK_Data * const GNTRU_MPKD_vrfy){
    int m_len0 = 0, fail;
    HISE_Decrypt(m, m_len0, d, GNTRU_C, GNTRU_MPKD, GNTRU_MSKD, c, c_len);
    m_len = m_len0;
    if(m_len == -1) return ;
    ZZX GNTRU_SK_id[3];

    uint8_to_ZZX(GNTRU_SK_id[1], m + (m_len - 8*N0), N0);
    uint8_to_ZZX(GNTRU_SK_id[2], m + (m_len - 4*N0), N0);
    HISE_Verify(GNTRU_MPKD_vrfy, GNTRU_SK_id, m, m_len - 8*N0, fail);
    m_len = m_len - 8*N0;
    if(fail == -1){
        m_len = -2;
        return ;
    }
}
