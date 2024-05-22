#pragma once

#include <stdio.h>
#include "KseMath/KseVector.h"

template <typename T>
class KseRandomDist {
public:
	virtual T E() const { return m_E; };

	virtual T V() const { return m_V; };

	virtual void Sample(KseVector<T>* opSample, unsigned long inSample = 1UL) const = 0;

	virtual void Sample(T* opSample, unsigned long inSample = 1UL) const = 0;

protected:
	KseRandomDist();

	virtual ~KseRandomDist();

protected:
	T m_E;
	T m_V;
};

template <typename T>
class KseUniformDist : public KseRandomDist<T> {
public:
	KseUniformDist(const T iA, const T iB);

	~KseUniformDist();

	void Sample(KseVector<T>* opSample, unsigned long inSample = 1UL) const;

	void Sample(T* opSample, unsigned long inSample = 1UL) const;

private:
	T m_a;
	T m_b;
};

template <typename T>
class KseNormalDist : public KseRandomDist<T> {
public:
	KseNormalDist(const T iE, const T iV);

	~KseNormalDist();

	void Sample(KseVector<T>* opSample, unsigned long inSample = 1UL) const;

	void Sample(T* opSample, unsigned long inSample = 1UL) const;

private:
	void BoxMuller(T* opRand1, T* opRand2 = nullptr) const;
};

template <typename T>
class KseLinearRegression {
public:
	KseLinearRegression();

	~KseLinearRegression();

	void SetSample(KseVector<T>* ipX, unsigned long inX);

	void Print_XtX(FILE* ipFile) const;

	bool EstimateBeta(KseVector<T>& irY, KseVector<T>& orBetaEst) const;

	bool SimulateY(KseRandomDist<T>* ipDist, KseVector<T>& irLBeta, KseVector<T>& orY) const;

private:
	KseVector<T>* m_pX;
	unsigned long m_nX;

	//KseVector<T> m_BetaEst;
	//KseVector<T> m_Beta;
};

template KseLinearRegression<double>;

template KseRandomDist<double>;
template KseUniformDist<double>;
template KseNormalDist<double>;