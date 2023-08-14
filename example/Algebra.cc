#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <NTL/ZZ.h>
#include <NTL/ZZX.h>
#include <NTL/mat_ZZ.h>
#include <gmp.h>

#include "Algebra.h"
#include "params.h"
#include "FFT.h"
#include "Random.h"

using namespace std;
using namespace NTL;


ZZX FastMod(const ZZX& f)
{
    return (trunc(f,N0) - (f>>N0));
}

ZZX Cyclo()
{
    ZZX phi0;
    phi0.SetLength(N0+1);
    phi0[0] = 1;
    phi0[N0] = 1;
    return phi0;
}

const ZZX phi = Cyclo();


//==============================================================================
//Compute N(f) in Z[y]/(y^(N/2)+1) from f in Z[x]/(x^N+1)
//==============================================================================
ZZX OneStepNorm(const ZZX& f, const unsigned int N){
	// res(y) = even(f)^2 - y * odd(f)^2 for y = x^2
	ZZX even, odd, res;
	unsigned int half = N / 2;
	even.SetLength(half);
	odd.SetLength(half);
	for(int i = 0; i < half; i++){
		even[i] = f[2 * i];
		odd[i] = f[2 * i - 1];
	}
	even = even * even;
	even = trunc(even, half) - (even >> half);
	odd = odd * odd;
	odd = trunc(odd, half) - (odd >> half);
	for(int i = half - 1; i > 0; i--){
		odd[i] = odd[i - 1];
	}
	odd[0] = -1 * odd[half - 1];
	res = even - odd;
	return res;
}


//==============================================================================
//Computes the squared norm of a polynomial f   
//==============================================================================
ZZ SquaredNorm(const ZZX& f, const unsigned int degree)
{
    unsigned int i;
    ZZ somme;
    for(i=0; i<=degree; i++)
    {
        somme += sqr(f[i]);
    }
    return somme;
}


//==============================================================================
//Verifies that for a parameter N, polynomials f, g are a valid semi-basis for building a NTRU lattice.
//If PGCD!=1, then (f,g) isn't a valid pair
//==============================================================================
void ValidPair(ZZ& PGCD, ZZ& Alpha, ZZ& Beta, ZZX& rho_f, ZZX& rho_g, const ZZX& f, const ZZX& g)
{
    ZZX Res_fx, Res_gx, iphi;
    ZZ Res_f, Res_g; 

    XGCD(Res_f, rho_f, iphi, f, phi, 0);
    if(GCD(Res_f, q1)!=1)
    {
        PGCD = 0;
    }
    else
    {    XGCD(Res_g, rho_g, iphi, g, phi, 0);
         XGCD(PGCD, Alpha, Beta, Res_f, Res_g);
    }
}


//==============================================================================
//Computes f(1/x) mod (x^N + 1)
//If f = a0 + a1*x + ... + a_{N-1}*x^{N-1}, then
//Reverse(f) = a0 - a_{N-1}*x + ... - a1*x^{N-1}
//==============================================================================
ZZX Reverse(const ZZX& f)
{
    assert(deg(f) >= 0);
    assert(deg(f) < N0);

    ZZX fb;
    unsigned int i;
    fb.SetLength(N0);
    fb[0] = f[0];
    fb.SetLength(N0);
    for(i = N0 - deg(f); i < N0; i++)
    {
        fb[i] = -f[N0 - i];
    }
    fb[0] = f[0];
    return fb;
}




//==============================================================================
//Computes the polynomial k such that (F,G) <-- (F,G) - k*(f,g) minimizes the size of (F,G)
//==============================================================================
ZZX ReductionCoefficient(const ZZX& f, const ZZX& g, const ZZX& F, const ZZX& G, unsigned int & mb)
{
    unsigned int i;
    ZZ a;
    ZZX fb, gb, num, den, iden, iphi, k;

    fb = Reverse(f);
    gb = Reverse(g);
    num = FastMod(fb*F + gb*G);
    den = FastMod(f*fb + g*gb);
    mb = MaxBits(num);


    XGCD(a, iden, iphi, den, phi);

    // cout << "Resultant of den = " << a << endl;
    k = FastMod(num*iden);

    k.SetLength(N0);
    for(i=0; i<N0; i++)
    {
        k[i] /= a;
    }

    return k;
}


//==============================================================================
//Computes the polynomial k such that (F,G) <-- (F,G) - k*(f,g) minimizes the size of (F,G)
//==============================================================================
ZZX FastReductionCoefficient(const ZZX& f, const ZZX& g, const ZZX& F, const ZZX& G)
{
    unsigned int i;
    ZZX k;
    CC_t f_FFT[N0], g_FFT[N0], F_FFT[N0], G_FFT[N0], num_FFT[N0], den_FFT[N0], k_FFT[N0];

    assert(MaxBits(f)<900);
    ZZXToFFT(f_FFT, f);

    assert(MaxBits(g)<900);
    ZZXToFFT(g_FFT, g);

    assert(MaxBits(F)<900);
    ZZXToFFT(F_FFT, F);

    assert(MaxBits(G)<900);
    ZZXToFFT(G_FFT, G);

    for(i=0; i<N0; i++)
    {
        num_FFT[i] = f_FFT[N0-1-i]*F_FFT[i] + g_FFT[N0-1-i]*G_FFT[i];
        den_FFT[i] = f_FFT[N0-1-i]*f_FFT[i] + g_FFT[N0-1-i]*g_FFT[i];
        k_FFT[i] = num_FFT[i]/den_FFT[i];
    }

    FFTToZZX(k, k_FFT);
    return k;
}



//==============================================================================
//Returns the anticircular matrix associated to integral polynomial f and integer N
//==============================================================================
mat_ZZ AnticircularMatrix(const ZZX& f)
{
    unsigned int i,j;
    int df;
    mat_ZZ M;
    M.SetDims(N0, N0);
    df = deg(f);
    if(df==-1)
    {
        return M;
    }
    unsigned dfu;
    dfu = ((unsigned) df);
    if(dfu>=N0)
    {
        cout << "df = " << dfu << endl;
        cout << "f = " << f << endl;
    }
    assert(dfu<N0);


    for(i=0; i<N0; i++)
    {
        for(j=i; ((j<=dfu+i)&&(j<N0)); j++)
        {
            M[i][j] = f[j-i];
        }
        for(j=0; (j+N0)<=(dfu+i); j++)
        {
            M[i][j] = -f[j-i+N0];
        }
    }
    return M;
}



