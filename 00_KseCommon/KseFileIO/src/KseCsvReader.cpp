#include "pch.h"
#include "framework.h"

#include <stdio.h>
#include <string.h>

#include "KseFileIO/KseCsvReader.h"

#include "KseBase/KseBase.h"
#include "KseMath/KseVector.h"

template <typename T>
KseCsvReader<T>::KseCsvReader(const char* ipFilePath)
: m_pFile(nullptr)
{
	errno_t err = fopen_s(&m_pFile, ipFilePath, "r");
}

template <typename T>
KseCsvReader<T>::~KseCsvReader()
{
	if (m_pFile != nullptr) fclose(m_pFile);
}

template <typename T>
unsigned long KseCsvReader<T>::Read_LinReg(const char* ipDelimiter, const int inYIndex, KseVector<T>*& oprX, KseVector<T>& orY)
{
	if (m_pFile == nullptr) return 0UL;

	rewind(m_pFile);

	unsigned long nLine = 0UL;
	unsigned int nField = 0;

	char pBuffer[KseMaxFPathLen];
	while (fgets(pBuffer, KseMaxFPathLen, m_pFile) != nullptr) {
		if(pBuffer[strlen(pBuffer) - 1] == '\n') pBuffer[strlen(pBuffer) - 1] = '\0';
	
		unsigned int _nField = 0;
		char* pCxt = nullptr;
		char* ptr = strtok_s(pBuffer, ipDelimiter, &pCxt);
		while (ptr != nullptr) {
			_nField++;
			ptr = strtok_s(nullptr, ipDelimiter, &pCxt);
		}

		if (_nField == 0) return 0UL;

		if (nField == 0)
			nField = _nField;
		else {
			if (nField != _nField) return 0UL;
		}

		nLine++;
	}

	rewind(m_pFile);

	if (!(inYIndex < nField)) return 0UL;

	oprX = new KseVector<T>[nLine];

	T* pX = new T[nField - 1];
	T* pY = new T[nLine];

	nLine = 0UL;
	while (fgets(pBuffer, KseMaxFPathLen, m_pFile) != nullptr) {
		if (pBuffer[strlen(pBuffer) - 1] == '\n') pBuffer[strlen(pBuffer) - 1] = '\0';

		unsigned int _nXField = 0;
		char* pCxt = nullptr;
		char* ptr = strtok_s(pBuffer, ipDelimiter, &pCxt);
		while (ptr != nullptr) {
			if (_nXField == inYIndex)
				pY[nLine] = (T)atof(ptr);
			else {
				pX[_nXField++] = (T)atof(ptr);
			}

			ptr = strtok_s(nullptr, ipDelimiter, &pCxt);
		}

		oprX[nLine].Set(pX, _nXField);
		nLine++;
	}
	
	delete[] pX;

	orY.Set(pY, nLine);

	delete[] pY;

	return nLine;
}