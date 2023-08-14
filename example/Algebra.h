#ifndef LIBE_ALGEBRA_H
#define LIBE_ALGEBRA_H

#include "params.h"

ZZX Cyclo();
ZZX FastMod(const ZZX& f);
ZZ SquaredNorm(const ZZX& f, const unsigned int degree);
ZZX Reverse(const ZZX& f);
mat_ZZ AnticircularMatrix(const ZZX& f);
ZZ_pX Inverse(const ZZX& f);
ZZ_pX Quotient(const ZZX& f, const ZZX& g);
ZZX OneStepNorm(const ZZX& f, const unsigned int N);


//NTRU
RR_t DotProduct(const RR_t * x1, const RR_t * x2);
void Rotate(RR_t * const dest, RR_t const * const src);
void ValidPair(ZZ& PGCD, ZZ& Alpha, ZZ& Beta, ZZX& rho_f, ZZX& rho_g, const ZZX& f, const ZZX& g);
ZZX ReductionCoefficient(const ZZX& f, const ZZX& g, const ZZX& F, const ZZX& G, unsigned int & mb);
ZZX FastReductionCoefficient(const ZZX& f, const ZZX& g, const ZZX& F, const ZZX& G);
mat_ZZ BasisFromPolynomials(const ZZX& f, const ZZX& g, const ZZX& F, const ZZX& G);
void GS_Norm(const ZZX fx, const ZZX gx, int& flag);
void GenerateBasis(ZZX& f, ZZX& g, ZZX& F, ZZX& G, const ZZ& Norme);
void ClassicMGS(RR_t Bstar[2 * N0][2 * N0], const RR_t B[2 * N0][2 * N0]);
void FastMGS(RR_t Bst[2 * N0][2 * N0], const RR_t B[2 * N0][2 * N0]);

//GNTRU
RR_t GNTRU_DotProduct(const RR_t * x1, const RR_t * x2);
void GNTRU_Rotate(RR_t * const dest, RR_t const * const src);
void GNTRU_ValidPair(ZZ& PGCD, ZZ& Alpha, ZZ& Beta, ZZ& Gamma, ZZX& rho_ff, ZZX& rho_gg, ZZX& rho_hh, const ZZX& ff, const ZZX& gg, const ZZX& hh);
ZZX* GNTRU_ReductionCoefficient(const ZZX& f1, const ZZX& g1, const ZZX& h1, const ZZX& f2, const ZZX& g2, const ZZX& h2, const ZZX& F, const ZZX& G, const ZZX& H);
ZZX* GNTRU_FastReductionCoefficient(const ZZX& f1, const ZZX& g1, const ZZX& h1, const ZZX& f2, const ZZX& g2, const ZZX& h2, const ZZX& F, const ZZX& G, const ZZX& H);
mat_ZZ GNTRU_BasisFromPolynomials(const ZZX& f1, const ZZX& g1, const ZZX& h1, const ZZX& f2, const ZZX& g2, const ZZX& h2, const ZZX& F, const ZZX& G, const ZZX& H);
void GNTRU_GS_Norm(const ZZX f1, const ZZX g1, const ZZX h1, const ZZX f2, const ZZX g2, const ZZX h2, int& flag);
void GNTRU_GenerateBasis(ZZX& f1, ZZX& g1, ZZX& h1, ZZX& f2, ZZX& g2, ZZX& h2, ZZX& F, ZZX& G, ZZX& H, ZZX& ff, ZZX& gg, ZZX& hh, const ZZ& Norme);
// void GNTRU_GenerateBasis_Norm(ZZX& f1, ZZX& g1, ZZX& h1, ZZX& f2, ZZX& g2, ZZX& h2, ZZX& F, ZZX& G, ZZX& H,  ZZX& ff, ZZX& gg, ZZX& hh, const ZZ& Norme);
void GNTRU_ClassicMGS(RR_t Bstar[3 * N0][3 * N0], const RR_t B[3 * N0][3 * N0]);
void GNTRU_FastMGS(RR_t Bst[3 * N0][3 * N0], const RR_t B[3 * N0][3 * N0], const CC_t PrK_fft[9][N0]);
// void GNTRU_TowerSolver(ZZX& F, ZZX& G, ZZX& H, const ZZX& ff, const ZZX& gg, const ZZX& hh, unsigned int& N, int& flag);


#endif