//==============================================================================
//Generates a basis from the double pair (f,g), (F,G) and N
//This basis has the form :
//    |f g|
//M = |F G|
//==============================================================================
mat_ZZ BasisFromPolynomials(const ZZX& f, const ZZX& g, const ZZX& F, const ZZX& G)
{
    unsigned int i,j;
    mat_ZZ A,M;
    M.SetDims(2*N0, 2*N0);
    A = AnticircularMatrix(f);
    for(i=0; i<N0; i++){
    for(j=0; j<N0; j++){
        M[i][j] = A[i][j];
    }}

    A = AnticircularMatrix(g);
    for(i=0; i<N0; i++){
    for(j=0; j<N0; j++){
        M[i][j+N0] = A[i][j];
    }}

    A = AnticircularMatrix(F);
    for(i=0; i<N0; i++){
    for(j=0; j<N0; j++){
        M[i+N0][j] = A[i][j];
    }}

    A = AnticircularMatrix(G);
    for(i=0; i<N0; i++){
    for(j=0; j<N0; j++){
        M[i+N0][j+N0] = A[i][j];
    }}

    return M;
}



//==============================================================================
//Computes the Inverse of f (mod phi) (mod q)
//==============================================================================
ZZ_pX Inverse(const ZZX& f)
{
    ZZ_p::init(q1);
    ZZX rho_f, iphi;
    ZZ Res_f;
    ZZ_p Res_f_1;
    XGCD(Res_f, rho_f, iphi, f, phi, 0);    
    inv(Res_f_1, conv<ZZ_p>(Res_f));
    assert(Res_f_1*conv<ZZ_p>(Res_f) == 1);

    return ( Res_f_1 * conv<ZZ_pX>(rho_f) );
}


//==============================================================================
//Computes h = g/f (mod phi) (mod q)
//==============================================================================
ZZ_pX Quotient(const ZZX& f, const ZZX& g)
{
    ZZ_pX f_1, g0, h0, phi0;
    f_1 = Inverse(f);
    g0 = conv<ZZ_pX>(g);
    phi0 = conv<ZZ_pX>(phi);
    h0 = (f_1*g0)%phi0;
    return h0;
}



//==============================================================================
//Computes the Gram-Schmidt norm of the basis B generated from f,g
//==============================================================================
void GS_Norm(const ZZX fx, const ZZX gx, int& flag)
{
    unsigned int i;

    double acc, acc3, Fred[N0], Gred[N0];
    CC_t f[N0], g[N0], F[N0], G[N0];

    acc = 0;
    for(i=0; i<N0; i++)
    {
        acc += conv<double>(fx[i]*fx[i] + gx[i]*gx[i]);
    }
    acc = sqrt(acc);

    ZZXToFFT(f, fx);
    ZZXToFFT(g, gx);

    for(i=0; i<N0; i++)
    {
        F[i] = f[i]/(f[i]*f[N0-1-i]+g[i]*g[N0-1-i]);
        G[i] = g[i]/(f[i]*f[N0-1-i]+g[i]*g[N0-1-i]);
    }
    MyRealReverseFFT(Fred, F);
    MyRealReverseFFT(Gred, G);

    acc3 = 0;
    for(i=0; i<N0; i++)
    {
        acc3 += Fred[i]*Fred[i] + Gred[i]*Gred[i];
    }
    acc3 = q0*sqrt(acc3);
    cout << "acc = " << acc << endl;
    cout << "acc3 = " << acc3 << endl;
    if(acc3<acc)
    {
        flag = 1;
    }
}



//==============================================================================
//Generates a secret basis (f,g),(F,G) from the parameters N,q,Norme
//This bases generates a NTRU lattice
//==============================================================================
void GenerateBasis(ZZX& f, ZZX& g, ZZX& F, ZZX& G, const ZZ& Norme)
{
    ZZX rho_f, rho_g, k, aux, fb, gb, num;
    ZZ PGCD, Alpha, Beta;

    int flag = 0;

    while( (PGCD!=1) || (flag==0) )
    {
        flag = 0;
        f = RandomPolyFixedSqNorm(Norme,N0-1);
        g = RandomPolyFixedSqNorm(Norme,N0-1);
        GS_Norm(f, g, flag);
        ValidPair(PGCD, Alpha, Beta, rho_f, rho_g, f, g);
    }
    F = -q1*Beta*rho_g;
    G = q1*Alpha*rho_f;

    cout << "F, G maxbits = " << MaxBits(F) <<", " << MaxBits(G) << endl;

    f.SetLength(N0);
    g.SetLength(N0);

    unsigned int mb;
    k = ReductionCoefficient(f, g, F, G, mb);


    while(deg(k) >= 0)
    {
        F = FastMod(F - k*f);
        G = FastMod(G - k*g);

        cout << "F, G maxbits = " << MaxBits(F) <<", " << MaxBits(G) << endl;


        fb = Reverse(f);
        gb = Reverse(g);

        num = FastMod(fb*F + gb*G);
        mb = MaxBits(num);


        k = ReductionCoefficient(f, g, F, G, mb);
        k.normalize();
    }

    aux = FastMod(f*G - g*F);

    assert(aux[0]==q1);
    assert(deg(aux)==0);
    aux.SetLength(N0);
}


RR_t DotProduct(const RR_t * x1, const RR_t * x2)
{
    unsigned int i;
    RR_t rep = 0;
    for(i=0; i<2*N0; i++)
    {
        rep += x1[i]*x2[i];
    }
    return rep;
}


void Rotate(RR_t * const dest, RR_t const * const src)
{
    unsigned int i;
    for(i=0; i<N0-1; i++)
    {
        dest[i+1] = src[i];
        dest[N0+i+1] = src[N0+i];
    }
    dest[0] = -src[N0-1];
    dest[N0] = -src[2*N0-1];
}



void ClassicMGS(RR_t Bstar[2*N0][2*N0], const RR_t B[2*N0][2*N0])
{
    RR_t SquareNorm[2*N0], aux[2*N0];
    unsigned int i,j,k;

    SquareNorm[0] = DotProduct(B[0], B[0]);
    for(j=0; j<2*N0; j++)
    {

        Bstar[0][j] = B[0][j];
    }

    for(i=1; i<2*N0; i++)
    {
        for(k=0; k<2*N0; k++)
        {
            Bstar[i][k] = B[i][k];
        }
        for(j=0; j<i; j++)
        {
            aux[j]= DotProduct(Bstar[i], Bstar[j]) / SquareNorm[j];
        }
        for(k=0; k<2*N0; k++)
        {
            for(j=0; j<i; j++)
            {
                Bstar[i][k] -= aux[j]*Bstar[j][k];
            }
        }
        SquareNorm[i] = DotProduct(Bstar[i], Bstar[i]);
    }
}


