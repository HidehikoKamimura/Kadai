#pragma once

#include <stdio.h>
#include "KseMath/KseVector.h"

template <typename T>
class KseCsvReader {
public:
	KseCsvReader(const char* ipFilePath);

	~KseCsvReader();

	unsigned long Read_LinReg(const char* ipDelimiter, const int inYIndex, KseVector<T>*& oprX, KseVector<T>& orY);

private:
	FILE* m_pFile;
};

template KseCsvReader<double>;