#include <iostream>
#include <time.h>
#include <direct.h>
#include "KseBase/KseBase.h"
#include "KseMath/KseStatistics.h"
#include "KseMath/KseMatrix.h"

#define T double

void Hist(FILE* ipFile, const T* ipValue, const long inSize, const unsigned int inRange, const char* ipHeader);
void HistTest();
void Histgram(KseRandomDist<T>* ipDist, unsigned long inSampling, T iPitch, const T iCenter, const T iHalfWidth);

int main(int argc, char* argv[])
{
	if (argc < 2) return -1;

	time_t cur = time(nullptr);
	tm now_tm;
	errno_t err = localtime_s(&now_tm, &cur);
	if (err != 0) return -1;

	char pDirPath[KseMaxFPathLen];
	sprintf_s(pDirPath, KseMaxFPathLen, "%s\\%d%02d_%02d_%02d%02d%02d",
		argv[1],
		now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday,
		now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec
	);

	if( _mkdir(pDirPath) != 0 ) return -1;

	char pLogFilePath[KseMaxFPathLen];
	sprintf_s(pLogFilePath, KseMaxFPathLen, "%s\\KseSampleApp002.log", pDirPath);

	FILE* pLogFile = nullptr;
	err = fopen_s(&pLogFile, pLogFilePath, "w");
	if (err != 0) return -1;

#if _DEBUG
	fprintf(pLogFile, "Debug Build\n");
#else
	fprintf(pLogFile, "Release Build\n");
#endif

	time_t start_tm = time(nullptr);

	/*
	 * X = (x_1 ... x_p),  n : sampling size
	 */
	const unsigned long nSample = 1000;
	const unsigned long p = 3;

	clock_t t0 = clock();

	KseVector<T>* pX = new KseVector<T>[nSample];
	for(unsigned long i = 0UL; i < nSample; i++ ) pX[i].Set(nullptr, p);
	
	KseUniformDist<T> u_dist(0, 1);
	u_dist.Sample(pX, nSample);

	clock_t t1 = clock();

	fprintf(pLogFile, "Sampling CPU time(s) : %lf\n", (double)(t1 - t0) / CLOCKS_PER_SEC);

	/*
	 * - beta = (1 2 3 4)' (p + 1 = 4, 1 : constant term)
	 * - epsilon ~ N(0, 1)
	 */
	T pBeta[] = {1, 2, 3, 4};
	KseVector<T> beta(pBeta, 4);
	KseNormalDist<T> n_dist(0, 1);
	
	KseLinearRegression<T> lnreg;
	lnreg.SetSample(pX, nSample);

	lnreg.Print_XtX(pLogFile);

	unsigned long nLoop = 100000;
	T* ppResultDelta[4];
	for (unsigned int i = 0U; i < 4; i++) {
		ppResultDelta[i] = new T[nLoop];
		memset(ppResultDelta[i], 0x00, sizeof(T) * nLoop);
	}
	unsigned long loop_1per = nLoop / 100;

	t0 = clock();

	unsigned long curLoop = 0UL;
	while (curLoop < nLoop) {
		KseVector<T> yvec;
		lnreg.SimulateY(&n_dist, beta, yvec);

		KseVector<T> beta_hat(nullptr, 4);
		if (lnreg.EstimateBeta(yvec, beta_hat) == false) {
			printf("fatal error !");
			exit(-9);
		}

		KseVector<T> beta_delta = beta_hat;
		beta_delta -= beta;

		ppResultDelta[0][curLoop] = beta_delta.m_pCoord[0];
		ppResultDelta[1][curLoop] = beta_delta.m_pCoord[1];
		ppResultDelta[2][curLoop] = beta_delta.m_pCoord[2];
		ppResultDelta[3][curLoop] = beta_delta.m_pCoord[3];
		
		curLoop++;

		if ( curLoop % loop_1per == 0 ) {
			printf("%d%% done...\r", curLoop * 100 / nLoop);
		}
	}
	printf("100%% done...\n");

	t1 = clock();

	fprintf(pLogFile, "Simulation CPU time(s) : %lf\n", (double)(t1 - t0) / CLOCKS_PER_SEC);

	fprintf(pLogFile, "X : %ld x %ld\n", nSample, p + 1);
	fprintf(pLogFile, "Simulation Count = %ld\n", nLoop);

	unsigned long nStack = 1000;
	Hist(pLogFile, ppResultDelta[0], nLoop, nStack, "beta[0]");
	Hist(pLogFile, ppResultDelta[1], nLoop, nStack, "beta[1]");
	Hist(pLogFile, ppResultDelta[2], nLoop, nStack, "beta[2]");
	Hist(pLogFile, ppResultDelta[3], nLoop, nStack, "beta[3]");

	for (unsigned int i = 0U; i < 4; i++) delete[] ppResultDelta[i];

	time_t end_tm = time(nullptr);

	fprintf(pLogFile, "Elaps : %ld sec (%f min)\n", end_tm - start_tm, (double)(end_tm - start_tm)/60);

	fclose(pLogFile);

	return 0;
}