void FastMGS(RR_t Bst[2*N0][2*N0], const RR_t B[2*N0][2*N0])
{
    RR_t v[2*N0], v1[2*N0], C_k, D_k, C_ko, D_ko, aux;
    //RR_t C[2*N0], D[2*N0];
    unsigned int j, k;

    cout << endl;
    //Reducing first vector (obvious)
    for(j=0; j<2*N0; j++)
    {    Bst[0][j] = B[0][j];    }


    //Initialising the vector v = b_N - Proj(b_N, (b_1...b_k-2) )
    for(j=0; j<N0-1; j++)
    {    v[j] = Bst[0][j+1];
         v[j+N0] = Bst[0][j+1+N0];    }
    v[N0-1] = -Bst[0][0];
    v[2*N0-1] = -Bst[0][N0];

    for(j=0; j<2*N0; j++)
    {    v1[j] = v[j];    }


    //Initialising recurring variables
    C_k = DotProduct(Bst[0], v);
    D_k = DotProduct(v, v);

    //C[0] = C_k;
    //D[0] = D_k;
    //CD[0] = C[0]/D[0];


    //Reducing b_2 to b_N and updating v at the same time
    for(k=1; k<N0; k++)
    {
        //b~k <-- r(b~_{k-1}) - <b~_{k-1},b_N>/<v_{k-1},b_N> r(v)
        aux = C_k/D_k;
        Bst[k][0] = -Bst[k-1][N0-1] + aux*v[N0-1];
        Bst[k][N0] = -Bst[k-1][2*N0-1] + aux*v[2*N0-1];
        for(j=1; j<N0; j++)
        {
            Bst[k][j] = Bst[k-1][j-1] - aux*v[j-1];
            Bst[k][j+N0] = Bst[k-1][j+N0-1] - aux*v[j+N0-1];
        }

        //v <-- v - Proj(v, b~_{k-1} )
        for(j=0; j<2*N0; j++)
        {
            v[j] -= aux*Bst[k-1][j];
        }
        //sqnorm_v -= aux*aux*SquareNorm[k-1];

        C_ko = C_k;
        D_ko = D_k;

        C_k = DotProduct(Bst[k], v1);
        D_k = D_ko - C_ko*C_ko/D_ko;

        //C[k] = C_k;
        //D[k] = D_k;
        //CD[k] = C[k]/D[k];
        //printf ("C[%d]= %Lf		", k, C_k);
        //printf ("D[%d]= %Lf\n", k, D_k);
    }



    //Reducing second half!
    //cout << "aux = " << (1<<10)/D[N0-1] << endl;
    for(j=0; j<N0; j++)
    {    Bst[N0][N0+j] = Bst[N0-1][N0-1-j]*q0/D_k;
         Bst[N0][j] = -Bst[N0-1][2*N0-1-j]*q0/D_k;    }

    //Initialising the vector v = b_N - Proj(b_N, (b_1...b_k-2) )
    for(j=0; j<N0-1; j++)
    {    v[j] = Bst[N0][j+1];
         v[j+N0] = Bst[N0][j+1+N0];    }
    v[N0-1] = -Bst[N0][0];
    v[2*N0-1] = -Bst[N0][N0];

    for(j=0; j<2*N0; j++)
    {    v1[j] = v[j];    }


    //Initialising recursive variables
    C_k = DotProduct(Bst[N0], v1);
    D_k = DotProduct(Bst[N0], Bst[N0]);

    //C[N0] = C_k;
    //D[N0] = D_k;
    //CD[N0] = C[N0]/D[N0];


    //Reducing b_2 to b_N and updating v at the same time
    for(k=N0+1; k<2*N0; k++)
    {
        //b~k <-- r(b~_{k-1}) - <b~_{k-1},b_N>/<v_{k-1},b_N> r(v)
        aux = C_k/D_k;
        Bst[k][0] = -Bst[k-1][N0-1] + aux*v[N0-1];
        Bst[k][N0] = -Bst[k-1][2*N0-1] + aux*v[2*N0-1];
        for(j=1; j<N0; j++)
        {
            Bst[k][j] = Bst[k-1][j-1] - aux*v[j-1];
            Bst[k][j+N0] = Bst[k-1][j+N0-1] - aux*v[j+N0-1];
        }
        //SquareNorm[k] = SquareNorm[k-1] - aux*aux*sqnorm_v;


        //v <-- v - Proj(v, b~_{k-1} )
        for(j=0; j<2*N0; j++)
        {
            v[j] -= aux*Bst[k-1][j];
        }
        //sqnorm_v -= aux*aux*SquareNorm[k-1];

        C_ko = C_k;
        D_ko = D_k;

        C_k = DotProduct(Bst[k], v1);
        D_k = D_ko - C_ko*C_ko/D_ko;

        //C[k] = C_k;
        //D[k] = D_k;
        //CD[k] = C[k]/D[k];
    }
}


//GNTRU

RR_t GNTRU_DotProduct(const RR_t * x1, const RR_t * x2)
{
    unsigned int i;
    RR_t rep = 0;
    for(i = 0; i < 3 * N0; i++)
    {
        rep += x1[i] * x2[i];
    }
    return rep;
}


void GNTRU_Rotate(RR_t * const dest, RR_t const * const src)
{
    unsigned int i;
    for(i = 0; i < N0 - 1; i++)
    {
        dest[i + 1] = src[i];
        dest[N0 + i + 1] = src[N0 + i];
        dest[2 * N0 + i + 1] = src[2 * N0 + i];        
    }
    dest[0] = -src[N0-1];
    dest[N0] = -src[2 * N0-1];
    dest[2 * N0] = -src[3 * N0-1];
}


void GNTRU_ValidPair(ZZ& PGCD, ZZ& Alpha, ZZ& Beta, ZZ& Gamma, ZZX& rho_ff, ZZX& rho_gg, ZZX& rho_hh, const ZZX& ff, const ZZX& gg, const ZZX& hh){
//ff = g1h2 - g2h1, gg = h1f2 - h2f1, hh = f1g2 - f2g1
    ZZX iphi;
    ZZ Res_ff, Res_gg, Res_hh, GCD_GG_HH, Omega; 

    XGCD(Res_ff, rho_ff, iphi, ff, phi, 0);
    XGCD(Res_gg, rho_gg, iphi, gg, phi, 0);
    XGCD(Res_hh, rho_hh, iphi, hh, phi, 0);

    //Check whether ff is invertible modulo q
    if(GCD(Res_ff, q1) != 1)
    {
        PGCD = q1 - 1;
    }
    else
    {
        //Utilize the equation GCD(Res_ff, Res_gg, Res_hh) = GCD(Res_ff, GCD(Res_gg, Reshh))
        XGCD(GCD_GG_HH, Beta, Gamma, Res_gg, Res_hh);
//        cout << GCD_GG_HH << ", " << Beta << ", " << Gamma <<endl;
        XGCD(PGCD, Alpha, Omega, Res_ff, GCD_GG_HH);
//        cout << PGCD << ", " << Alpha << ", " << Omega << endl;
        Beta *= Omega;
        Gamma *= Omega;
//        cout << "(Alpha, Beta, Gamma) = (" << Alpha << ", " << Beta << ", " << Gamma << ")" << endl;
    }

}

