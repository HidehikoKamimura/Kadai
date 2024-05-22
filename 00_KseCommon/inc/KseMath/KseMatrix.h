#pragma once

#include "KseMath/KseVector.h"

template <typename T>
class KseMatrix {
public:

	KseMatrix();

	KseMatrix(const unsigned long inRow, const unsigned long inColumn);

	KseMatrix(const KseVector<T>* ipVector, unsigned long inCount);

	KseMatrix(const KseMatrix<T>& irMatrix);

	~KseMatrix();

	void Transpose();

	KseMatrix<T> operator*(const KseMatrix<T>& irMatrix) const;

	KseVector<T> operator*(const KseVector<T>& irVector) const;

	KseMatrix<T>& operator=(const KseMatrix<T>& irMatrix);

	void Resize(unsigned long inRow, unsigned long inColumn, bool ibKeepValue);
	
	void GetRCRemovedMatrix(unsigned long iRowIndex, unsigned long iColumnIndex, KseMatrix<T>& orMatrix) const;

	T Cofactor(unsigned long iRowIndex, unsigned long iColumnIndex) const;

	T Det() const;

	void Replace(const KseVector<T>* ipVector, unsigned long inColumnIndex);

	void ExtractColumn(unsigned long iColumnIndex, KseVector<T>& orVector) const;

	unsigned long m_nRow;
	unsigned long m_nColumn;

	T* m_pM;

	void Print(FILE* ipFile, const char* ipHeader) const;

private:
	T Det3x3() const;
	T Det2x2() const;
	T Det1x1() const;
};

template KseMatrix<float>;
template KseMatrix<double>;