void Hist(FILE* ipFile, const T* ipValue, const long inSize, const unsigned int inRange, const char* ipHeader)
{
	T min = ipValue[0];
	T max = ipValue[0];
	for (unsigned long i = 1UL; i < inSize; i++) {
		if (isfinite(ipValue[i]) == false) {
			printf("fatal error, %s(%d)", __FILE__, __LINE__);
			exit(-9);
		}
		min = ( min > ipValue[i] ) ? ipValue[i] : min;
		max = ( max < ipValue[i] ) ? ipValue[i] : max;
	}

	T rng = max - min;
	T stp = rng / inRange;

	unsigned long* pHist = new unsigned long[inRange];
	memset(pHist, 0x00, sizeof(unsigned long) * inRange);

	for (unsigned long i = 0UL; i < inSize; i++) {
		T ofs = ipValue[i] - min;
		unsigned long idx = (unsigned long)(ofs / stp);
		if (idx >= inRange) {
			idx = inRange - 1;
		}
		else if (idx < 0) {
			idx = 0UL;
		}
		pHist[idx]++;
	}

	fprintf(ipFile, "%s\n", ipHeader);
	for (unsigned long i = 0UL; i < inRange; i++) {
		fprintf(ipFile, "%.3f : %ld\n", min + i * stp, pHist[i]);
	}
	fprintf(ipFile, "\n");

	delete[] pHist;
}

/*
 * prototype...
 */
void HistTest()
{
	KseNormalDist<T> n(0, 1);
	Histgram(&n, 100000, 0.1, n.E(), 4.0 * sqrt(n.V()));

	KseUniformDist<T> u(0, 1);
	Histgram(&u, 100000, 0.1, u.E(), 0.5);
}

void Histgram(KseRandomDist<T>* ipDist, unsigned long inSampling, T iPitch, const T iCenter, const T iHalfWidth)
{
	T* pSample = new T[inSampling];
	ipDist->Sample(pSample, inSampling);

	size_t nHist = iHalfWidth * 2 / iPitch;
	unsigned long pExcept[2] = { 0, 0 };

	unsigned long* pHist = new unsigned long[nHist];
	memset(pHist, 0x00, sizeof(unsigned long) * nHist);

	T min = iCenter - iHalfWidth;

	for (unsigned long i = 0UL; i < inSampling; i++) {
		T ofs = pSample[i] - min;
		int idx = (int)(ofs / iPitch);

		if (idx < 0) {
			pExcept[0]++;
		}
		else if (idx > nHist) {
			pExcept[1]++;
		}
		else if (idx == nHist) {
			pHist[nHist-1]++;
		}
		else
			pHist[idx]++;
	}
	printf("%.1f : %ld\n", min - iPitch, pExcept[0]);
	for (unsigned long i = 0UL; i < nHist; i++) printf("%.1f : %ld\n", min + i * iPitch, pHist[i]);
	printf("%.1f : %ld\n", iCenter + iHalfWidth, pExcept[1]);

	delete[] pHist;
	delete[] pSample;
}