// TO CHECK: I have no idea on the role of mb (maxbit)
// For the time being, I will put mb as an input of GNTRU_ReductionCoefficient following ReductionCoefficient
ZZX* GNTRU_ReductionCoefficient(const ZZX& f1, const ZZX& g1, const ZZX& h1, const ZZX& f2, const ZZX& g2, const ZZX& h2, const ZZX& F, const ZZX& G, const ZZX& H){
// <v, w> = sum vi(1/x)* wi(x)
// What we want to compute? v3^star = v3 - k2*v2 - k2*v1, then k1, k2 = ?
// From the simple compuation, we can obtain
// k2 = (<v1,v1> * <v2,v3> - <v2,v1> * <v1,v3>) / (<v1,v1> * <v2,v2> - <v1,v2> * <v2,v1>)
// k1 = (<v2,v2> * <v1,v3> - <v1,v2> * <v2,v3>) / (<v1,v1> * <v2,v2> - <v1,v2> * <v2,v1>)
// Set v1 = (f1, g1, h1), v2 = (f2, g2, h2), and v3 = (F, G, H)
//     den = <v1,v1> * <v2,v2> - <v1,v2> * <v2,v1>
//     num2 = <v1,v1> * <v2,v3> - <v2,v1> * <v1,v3>
//     num1 = <v2,v2> * <v1,v3> - <v1,v2> * <v2,v3>      
//  Te output is rounding{(k1, k2)}   
    unsigned int i;
    ZZ a;
    ZZX f1b, g1b, h1b, f2b, g2b, h2b, num1, num2, den, iden, iphi;
    ZZX* k = new ZZX[2];

    f1b = Reverse(f1);
    g1b = Reverse(g1);
    h1b = Reverse(h1);
    f2b = Reverse(f2);
    g2b = Reverse(g2);
    h2b = Reverse(h2);
    den = FastMod(FastMod(f1b * f1 + g1b * g1 + h1b * h1) * FastMod(f2b * f2 + g2b * g2 + h2b * h2) - FastMod(f1b * f2 + g1b * g2 + h1b * h2) * FastMod(f2b * f1 + g2b * g1 + h2b * h1));
    num2 = FastMod(FastMod(f1b * f1 + g1b * g1 + h1b * h1) * FastMod(f2b * F + g2b * G + h2b * H) - FastMod(f2b * f1 + g2b * g1 + h2b * h1) * FastMod(f1b * F + g1b * G + h1b * H));
    num1 = FastMod(FastMod(f2b * f2 + g2b * g2 + h2b * h2) * FastMod(f1b * F + g1b * G + h1b * H) - FastMod(f1b * f2 + g1b * g2 + h1b * h2) * FastMod(f2b * F + g2b * G + h2b * H));

    XGCD(a, iden, iphi, den, phi);

    k[0] = FastMod(num1 * iden);
    k[1] = FastMod(num2 * iden);

    k[0].SetLength(N0);
    k[1].SetLength(N0);
    for(i = 0; i < N0; i++)
    {
        k[0][i] /= a;
        k[1][i] /= a;
    }

    return k;
}
ZZX* GNTRU_FastReductionCoefficient(const ZZX& f1, const ZZX& g1, const ZZX& h1, const ZZX& f2, const ZZX& g2, const ZZX& h2, const ZZX& F, const ZZX& G, const ZZX& H){
// FFT optimization of GNTRU_ReductionCoefficient
    unsigned int i;
    ZZX* k = new ZZX[2];
    CC_t f1_FFT[N0], g1_FFT[N0], h1_FFT[N0], f2_FFT[N0], g2_FFT[N0], h2_FFT[N0], F_FFT[N0], G_FFT[N0], H_FFT[N0], num1_FFT[N0], num2_FFT[N0], den_FFT[N0], k1_FFT[N0], k2_FFT[N0];

    assert(MaxBits(f1) < 900);
    ZZXToFFT(f1_FFT, f1);

    assert(MaxBits(g1) < 900);
    ZZXToFFT(g1_FFT, g1);

    assert(MaxBits(h1) < 900);
    ZZXToFFT(h1_FFT, h1);

    assert(MaxBits(f2) < 900);
    ZZXToFFT(f2_FFT, f2);

    assert(MaxBits(g2) < 900);
    ZZXToFFT(g2_FFT, g2);   

    assert(MaxBits(h2) < 900);
    ZZXToFFT(h2_FFT, h2);     

    assert(MaxBits(F) < 900);
    ZZXToFFT(F_FFT, F);

    assert(MaxBits(G) < 900);
    ZZXToFFT(G_FFT, G);

    assert(MaxBits(H) < 900);
    ZZXToFFT(H_FFT, H);   

    for(i = 0; i < N0; i++)
    {
        den_FFT[i] = (f1_FFT[N0 - 1 - i] * f1_FFT[i] + g1_FFT[N0 - 1 - i] * g1_FFT[i] +  h1_FFT[N0 - 1 - i] * h1_FFT[i]) * (f2_FFT[N0 - 1 - i] * f2_FFT[i] + g2_FFT[N0 - 1 - i] * g2_FFT[i] +  h2_FFT[N0 - 1 - i] * h2_FFT[i]) - (f1_FFT[N0 - 1 - i] * f2_FFT[i] + g1_FFT[N0 - 1 - i] * g2_FFT[i] +  h1_FFT[N0 - 1 - i] * h2_FFT[i]) * (f2_FFT[N0 - 1 - i] * f1_FFT[i] + g2_FFT[N0 - 1 - i] * g1_FFT[i] +  h2_FFT[N0 - 1 - i] * h1_FFT[i]);
        num2_FFT[i] = (f1_FFT[N0 - 1 - i] * f1_FFT[i] + g1_FFT[N0 - 1 - i] * g1_FFT[i] +  h1_FFT[N0 - 1 - i] * h1_FFT[i]) * (f2_FFT[N0 - 1 - i] * F_FFT[i] + g2_FFT[N0 - 1 - i] * G_FFT[i] +  h2_FFT[N0 - 1 - i] * H_FFT[i]) - (f2_FFT[N0 - 1 - i] * f1_FFT[i] + g2_FFT[N0 - 1 - i] * g1_FFT[i] +  h2_FFT[N0 - 1 - i] * h1_FFT[i]) * (f1_FFT[N0 - 1 - i] * F_FFT[i] + g1_FFT[N0 - 1 - i] * G_FFT[i] +  h1_FFT[N0 - 1 - i] * H_FFT[i]);
        num1_FFT[i] = (f2_FFT[N0 - 1 - i] * f2_FFT[i] + g2_FFT[N0 - 1 - i] * g2_FFT[i] +  h2_FFT[N0 - 1 - i] * h2_FFT[i]) * (f1_FFT[N0 - 1 - i] * F_FFT[i] + g1_FFT[N0 - 1 - i] * G_FFT[i] +  h1_FFT[N0 - 1 - i] * H_FFT[i]) - (f1_FFT[N0 - 1 - i] * f2_FFT[i] + g1_FFT[N0 - 1 - i] * g2_FFT[i] +  h1_FFT[N0 - 1 - i] * h2_FFT[i]) * (f2_FFT[N0 - 1 - i] * F_FFT[i] + g2_FFT[N0 - 1 - i] * G_FFT[i] +  h2_FFT[N0 - 1 - i] * H_FFT[i]); 
        k2_FFT[i] = num2_FFT[i] / den_FFT[i];
        k1_FFT[i] = num1_FFT[i] / den_FFT[i];
    }

    FFTToZZX(k[0], k1_FFT);
    FFTToZZX(k[1], k2_FFT);
    return k;
}
//==============================================================================
//Generates a basis from (f1, g1, h1), (f2, g2, h2), (F, G, H) and N
//This basis has the form :
//    |f1 g1 h1|
//M = |f2 g2 h2|
//    |F  G  H |
//==============================================================================
mat_ZZ GNTRU_BasisFromPolynomials(const ZZX& f1, const ZZX& g1, const ZZX& h1, const ZZX& f2, const ZZX& g2, const ZZX& h2, const ZZX& F, const ZZX& G, const ZZX& H){
    unsigned int i,j;
    mat_ZZ A, M;
    M.SetDims(3 * N0, 3 * N0);
    A = AnticircularMatrix(f1);
    for(i = 0; i < N0; i++){
        for(j = 0; j < N0; j++){
        M[i][j] = A[i][j];
        }
    }
    A = AnticircularMatrix(g1);
    for(i = 0; i < N0; i++){
        for(j = 0; j < N0; j++){
        M[i][j + N0] = A[i][j];
        }
    }
    A = AnticircularMatrix(h1);
    for(i = 0; i < N0; i++){
        for(j = 0; j < N0; j++){
        M[i][j + 2 * N0] = A[i][j];
        }
    }
    A = AnticircularMatrix(f2);
    for(i = 0; i < N0; i++){
        for(j = 0; j < N0; j++){
        M[i + N0][j] = A[i][j];
        }
    }
    A = AnticircularMatrix(g2);
    for(i = 0; i < N0; i++){
        for(j = 0; j < N0; j++){
        M[i + N0][j + N0] = A[i][j];
        }
    }
    A = AnticircularMatrix(h2);
    for(i = 0; i < N0; i++){
        for(j = 0; j < N0; j++){
        M[i + N0][j + 2 * N0] = A[i][j];
        }
    }
    A = AnticircularMatrix(F);
    for(i = 0; i < N0; i++){
        for(j = 0; j < N0; j++){
        M[i + 2 * N0][j] = A[i][j];
        }
    }
    A = AnticircularMatrix(G);
    for(i = 0; i < N0; i++){
        for(j = 0; j < N0; j++){
        M[i + 2 * N0][j + N0] = A[i][j];
        }
    }
    A = AnticircularMatrix(H);
    for(i = 0; i < N0; i++){
        for(j = 0; j < N0; j++){
        M[i + 2 * N0][j + 2 * N0] = A[i][j];
        }
    }

    return M;
}


