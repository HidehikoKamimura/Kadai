#include "pch.h"
#include "framework.h"

#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "KseMath/KseMatrix.h"

template <typename T>
KseMatrix<T>::KseMatrix()
: m_pM(nullptr)
, m_nRow(0UL)
, m_nColumn(0UL)
{
}

template <typename T>
KseMatrix<T>::KseMatrix(const unsigned long inRow, const unsigned long inColumn)
: m_pM(nullptr)
, m_nRow(inRow)
, m_nColumn(inColumn)
{
	assert(m_nRow > 0L && m_nColumn > 0L);

	m_pM = new T[m_nRow * m_nColumn];
	memset(m_pM, 0x00, sizeof(T) * m_nRow * m_nColumn);
}

template <typename T>
KseMatrix<T>::KseMatrix(const KseVector<T>* ipVector, unsigned long inCount)
: m_pM(nullptr)
, m_nRow(0UL)
, m_nColumn(0UL)
{
	unsigned long nColumn = ipVector[0].m_nDim;

	for (unsigned long i = 1L; i < inCount; i++) {
		if (ipVector[i].m_nDim != nColumn) return;
	}
	
	if (nColumn == 0L || inCount == 0L) return;

	m_nRow = inCount;
	m_nColumn = nColumn;

	m_pM = new T[m_nRow * m_nColumn];

	for (unsigned long i = 0L; i < m_nRow; i++) {
		for (unsigned long j = 0L; j < m_nColumn; j++) {
			m_pM[i* m_nColumn + j] = ipVector[i].m_pCoord[j];
		}
	}
}

template <typename T>
KseMatrix<T>::KseMatrix(const KseMatrix<T>& irMatrix)
{
	*this = irMatrix;
}

template <typename T>
KseMatrix<T>::~KseMatrix()
{
	if (m_pM != nullptr) delete[] m_pM;
}

template <typename T>
KseMatrix<T> KseMatrix<T>::operator*(const KseMatrix<T>& irMatrix) const
{
	assert(m_nColumn == irMatrix.m_nRow);

	KseMatrix<T> mtx(m_nRow, irMatrix.m_nColumn);

	for (unsigned long i = 0L; i < m_nRow; i++ ) {
		for (unsigned long j = 0L; j < irMatrix.m_nColumn; j++) {
			T m = 0.0f;
			for (unsigned long k = 0L; k < m_nColumn; k++) {
				m += m_pM[i * m_nColumn + k] * irMatrix.m_pM[k* irMatrix.m_nColumn + j];
			}
			mtx.m_pM[i* irMatrix.m_nColumn + j] = m;
		}
	}
	return KseMatrix<T>(mtx);
}

template <typename T>
KseVector<T> KseMatrix<T>::operator*(const KseVector<T>& irVector) const
{
	assert(m_nColumn == irVector.m_nDim);

	KseVector<T> vec(nullptr, m_nRow);

	for (unsigned long i = 0L; i < m_nRow; i++) {
		T m = 0.0f;
		for (unsigned long j = 0L; j < m_nColumn; j++) {
			m += m_pM[i * m_nColumn + j] * irVector.m_pCoord[j];
			if (isfinite(m) == false) {
				printf("fatal error, %s(%d)", __FILE__, __LINE__);
				exit(-9);
			}
		}
		vec.m_pCoord[i] = m;
	}
	return KseVector<T>(vec);
}

template <typename T>
void KseMatrix<T>::Transpose()
{
	if (m_pM == nullptr || m_nRow == 0UL || m_nColumn == 0UL) return;

	T* pM = new T[m_nRow * m_nColumn];
	for (unsigned long i = 0UL; i < m_nRow; i++) {
		for (unsigned long j = 0UL; j < m_nColumn; j++) {
			pM[j * m_nRow + i] = m_pM[i * m_nColumn + j];
		}
	}

	unsigned long nColumn = m_nColumn;
	m_nColumn = m_nRow;
	m_nRow = nColumn;

	delete[] m_pM;
	m_pM = pM;
}

template <typename T>
KseMatrix<T>& KseMatrix<T>::operator=(const KseMatrix<T>& irMatrix)
{
	if (m_pM != nullptr) delete[] m_pM;

	m_nRow = irMatrix.m_nRow;
	m_nColumn = irMatrix.m_nColumn;
	m_pM = new T[m_nRow * m_nColumn];

	for (unsigned long i = 0L; i < m_nRow; i++) {
		for (unsigned long j = 0L; j < m_nColumn; j++) {
			m_pM[i* m_nColumn + j] = irMatrix.m_pM[i* m_nColumn + j];
		}
	}
	return *this;
}

template <typename T>
void KseMatrix<T>::Replace(const KseVector<T>* ipVector, unsigned long inColumnIndex)
{
	assert(m_nRow == ipVector->m_nDim && inColumnIndex < m_nColumn);

	for (unsigned long i = 0L; i < m_nRow; i++) {
		m_pM[i * m_nColumn + inColumnIndex] = ipVector->m_pCoord[i];
	}
}

