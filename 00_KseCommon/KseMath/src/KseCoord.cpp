#include "pch.h"
#include "framework.h"

#include <assert.h>
#include <memory.h>

#include "KseMath/KseMath.h"
#include "KseMath/KseCoord.h"
#include "KseMath/KseMath.h"

template <typename T>
KseCoord<T>::KseCoord()
: m_pCoord(nullptr)
, m_nDim(0UL)
{
}

template <typename T>
KseCoord<T>::KseCoord(const T* ipCoord, const unsigned long inDim)
: m_pCoord(nullptr)
, m_nDim(0UL)
{
	Set(ipCoord, inDim);
}

template <typename T>
KseCoord<T>::KseCoord(const KseCoord <T>& irCoord)
: m_pCoord(nullptr)
, m_nDim(0UL)
{
	*this = irCoord;
}

template <typename T>
KseCoord<T>::~KseCoord()
{
	if (m_pCoord != nullptr) delete[] m_pCoord;
}

template <typename T>
void KseCoord<T>::Set(const T* ipCoord, const unsigned long inDim)
{
	if (inDim > 0) {
		m_nDim = inDim;
		m_pCoord = new T[m_nDim];
		if (ipCoord != nullptr)
			memcpy_s(m_pCoord, sizeof(T) * inDim, ipCoord, sizeof(T) * m_nDim);
		else
			memset(m_pCoord, 0x00, sizeof(T) * inDim);
	}
}

template <typename T>
T KseCoord<T>::Average() const
{
	T avg = 0.0f;
	if (m_nDim > 0UL) {
		for (unsigned long i = 0UL; i < m_nDim; i++) avg += m_pCoord[i];
		avg /= m_nDim;
	}
	return avg;
}

template <typename T>
KseCoord<T>& KseCoord<T>::operator+=(const KseCoord <T>& irCoord)
{
	assert(m_nDim == irCoord.m_nDim);

	for (unsigned long i = 0L; i < m_nDim; i++) m_pCoord[i] += irCoord.m_pCoord[i];
	return *this;
}

template <typename T>
KseCoord<T>& KseCoord<T>::operator-=(const KseCoord <T>& irCoord)
{
	assert(m_nDim == irCoord.m_nDim);

	for (unsigned long i = 0L; i < m_nDim; i++) m_pCoord[i] -= irCoord.m_pCoord[i];
	return *this;
}

template <typename T>
KseCoord<T>& KseCoord<T>::operator/=(const T iFactor)
{
	// assert(iFactor > Kse_SysTol);

	for (unsigned long i = 0L; i < m_nDim; i++) m_pCoord[i] /= iFactor;
	return *this;
}

template <typename T>
KseCoord<T>& KseCoord<T>::operator*=(const T iFactor)
{
	for (unsigned long i = 0L; i < m_nDim; i++) m_pCoord[i] *= iFactor;
	return *this;
}

template <typename T>
T KseCoord<T>::operator*=(const KseCoord <T>& irCoord)
{
	assert(m_nDim == irCoord.m_nDim);

	T dp = 0;
	for (unsigned long i = 0L; i < m_nDim; i++) dp += m_pCoord[i] * irCoord.m_pCoord[i];
	return dp;
}

template <typename T>
KseCoord<T>& KseCoord<T>::operator%=(const KseCoord < T>& irCoord)
{
	assert(m_nDim == 3L && irCoord.m_nDim == 3L);

	KseCoord<T> p;
	return p;
}

template <typename T>
KseCoord<T>& KseCoord<T>::operator=(const KseCoord<T>& irCoord)
{
	if (m_pCoord != nullptr) {
		delete[] m_pCoord;
	}

	m_pCoord = new T[m_nDim = irCoord.m_nDim];

	for (unsigned long i = 0L; i < m_nDim; i++) m_pCoord[i] = irCoord.m_pCoord[i];
	return *this;
}
