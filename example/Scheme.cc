#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <NTL/ZZ.h>
#include <NTL/ZZX.h>
#include <NTL/mat_ZZ.h>
#include <gmp.h>

#include "Sampling.h"
#include "params.h"
#include "FFT.h"
#include "Random.h"
#include "Algebra.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace NTL;

const ZZX phi = Cyclo();


//GNTRU
//==============================================================================
//Generates from parameters N and q :
// - a public key : polynomial h1, h2
// - a private key : polynomials f1, g1, h1, f2, g2, h2, F, G, H
//==============================================================================
void GNTRU_Keygen(ZZ_pX* GNTRU_PublicKey, ZZX* GNTRU_PrivateKey)
{
    ZZ SqNorm, SqNorm_;
    ZZX f1, g1, h1, f2, g2, h2, F, G, H, ff, gg, hh;

    long logq = NumBits(q0) - 1;
    cout << "logq = " << logq << ", q = "<< q0 << endl;
	double qq = (double) q2;
    SqNorm = conv<ZZ>(1.438 * pow(qq, 0.6667) / 3);
    cout << "SqNorm = " << SqNorm << endl;

    GNTRU_GenerateBasis(f1, g1, h1, f2, g2, h2, F, G, H, ff, gg, hh, SqNorm);
    GNTRU_PrivateKey[0] = f1;
    GNTRU_PrivateKey[1] = g1;
    GNTRU_PrivateKey[2] = h1;
    GNTRU_PrivateKey[3] = f2;
    GNTRU_PrivateKey[4] = g2;
    GNTRU_PrivateKey[5] = h2;
    GNTRU_PrivateKey[6] = F;
    GNTRU_PrivateKey[7] = G;
    GNTRU_PrivateKey[8] = H;

    for(unsigned int i = 0; i < 9; i++)
    {
            GNTRU_PrivateKey[i].SetLength(N0);
    }

    GNTRU_PublicKey[0] = Quotient(ff, gg);
    GNTRU_PublicKey[1] = Quotient(ff, hh);
}