void GNTRU_GS_Norm(const ZZX f1x, const ZZX g1x, const ZZX h1x, const ZZX f2x, const ZZX g2x, const ZZX h2x, int& flag)
{
// Check whether ||tilde B_{2N+1}|| < min {||tilde B_{N+1}||, ||tilde B_1||}
    unsigned int i;

    double acc1, acc2, acc3, Fred[N0], Gred[N0], Hred[N0];
    CC_t f1[N0], g1[N0], h1[N0], f2[N0], g2[N0], h2[N0], F[N0], G[N0], H[N0], ff[N0], gg[N0], hh[N0];

    acc1 = 0;
    acc2 = 0;
    for(i = 0; i < N0; i++)
    {
        acc1 += conv<double>(f1x[i] * f1x[i] + g1x[i] * g1x[i] + h1x[i] * h1x[i]);
        acc2 += conv<double>(f2x[i] * f2x[i] + g2x[i] * g2x[i] + h2x[i] * h2x[i]);
    }
    acc1 = sqrt(acc1);
    acc2 = sqrt(acc2);

    ZZXToFFT(f1, f1x);
    ZZXToFFT(g1, g1x);
    ZZXToFFT(h1, h1x);
    ZZXToFFT(f2, f2x);
    ZZXToFFT(g2, g2x);
    ZZXToFFT(h2, h2x);

    for(i = 0; i < N0; i++){
        ff[i] = g1[i] * h2[i] - g2[i] * h1[i];
        gg[i] = h1[i] * f2[i] - h2[i] * f1[i];
        hh[i] = f1[i] * g2[i] - f2[i] * g1[i];
    }
    for(i = 0; i < N0; i++){
        F[i] = ff[N0 - 1 - i]/(ff[i] * ff[N0 - 1 - i] + gg[i] * gg[N0 - 1 - i] + hh[i] * hh[N0 - 1 - i]);
        G[i] = gg[N0 - 1 - i]/(ff[i] * ff[N0 - 1 - i] + gg[i] * gg[N0 - 1 - i] + hh[i] * hh[N0 - 1 - i]);
        H[i] = hh[N0 - 1 - i]/(ff[i] * ff[N0 - 1 - i] + gg[i] * gg[N0 - 1 - i] + hh[i] * hh[N0 - 1 - i]);
    }
    MyRealReverseFFT(Fred, F);
    MyRealReverseFFT(Gred, G);
    MyRealReverseFFT(Hred, H);

    acc3 = 0;
    for(i = 0; i < N0; i++)
    {
        acc3 += Fred[i] * Fred[i] + Gred[i] * Gred[i] + Hred[i] * Hred[i];
    }
    acc3 = q0 * sqrt(acc3);
    cout << "acc1 = " << acc1 << endl;
    cout << "acc2 = " << acc2 << endl;    
    cout << "acc3 = " << acc3 << endl;
    if(acc3 < acc1)
    {
        flag = 1;
    }
}

