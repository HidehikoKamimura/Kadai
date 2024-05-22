#include "pch.h"
#include "framework.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include "KseMath/KseStatistics.h"
#include "KseMath/KseVector.h"
#include "KseMath/KseMatrix.h"

template <typename T>
KseRandomDist<T>::KseRandomDist()
: m_E(0.0f)
, m_V(0.0f)
{
    static bool bInit = false;
    if (bInit == false) srand(time(nullptr));
}

template <typename T>
KseRandomDist<T>::~KseRandomDist()
{

}


template <typename T>
KseUniformDist<T>::KseUniformDist(const T iA, const T iB)
: KseRandomDist<T>()
, m_a(iA)
, m_b(iB)
{
    this->m_E = (m_a + m_b) / 2;
    this->m_V = (m_b - m_a) * (m_b - m_a) / 12;
}

template <typename T>
KseUniformDist<T>::~KseUniformDist()
{
}

template <typename T>
void KseUniformDist<T>::Sample(KseVector<T>* opSample, unsigned long inSample) const
{
    for (unsigned long i = 0UL; i < inSample; i++) {
        Sample(opSample[i].m_pCoord, opSample[i].m_nDim);
    }
}

template <typename T>
void KseUniformDist<T>::Sample(T* opSample, unsigned long inSample) const
{
    for (unsigned long i = 0UL; i < inSample; i++) {
        T r = (T)(rand()) / RAND_MAX;// r ~ U(0, 1)
        r *= (m_b - m_a);
        opSample[i] = m_a + r;
    }
}


template <typename T>
KseNormalDist<T>::KseNormalDist(const T iE, const T iV)
: KseRandomDist<T>()
{
    this->m_E = iE;
    this->m_V = iV;
}

template <typename T>
KseNormalDist<T>::~KseNormalDist()
{
}

template <typename T>
void KseNormalDist<T>::Sample(KseVector<T>* opSample, unsigned long inSample) const
{
    for(unsigned long i = 0UL; i < inSample; i++) Sample(opSample->m_pCoord, opSample->m_nDim);
}

template <typename T>
void KseNormalDist<T>::Sample(T* opSample, unsigned long inSample) const
{
    for (unsigned long i = 0UL; i < inSample; i++) {
        BoxMuller(opSample + i);
        if (isfinite(opSample[i]) == false) {
            printf("fatal error, %s(%d)...\n", __FILE__, __LINE__);
            exit(-9);
        }
    }
}

template <typename T>
void KseNormalDist<T>::BoxMuller(T* opRand1, T* opRand2) const
{
    // r1, r2 ~ N(0, 1)
    T u1 = (T)(rand()) / RAND_MAX;
    T u2 = (T)(rand()) / RAND_MAX;

    T r1 = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
    T r2 = sqrt(-2 * log(u1)) * sin(2 * M_PI * u2);

    if (isfinite(r1) == false || isfinite(r2) == false ) {
        BoxMuller(opRand1, opRand2);
    }
    else {
        *opRand1 = sqrt(this->m_V) * r1 + this->m_E;
        if (opRand2 != nullptr) *opRand2 = sqrt(this->m_V) * r2 + this->m_E;
    }
}
 
template <typename T>
KseLinearRegression<T>::KseLinearRegression()
: m_pX(nullptr)
, m_nX(0UL)
{
}

template <typename T>
KseLinearRegression<T>::~KseLinearRegression()
{
    if (m_pX != nullptr) delete[] m_pX;
}

template <typename T>
void KseLinearRegression<T>::SetSample(KseVector<T>* ipX, unsigned long inX)
{
    if (ipX == nullptr || inX == 0UL) return;

    m_pX = new KseVector<T>[m_nX = inX];
    for (unsigned long i = 0UL; i < m_nX; i++) m_pX[i] = ipX[i];
}