template <typename T>
T KseMatrix<T>::Det3x3() const
{
	return
		m_pM[0 * 3 + 0] * m_pM[1 * 3 + 1] * m_pM[2 * 3 + 2] +
		m_pM[0 * 3 + 1] * m_pM[1 * 3 + 2] * m_pM[2 * 3 + 0] +
		m_pM[0 * 3 + 2] * m_pM[1 * 3 + 0] * m_pM[2 * 3 + 1] -
		m_pM[0 * 3 + 2] * m_pM[1 * 3 + 1] * m_pM[2 * 3 + 0] -
		m_pM[0 * 3 + 1] * m_pM[1 * 3 + 0] * m_pM[2 * 3 + 2] -
		m_pM[0 * 3 + 0] * m_pM[1 * 3 + 2] * m_pM[2 * 3 + 1];
}

template <typename T>
T KseMatrix<T>::Det2x2() const
{
	return m_pM[0 * 2 + 0] * m_pM[1 * 2 + 1] - m_pM[0 * 2 + 1] * m_pM[1 * 2 + 0];
}

template <typename T>
T KseMatrix<T>::Det1x1() const
{
	return m_pM[0];
}

template <typename T>
T KseMatrix<T>::Det() const
{
	assert(m_nRow > 0L && m_nRow == m_nColumn);

	T det = 0.0f;

	if ( m_nRow == 3 ) {
		det = Det3x3();
	}
	else if ( m_nRow == 2 ) {
		det = Det2x2();
	}
	else if ( m_nRow == 1 ) {
		det = Det1x1();
	}
	else {
		for (unsigned long i = 0L; i < m_nColumn; i++) {
			det += m_pM[0 * m_nColumn + i] * Cofactor(0, i);
		}
	}

	return det;
}

template <typename T>
void KseMatrix<T>::Resize(unsigned long inRow, unsigned long inColumn, bool ibKeepValue)
{
	assert(inRow > 0L && inColumn > 0L);

	T* _pM = new T[inRow * inColumn];
	memset(_pM, 0x00, sizeof(T) * inRow * inColumn);

	if (ibKeepValue == true) {
		unsigned long nRow = (inRow < m_nRow) ? inRow : m_nRow;
		unsigned long nColumn = (inColumn < m_nColumn) ? inColumn : m_nColumn;

		for (unsigned long i = 0L; i < nRow; i++) {
			for (unsigned long j = 0L; j < nColumn; j++) {
				_pM[i * nColumn + j] = m_pM[i * nColumn + j];
			}
		}
	}

	if (m_pM != nullptr) delete[] m_pM;

	m_nRow = inRow;
	m_nColumn = inColumn;
	m_pM = _pM;
}

template <typename T>
T KseMatrix<T>::Cofactor(unsigned long iRowIndex, unsigned long iColumnIndex) const
{
	KseMatrix<T> mtx;
	GetRCRemovedMatrix(iRowIndex, iColumnIndex, mtx);

	unsigned long exp = (iRowIndex + 1) + (iColumnIndex + 1);
	bool bOdd = ((exp & 0x01) == 1) ? true : false;

	T r = mtx.Det();
	if (bOdd == true) r *= -1;

	return r;
}

template <typename T>
void KseMatrix<T>::GetRCRemovedMatrix(unsigned long iRowIndex, unsigned long iColumnIndex, KseMatrix<T>& orMatrix) const
{
	assert(iRowIndex < m_nRow && iColumnIndex < m_nColumn);

	orMatrix.Resize(m_nRow - 1, m_nColumn - 1, false);

	unsigned long r = 0L;
	for (unsigned long i = 0L; i < m_nRow; i++) {
		if (i == iRowIndex) continue;
		r++;

		unsigned long c = 0L;
		for (unsigned long j = 0L; j < m_nColumn; j++) {
			if (j == iColumnIndex) continue;
			c++;
			orMatrix.m_pM[(r - 1) * (m_nColumn - 1) + ( c - 1 )] = m_pM[i * m_nColumn + j];
		}
	}
}

template <typename T>
void KseMatrix<T>::ExtractColumn(unsigned long iColumnIndex, KseVector<T>& orVector) const
{
	T* pColumn = new T[m_nRow];

	for (unsigned long i = 0UL; i < m_nRow; i++) pColumn[i] = m_pM[i * m_nColumn + iColumnIndex];

	orVector.Set(pColumn, m_nRow);

	delete[] pColumn;
}

template <typename T>
void KseMatrix<T>::Print(FILE* ipFile, const char* ipHeader) const
{
	fprintf(ipFile, "%s\n", ipHeader);

	for (unsigned long i = 0UL; i < m_nRow; i++) {
		for (unsigned long j = 0UL; j < m_nColumn; j++) {
			fprintf(ipFile, "\t%10.3f", m_pM[i * m_nColumn + j]);
		}
		fprintf(ipFile, "\n");
	}
	fprintf(ipFile, "\n");
}