//==============================================================================
//Generates a secret basis (f1,g1,h1),(f2,g2,h2),(F,G,H) from the parameters N,q,Norme
//This bases generates a GNTRU lattice
//==============================================================================
void GNTRU_GenerateBasis(ZZX& f1, ZZX& g1, ZZX& h1, ZZX& f2, ZZX& g2, ZZX& h2, ZZX& F, ZZX& G, ZZX& H,  ZZX& ff, ZZX& gg, ZZX& hh, const ZZ& Norme)
{
// ff = g1h2 - g2h1, gg = h1f2 - h2f1, hh = f1g2 - f2g1    
    ZZX rho_ff, rho_gg, rho_hh, aux, f1b, g1b, h1b, f2b, g2b, h2b, num;
    ZZX* k = new ZZX[2];
    ZZ PGCD = conv<ZZ>(1), Alpha, Beta, Gamma;

    int flag = 0;
    double c = 1.56;            // 1.25^2
    while( (q1 % PGCD != 0) || (flag == 0) )
    {
        flag = 1;
        f1 = RandomPolyFixedSqNorm(Norme, N0 - 1);
        g1 = RandomPolyFixedSqNorm(Norme, N0 - 1);
        h1 = RandomPolyFixedSqNorm(Norme, N0 - 1);
        f2 = RandomPolyFixedSqNormreal(c * conv<double>(Norme), N0 - 1);
        g2 = RandomPolyFixedSqNormreal(c * conv<double>(Norme), N0 - 1);
        h2 = RandomPolyFixedSqNormreal(c * conv<double>(Norme), N0 - 1);

        GNTRU_GS_Norm(f1, g1, h1, f2, g2, h2, flag);

        ff = FastMod(g1 * h2 - g2 * h1);
        gg = FastMod(h1 * f2 - h2 * f1);
        hh = FastMod(f1 * g2 - f2 * g1);
        GNTRU_ValidPair(PGCD, Alpha, Beta, Gamma, rho_ff, rho_gg, rho_hh, ff, gg, hh);
        // cout << "(PGCD, flag) = (" << PGCD << ", " << flag << ")" << endl; 
    }
    // ZZX tmp = FastMod(Alpha * FastMod(rho_ff * ff) + Beta * FastMod(rho_gg * gg)+ Gamma * FastMod(rho_hh * hh));
    // cout << tmp.rep.length() <<", " << tmp[0] << endl;
    // cout << "ValidPair Check Done!" << endl;
    F = q1 / PGCD * Alpha * rho_ff;
    G = q1 / PGCD * Beta * rho_gg;
    H = q1 / PGCD * Gamma * rho_hh;

    // cout << F.rep.length() <<", " <<  G.rep.length() << ", " << H.rep.length() << endl;

//    cout << "F maxbit = " << MaxBits(F) << endl;

    f1.SetLength(N0);
    g1.SetLength(N0);
    h1.SetLength(N0);
    f2.SetLength(N0);
    g2.SetLength(N0);
    h2.SetLength(N0);
    F.SetLength(N0);
    G.SetLength(N0);
    H.SetLength(N0);

    cout << "F,G,H maxbits Before Reduction = " << MaxBits(F) << ", " << MaxBits(G) << ", "<<MaxBits(H) << endl;
    cout << "Start ReductionCoefficient!" << endl;
    clock_t t1 = clock();
    k = GNTRU_ReductionCoefficient(f1, g1, h1, f2, g2, h2, F, G, H);
    // k = GNTRU_FastReductionCoefficient(f1, g1, h1, f2, g2, h2, F, G, H);
    while(deg(k[0]) >= 0 && deg(k[1]) >= 0)
    {
        F = FastMod(F - k[0] * f1 - k[1] * f2);
        G = FastMod(G - k[0] * g1 - k[1] * g2);
        H = FastMod(H - k[0] * h1 - k[1] * h2);

        // k = GNTRU_FastReductionCoefficient(f1, g1, h1, f2, g2, h2, F, G, H);
        k = GNTRU_ReductionCoefficient(f1, g1, h1, f2, g2, h2, F, G, H);
        k[0].normalize();
        k[1].normalize();
        // cout << "(deg k1, deg k2) = ( " << deg(k[0]) << ", " << deg(k[1]) <<" )"<< endl;
    }
    cout << "End ReductionCoefficient!" << endl;
    clock_t t2 = clock();
    float diff = ((float)t2 - (float)t1)/1000000.0F;
    cout << "Reductioncoefficient takes " << diff << " seconds" << endl;
    cout << "F,G,H maxbits After Reduction = " << MaxBits(F) << ", " << MaxBits(G) << ", "<<MaxBits(H) << endl;
    aux = FastMod(ff * F + gg * G + hh * H);

    assert(aux[0] == q1);
    assert(deg(aux) == 0);
    aux.SetLength(N0);
}

// //==============================================================================
// //Generates a secret basis (f1,g1,h1),(f2,g2,h2),(F,G,H) from the parameters N,q,Norme
// //This bases generates a GNTRU lattice
// //==============================================================================
// void GNTRU_GenerateBasis_Norm(ZZX& f1, ZZX& g1, ZZX& h1, ZZX& f2, ZZX& g2, ZZX& h2, ZZX& F, ZZX& G, ZZX& H,  ZZX& ff, ZZX& gg, ZZX& hh, const ZZ& Norme)
// {
// // ff = g1h2 - g2h1, gg = h1f2 - h2f1, hh = f1g2 - f2g1    
//     ZZX rho_ff, aux, f1b, g1b, h1b, f2b, g2b, h2b, num;
//     ZZX* k = new ZZX[2];
//     ZZ PGCD = conv<ZZ>(1), Alpha, Beta, Gamma, Res_ff = 0;

//     int flag = 0;
//     double c = 1.56;            // 1.25^2
//     while(1){
// 	    while(1)
// 	    {
// 	        flag = 0;
// 	        f1 = RandomPolyFixedSqNorm(Norme, N0 - 1);
// 	        g1 = RandomPolyFixedSqNorm(Norme, N0 - 1);
// 	        h1 = RandomPolyFixedSqNorm(Norme, N0 - 1);
// 	        f2 = RandomPolyFixedSqNormreal(c * conv<double>(Norme), N0 - 1);
// 	        g2 = RandomPolyFixedSqNormreal(c * conv<double>(Norme), N0 - 1);
// 	        h2 = RandomPolyFixedSqNormreal(c * conv<double>(Norme), N0 - 1);

// 	        GNTRU_GS_Norm(f1, g1, h1, f2, g2, h2, flag);

// 	        ff = FastMod(g1 * h2 - g2 * h1);
// 	        XGCD(Res_ff, rho_ff, iphi, ff, phi, 0);
// 	        if(Res_ff % q1 == 1)
// 	        	break;
// 	    }
// 	    gg = FastMod(h1 * f2 - h2 * f1);
// 	    hh = FastMod(f1 * g2 - f2 * g1);
// 	    GNTRU_TowerSolver(F, G, H, ff, gg, hh, N0);
// 	}











//     f1.SetLength(N0);
//     g1.SetLength(N0);
//     h1.SetLength(N0);
//     f2.SetLength(N0);
//     g2.SetLength(N0);
//     h2.SetLength(N0);
//     F.SetLength(N0);
//     G.SetLength(N0);
//     H.SetLength(N0);