template <typename T>
void KseLinearRegression<T>::Print_XtX(FILE* ipFile) const
{
    if (m_pX == nullptr || m_nX == 0UL) return;

    for (unsigned long i = 1UL; i < m_nX; i++)
        if (m_pX[i].m_nDim != m_pX[0].m_nDim) return;

    unsigned long nBeta = m_pX[0].m_nDim + 1; // increment for constant term.
    KseVector<T>* pX = new KseVector<T>[m_nX];

    T* pCoord = new T[nBeta];
    for (unsigned long i = 0UL; i < m_nX; i++) {
        pCoord[0] = 1.0f;
        memcpy_s(pCoord + 1, sizeof(T) * m_pX[i].m_nDim, m_pX[i].m_pCoord, sizeof(T) * m_pX[i].m_nDim);
        pX[i].Set(pCoord, nBeta);
    }
    delete[] pCoord;

    KseMatrix<T> x(pX, m_nX);
    delete[] pX;
    //x.Print("X = \n");

    KseMatrix<T> x_t = x;
    x_t.Transpose();

    KseMatrix<T> x_t_x = x_t * x;
    x_t_x.Print(ipFile, "XtX = \n");
}

template <typename T>
bool KseLinearRegression<T>::EstimateBeta(KseVector<T>& irY, KseVector<T>& orBetaEst) const
{
    if (m_pX == nullptr || m_nX == 0UL) return false;

    for (unsigned long i = 1UL; i < m_nX; i++)
        if (m_pX[i].m_nDim != m_pX[0].m_nDim) return false;

    unsigned long nBeta = m_pX[0].m_nDim + 1; // increment for constant term.
    KseVector<T>* pX = new KseVector<T>[m_nX];

    T* pCoord = new T[nBeta];
    for (unsigned long i = 0UL; i < m_nX; i++) {
        pCoord[0] = 1.0f;
        memcpy_s(pCoord + 1, sizeof(T) * m_pX[i].m_nDim, m_pX[i].m_pCoord, sizeof(T) * m_pX[i].m_nDim);
        pX[i].Set(pCoord, nBeta);
    }
    delete[] pCoord;

    KseMatrix<T> x(pX, m_nX);
    delete[] pX;
    //x.Print("X = \n");

    KseMatrix<T> x_t = x;
    x_t.Transpose();

    KseMatrix<T> x_t_x = x_t * x;
    //x_t_x.Print("Xt * X = \n");

    T d = x_t_x.Det();
    if (d == 0.0f) return false;

    KseVector<T> x_t_y = x_t * irY;

    T* pBeta = new T[nBeta];
    for (unsigned long i = 0UL; i < nBeta; i++) {
        KseMatrix<T> _mtx = x_t_x;
        _mtx.Replace(&x_t_y, i);

        T _d = _mtx.Det();
        pBeta[i] = _d / d; // cramer's formula
    }

    orBetaEst.Set(pBeta, nBeta);
    delete[] pBeta;

    return true;
}

template <typename T>
bool KseLinearRegression<T>::SimulateY(KseRandomDist<T>* ipDist, KseVector<T>& irLBeta, KseVector<T>& orY) const
{
    KseVector<T>* pX = new KseVector<T>[m_nX];

    T* pCoord = new T[m_pX[0].m_nDim + 1];
    for (unsigned long i = 0UL; i < m_nX; i++) {
        pCoord[0] = 1.0f;
        memcpy_s(pCoord + 1, sizeof(T) * m_pX[i].m_nDim, m_pX[i].m_pCoord, sizeof(T) * m_pX[i].m_nDim);

        for (unsigned long j = 0UL; j < m_pX[i].m_nDim + 1; j++) {
            if (isfinite(pCoord[j]) == false) {
                printf("fatal error, %s(%d)\n", __FILE__, __LINE__);
                exit(-9);
            }
        }


        pX[i].Set(pCoord, m_pX[i].m_nDim + 1);
    }
    delete[] pCoord;

    KseMatrix<T> mtx_X(pX, m_nX);
    orY = mtx_X* irLBeta;

    for (unsigned long i = 0UL; i < orY.m_nDim; i++) {
        if (isfinite(orY.m_pCoord[i]) == false) {
            printf("fatal error, %s(%d)\n", __FILE__, __LINE__);
            exit(-9);
        }
    }

    KseVector<T> eps(nullptr, orY.m_nDim);
    ipDist->Sample(&eps);

    for (unsigned long i = 0UL; i < eps.m_nDim; i++) {
        if (isfinite(eps.m_pCoord[i]) == false) {
            printf("fatal error, %s(%d)\n", __FILE__, __LINE__);
            exit(-9);
        }
    }

    orY += eps;

    delete[] pX;

    return true;
}