void GNTRU_CompletePrivateKey(mat_ZZ& B, const ZZX * const GNTRU_PrivateKey){
    ZZX f1, g1, h1, f2, g2, h2, F, G, H;
    f1 = GNTRU_PrivateKey[0];
    g1 = GNTRU_PrivateKey[1];
    h1 = GNTRU_PrivateKey[2];
    f2 = GNTRU_PrivateKey[3];
    g2 = GNTRU_PrivateKey[4];
    h2 = GNTRU_PrivateKey[5];
    F = GNTRU_PrivateKey[6];
    G = GNTRU_PrivateKey[7];
    H = GNTRU_PrivateKey[8];
//NOTE: In our case, we do not have to reverse the sign of above polynomials contrary to CompletePrivateKey    
    B = GNTRU_BasisFromPolynomials(f1, g1, h1, f2, g2, h2, F, G, H);
}
void GNTRU_GPV(RR_t * v, const RR_t * const c, const RR_t s, const GNTRU_MSK_Data * const GNTRU_MSKD){
    int i;
    unsigned j;
    RR_t ci[3 * N0], zi, cip, sip, aux;

    for(j = 0; j < 3 * N0; j++)
    {
        ci[j] = c[j];
    } 

    for(i = 3 * N0 - 1; i >= 0; i--)
    {
        aux = (GNTRU_MSKD -> GS_Norms)[i];
        cip = GNTRU_DotProduct(ci, GNTRU_MSKD -> Bstar[i])/(aux * aux);
        sip = s / aux;
        zi = Sample4(cip, sip * PiPrime);

        for(j = 0; j < 3 * N0; j++)
        {
            ci[j] -= zi * (GNTRU_MSKD -> B)[i][j];
        }
    }

    for(j = 0; j < 3 * N0; j++)
    {
        v[j] = c[j] - ci[j];
    }

}
void GNTRU_CompleteMSK(GNTRU_MSK_Data * GNTRU_MSKD, ZZX * GNTRU_MSK){
    unsigned int i, j;
    mat_ZZ B0;

    for(i = 0; i < 9; i++)
    {
        GNTRU_MSKD->PrK[i] = GNTRU_MSK[i];
        ZZXToFFT(GNTRU_MSKD->PrK_fft[i], GNTRU_MSK[i]);
    }

    GNTRU_CompletePrivateKey(B0, GNTRU_MSK);

    for(i = 0; i < 3 * N0; i++)
    {
        for(j = 0; j < 3 * N0; j++)
        {
            GNTRU_MSKD->B[i][j] = ( (RR_t) conv<double>(B0[i][j]) );
        }
    }

    for(i = 0; i < 1; i++)
    {

        GNTRU_FastMGS(GNTRU_MSKD->Bstar, GNTRU_MSKD->B, GNTRU_MSKD->PrK_fft);
        // GNTRU_ClassicMGS(GNTRU_MSKD->Bstar, GNTRU_MSKD->B);
    }

    double* norm = new double[3 * N0];
    for(i = 0; i < 3 * N0; i++){
        norm[i] = GNTRU_DotProduct(GNTRU_MSKD->Bstar[i], GNTRU_MSKD->Bstar[i]);
        norm[i] = sqrt(norm[i]);
    }

    // ofstream output("GSnorm.dat");
    // for(int i = 0; i < 3 * N0; i++){
    //     output << *norm;
    //     if (i < 3 * N0 - 1)
    //         output << "\n";
    //     norm++;
    // }

    for(i = 0; i < 3 * N0; i++)
    {
        GNTRU_MSKD->GS_Norms[i] = sqrt( GNTRU_DotProduct(GNTRU_MSKD->Bstar[i], GNTRU_MSKD->Bstar[i]) );
    }
// TO CHECK: The constant "2" seems to be (actually should be, I think) a smoothing parameter 
    GNTRU_MSKD->sigma = 2 * GNTRU_MSKD->GS_Norms[0];
    // cout << "MSK sigma = " << GNTRU_MSKD->sigma << endl;
}
void GNTRU_CompleteMPK(GNTRU_MPK_Data * GNTRU_MPKD, ZZ_pX * GNTRU_MPK){
    GNTRU_MPKD->h1 = GNTRU_MPK[0];
    GNTRU_MPKD->h2 = GNTRU_MPK[1];
    ZZXToFFT(GNTRU_MPKD->h1_FFT, conv<ZZX>(GNTRU_MPK[0]));
    ZZXToFFT(GNTRU_MPKD->h2_FFT, conv<ZZX>(GNTRU_MPK[1]));
}
void GNTRU_IBE_Extract(ZZX GNTRU_SK_id[3], vec_ZZ id, const GNTRU_MSK_Data * const GNTRU_MSKD){
    unsigned int i;
    RR_t c[3 * N0], sk[3 * N0], sigma;

    sigma = GNTRU_MSKD -> sigma;
    GNTRU_SK_id[0].SetLength(N0);
    GNTRU_SK_id[1].SetLength(N0);
    GNTRU_SK_id[2].SetLength(N0);

    for(i = 0; i < N0; i++)
    {
        c[i] = ((RR_t) conv<double>(id[i])) ;
        c[i + N0] = 0;
        c[i + 2 * N0] = 0;
    }

    GNTRU_GPV(sk, c, sigma, GNTRU_MSKD);

    for(i = 0; i < N0; i++)
    {
        sk[i] = c[i] - sk[i];
        sk[i + N0] = - sk[i + N0];
        sk[i + 2 * N0] = - sk[i + 2 * N0];
    }

    for(i = 0; i < N0; i++)
    {
        GNTRU_SK_id[0][i] = sk[i];
        GNTRU_SK_id[1][i] = sk[i + N0];
        GNTRU_SK_id[2][i] = sk[i + 2 * N0];
    }
    
}
unsigned long GNTRU_IBE_Verify_Key(const ZZX GNTRU_SK_id[3], const vec_ZZ id, const GNTRU_MPK_Data * const GNTRU_MPKD){
//NOTE: I am not sure whether this function gets the master secret key as an input   
    ZZ_pX sk[3], sk2, h1, h2, aux, t;
    ZZX t_ZZ;

    h1 = GNTRU_MPKD -> h1;
    h2 = GNTRU_MPKD -> h2;
    
    t_ZZ = conv<ZZX>(id);
    t = conv<ZZ_pX>(t_ZZ);
    for(int i = 0; i < 3; i++){
        sk[i] = conv<ZZ_pX>(GNTRU_SK_id[i]);
    }
    aux = sk[0] + sk[1] * h1 + sk[2] * h2 - t;

    if( IsZero(aux) != 0)
    {
        cout << "The user secret key (s0,s1,s2) doesn't verify the required equality [ s0 + s1h1 + s2h2 - t = 0 ] !\nActually, s0 + s1h1 + s2h2 - t = " << aux << endl << endl;
    }
    return IsZero(aux);
}
void GNTRU_IBE_Encrypt(long GNTRU_C[3][N0], const long m[N0], const long id0[N0], const GNTRU_MPK_Data * const GNTRU_MPKD){
//Encryption: (r * h1 + e1, r * h2 + e2, r * t + e3 + rd{q/2} * m)
//NOTE: error was uniformly chosen from {-1,0,1}    
    unsigned long i;
    long r[N0], e1[N0], e2[N0], e3[N0];
    CC_t r_FFT[N0], t_FFT[N0], aux1_FFT[N0], aux2_FFT[N0], aux3_FFT[N0];

    for(i = 0; i < N0; i++)
    {
        e1[i] = (rand() % 3) - 1;
        e2[i] = (rand() % 3) - 1;
        e3[i] = (rand() % 3) - 1;
        r[i] = (rand() % 3) - 1;
    }

    MyIntFFT(r_FFT, r);
    MyIntFFT(t_FFT, id0);

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
void GNTRU_IBE_Decrypt(long message[N0], const long GNTRU_C[3][N0], const CC_t * const GNTRU_SKid1_FFT, const CC_t * const GNTRU_SKid2_FFT){
//Decryption: rd{(2/q)*(C[2] - C[0] * s1 - C[1] * s2)} 
//SKid1 = s1, SKid2 = s2 for a user secret key (s0, s1, s2)
    unsigned int i;
    CC_t c0_FFT[N0], c1_FFT[N0], aux_FFT[N0];

    MyIntFFT(c0_FFT, GNTRU_C[0]);
    MyIntFFT(c1_FFT, GNTRU_C[1]);

    for(i = 0; i < N0; i++)
    {
        aux_FFT[i] = c0_FFT[i] * GNTRU_SKid1_FFT[i] + c1_FFT[i] * GNTRU_SKid2_FFT[i];
    }

    MyIntReverseFFT(message, aux_FFT);

    for(i = 0; i < N0; i++)
    {
        message[i] = GNTRU_C[2][i] - message[i];
        message[i] = ((unsigned long)(message[i] )) % q0;
        message[i] = (message[i] + (q0 >> 2) )/(q0 >> 1);
        message[i] %= 2;
    }
}