//     aux = FastMod(ff * F + gg * G + hh * H);

//     assert(aux[0] == q1);
//     assert(deg(aux) == 0);
//     aux.SetLength(N0);
// }


void GNTRU_ClassicMGS(RR_t Bstar[3 * N0][3 * N0], const RR_t B[3 * N0][3 * N0])
{
    RR_t SquareNorm[3 * N0], aux[3 * N0];
    unsigned int i,j,k;

    SquareNorm[0] = GNTRU_DotProduct(B[0], B[0]);
    for(j = 0; j < 3 * N0; j++)
    {
        Bstar[0][j] = B[0][j];
    }

    for(i = 1; i < 3 * N0; i++)
    {
        for(k = 0; k < 3 * N0; k++)
        {
            Bstar[i][k] = B[i][k];
        }
        for(j = 0; j < i; j++)
        {
            aux[j]= GNTRU_DotProduct(Bstar[i], Bstar[j]) / SquareNorm[j];
        }
        for(k = 0; k < 3 * N0; k++)
        {
            for(j = 0; j < i; j++)
            {
                Bstar[i][k] -= aux[j] * Bstar[j][k];
            }
        }
        SquareNorm[i] = GNTRU_DotProduct(Bstar[i], Bstar[i]);
    }
}


void GNTRU_FastMGS(RR_t Bst[3 * N0][3 * N0], const RR_t B[3 * N0][3 * N0], const CC_t PrK_fft[9][N0])
{
//TO CHECK: correctness    
    RR_t v[3 * N0], v1[3 * N0], C_k, D_k, C_ko, D_ko, aux;
    unsigned int j, k;

    cout << endl;
    //Reducing first vector (obvious)
    for(j = 0; j < 3 * N0; j++)
    {    Bst[0][j] = B[0][j];    }

    //Initialising the vector v = b_N - Proj(b_N, (b_1...b_k-2) )
    // v = (1/x) * B[0] = - B[N - 1]
    for(j = 0; j < N0 - 1; j++){    
        v[j] = Bst[0][j + 1];
        v[j + N0] = Bst[0][j + 1 + N0];  
        v[j + 2 * N0] = Bst[0][j + 1 + 2 * N0];  
    }
    v[N0 - 1] = -Bst[0][0];
    v[2 * N0 - 1] = -Bst[0][N0];
    v[3 * N0 - 1] = -Bst[0][2 * N0];

    for(j = 0; j < 3 * N0; j++)
    {    v1[j] = v[j];    }

    //Initialising recurring variables
    C_k = GNTRU_DotProduct(Bst[0], v1);
    D_k = GNTRU_DotProduct(v1 ,v1);

    //Reducing b_2 to b_N and updating v at the same time
    for(k = 1; k < N0; k++)
    {
        //b~k <-- r(b~_{k-1}) - <b~_{k-1},b_N>/<v_{k-1},b_N> r(v)
        //r(f) = x * f
        aux = C_k / D_k;
        Bst[k][0] = -Bst[k - 1][N0 - 1] + aux * v[N0 - 1];
        Bst[k][N0] = -Bst[k - 1][2 * N0 - 1] + aux * v[2 * N0 - 1];
        Bst[k][2 * N0] = -Bst[k - 1][3 * N0 - 1] + aux * v[3 * N0 - 1];
        for(j = 1; j < N0; j++)
        {
            Bst[k][j] = Bst[k - 1][j - 1] - aux * v[j - 1];
            Bst[k][j + N0] = Bst[k - 1][j + N0 - 1] - aux * v[j + N0 - 1];
            Bst[k][j + 2 * N0] = Bst[k - 1][j + 2 * N0 - 1] - aux * v[j + 2 * N0 - 1];
        }
        //v <-- v - Proj(v, b~_{k-1} )
        for(j = 0; j < 3 * N0; j++)
        {
            v[j] -= aux * Bst[k - 1][j];
        }
        //sqnorm_v -= aux*aux*SquareNorm[k-1];

        C_ko = C_k;
        D_ko = D_k;

        C_k = GNTRU_DotProduct(Bst[k], v1);
        D_k = D_ko - C_ko * C_ko / D_ko;
    }



    //Reducing second part!
    CC_t* den_FFT = new CC_t[N0]; CC_t* k1_FFT = new CC_t[N0];
    CC_t* Bst_N0_FFT1 = new CC_t[N0]; CC_t* Bst_N0_FFT2 = new CC_t[N0]; CC_t* Bst_N0_FFT3 = new CC_t[N0];
    double* Bst_N0_1 = new double[N0]; double* Bst_N0_2 = new double[N0]; double* Bst_N0_3 = new double[N0];
    //k1 = <(f1,g1,h1), (f2,g2,h2)> / <(f1,g1,h1), (f1,g1,h1)>
    for(j = 0; j < N0; j++){
        den_FFT[j] = PrK_fft[0][j] * PrK_fft[0][N0 - 1 - j] + PrK_fft[1][j] * PrK_fft[1][N0 - 1 - j] + PrK_fft[2][j] * PrK_fft[2][N0 - 1 - j];
        k1_FFT[j] = PrK_fft[3][j] * PrK_fft[0][N0 - 1 - j] + PrK_fft[4][j] * PrK_fft[1][N0 - 1 - j] + PrK_fft[5][j] * PrK_fft[2][N0 - 1 - j];
        k1_FFT[j] /= den_FFT[j];
    }

    for(j = 0; j < N0; j++){
        //TO DO: Define Bst[N0] = tilde B_N 
        Bst_N0_FFT1[j] = PrK_fft[3][j] - k1_FFT[j] * PrK_fft[0][j];
        Bst_N0_FFT2[j] = PrK_fft[4][j] - k1_FFT[j] * PrK_fft[1][j];
        Bst_N0_FFT3[j] = PrK_fft[5][j] - k1_FFT[j] * PrK_fft[2][j];
        }
    
    MyRealReverseFFT(Bst_N0_1, Bst_N0_FFT1);
    MyRealReverseFFT(Bst_N0_2, Bst_N0_FFT2);
    MyRealReverseFFT(Bst_N0_3, Bst_N0_FFT3);

    for(j = 0; j < N0; j++){
        Bst[N0][j] = Bst_N0_1[j];
        Bst[N0][j + N0] = Bst_N0_2[j];
        Bst[N0][j + 2 * N0] = Bst_N0_3[j];        
    }

    //Initialising the vector v = b_N - Proj(b_N, (b_1...b_k-2) )
    for(j = 0; j < N0 - 1; j++){
        v[j] = Bst[N0][j + 1];
        v[j + N0] = Bst[N0][j + 1 + N0];    
        v[j + 2 * N0] = Bst[N0][j + 1 + 2 * N0];  
    }
    v[N0 - 1] = -Bst[N0][0];
    v[2 * N0 - 1] = -Bst[N0][N0];
    v[3 * N0 - 1] = -Bst[N0][2 * N0];

    for(j = 0; j < 3 * N0; j++)
    {    v1[j] = v[j];    }


    //Initialising recursive variables
    C_k = GNTRU_DotProduct(Bst[N0], v1);
    D_k = GNTRU_DotProduct(Bst[N0], Bst[N0]);


    //Reducing b_2 to b_N and updating v at the same time
    for(k = N0 + 1; k < 2 * N0; k++)
    {
        //b~k <-- r(b~_{k-1}) - <b~_{k-1},b_N>/<v_{k-1},b_N> r(v)
        aux = C_k / D_k;
        Bst[k][0] = -Bst[k - 1][N0 - 1] + aux * v[N0 - 1];
        Bst[k][N0] = -Bst[k - 1][2 * N0 - 1] + aux * v[2 * N0 - 1];
        Bst[k][2 * N0] = -Bst[k - 1][3 * N0 - 1] + aux * v[3 * N0 - 1];
        for(j = 1; j < N0; j++)
        {
            Bst[k][j] = Bst[k - 1][j - 1] - aux * v[j - 1];
            Bst[k][j + N0] = Bst[k - 1][j + N0 - 1] - aux * v[j + N0 - 1];
            Bst[k][j + 2 * N0] = Bst[k - 1][j + 2 * N0 - 1] - aux * v[j + 2 * N0 - 1];
        }

        //v <-- v - Proj(v, b~_{k-1} )
        for(j = 0; j < 3 * N0; j++)
        {
            v[j] -= aux * Bst[k - 1][j];
        }

        C_ko = C_k;
        D_ko = D_k;

        C_k = GNTRU_DotProduct(Bst[k], v1);
        D_k = D_ko - C_ko * C_ko / D_ko;

    }


    //TO DO: Reducing third part!
    CC_t* ff_FFT = new CC_t[N0]; CC_t* gg_FFT = new CC_t[N0]; CC_t* hh_FFT = new CC_t[N0];
    CC_t* Bst_2N0_FFT1 = new CC_t[N0]; CC_t* Bst_2N0_FFT2 = new CC_t[N0]; CC_t* Bst_2N0_FFT3 = new CC_t[N0];
    double* Bst_2N0_1 = new double[N0]; double* Bst_2N0_2 = new double[N0]; double* Bst_2N0_3 = new double[N0];

    for(j = 0; j < N0; j++){
        ff_FFT[j] = PrK_fft[1][j] * PrK_fft[5][j] - PrK_fft[4][j] * PrK_fft[2][j];
        gg_FFT[j] = PrK_fft[2][j] * PrK_fft[3][j] - PrK_fft[5][j] * PrK_fft[0][j];
        hh_FFT[j] = PrK_fft[0][j] * PrK_fft[4][j] - PrK_fft[3][j] * PrK_fft[1][j];
    }

    for(j = 0; j < N0; j++){
        den_FFT[j] = ff_FFT[j] * ff_FFT[N0 - 1 - j] + gg_FFT[j] * gg_FFT[N0 - 1 - j] + hh_FFT[j] * hh_FFT[N0 - 1 - j];
    }

    for(j = 0; j < N0; j++){
        //TO DO: Define Bst[2 * N0] = tilde B_2N  
        Bst_2N0_FFT1[j] = ff_FFT[ N0 - 1 - j] / den_FFT[j];
        Bst_2N0_FFT2[j] = gg_FFT[ N0 - 1 - j] / den_FFT[j];
        Bst_2N0_FFT3[j] = hh_FFT[ N0 - 1 - j] / den_FFT[j];        
    }
    MyRealReverseFFT(Bst_2N0_1, Bst_2N0_FFT1);
    MyRealReverseFFT(Bst_2N0_2, Bst_2N0_FFT2);
    MyRealReverseFFT(Bst_2N0_3, Bst_2N0_FFT3);

    for(j = 0; j < N0; j++){
        Bst[2 * N0][j] = q2 * Bst_2N0_1[j];
        Bst[2 * N0][j + N0] = q2 * Bst_2N0_2[j];
        Bst[2 * N0][j + 2 * N0] = q2 * Bst_2N0_3[j];
    }

    //Initialising the vector v = b_N - Proj(b_N, (b_1...b_k-2) )
    for(j = 0; j < N0 - 1; j++){
        v[j] = Bst[2 * N0][j + 1];
        v[j + N0] = Bst[2 * N0][j + 1 + N0];    
        v[j + 2 * N0] = Bst[2 * N0][j + 1 + 2 * N0];  
    }
    v[N0 - 1] = -Bst[2 * N0][0];
    v[2 * N0 - 1] = -Bst[2 * N0][N0];
    v[3 * N0 - 1] = -Bst[2 * N0][2 * N0];

    for(j = 0; j < 3 * N0; j++)
    {    v1[j] = v[j];    }


    //Initialising recursive variables
    C_k = GNTRU_DotProduct(Bst[2 * N0], v1);
    D_k = GNTRU_DotProduct(Bst[2 * N0], Bst[2 * N0]);


    //Reducing b_2 to b_N and updating v at the same time
    for(k = 2 * N0 + 1; k < 3 * N0; k++)
    {
        //b~k <-- r(b~_{k-1}) - <b~_{k-1},b_N>/<v_{k-1},b_N> r(v)
        aux = C_k / D_k;
        Bst[k][0] = -Bst[k - 1][N0 - 1] + aux * v[N0 - 1];
        Bst[k][N0] = -Bst[k - 1][2 * N0 - 1] + aux * v[2 * N0 - 1];
        Bst[k][2 * N0] = -Bst[k - 1][3 * N0 - 1] + aux * v[3 * N0 - 1];
        for(j = 1; j < N0; j++)
        {
            Bst[k][j] = Bst[k - 1][j - 1] - aux * v[j - 1];
            Bst[k][j + N0] = Bst[k - 1][j + N0 - 1] - aux * v[j + N0 - 1];
            Bst[k][j + 2 * N0] = Bst[k - 1][j + 2 * N0 - 1] - aux * v[j + 2 * N0 - 1];
        }

        //v <-- v - Proj(v, b~_{k-1} )
        for(j = 0; j < 3 * N0; j++)
        {
            v[j] -= aux * Bst[k - 1][j];
        }

        C_ko = C_k;
        D_ko = D_k;

        C_k = GNTRU_DotProduct(Bst[k], v1);
        D_k = D_ko - C_ko * C_ko / D_ko;
    }
}

// void GNTRU_TowerSolver(ZZX& F, ZZX& G, ZZX& H, const ZZX& ff, const ZZX& gg, const ZZX& hh, unsigned int& N, int& flag){


// }
