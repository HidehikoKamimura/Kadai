#pragma once

template <typename T>
class KseCoord {
public:
	KseCoord();

	KseCoord(const T* ipCoord, const unsigned long inDim);

	KseCoord(const KseCoord <T>& irCoord);

	~KseCoord();

	void Set(const T* ipCoord, const unsigned long inDim);

	T Average() const;

	KseCoord<T>& operator+=(const KseCoord <T>& irCoord);

	KseCoord<T>& operator-=(const KseCoord <T>& irCoord);

	KseCoord<T>& operator*=(const T iFactor);

	KseCoord<T>& operator/=(const T iFactor);

	T operator*=(const KseCoord <T>& irCoord);

	KseCoord<T>& operator%=(const KseCoord <T> & irCoord);

	KseCoord<T>& operator=(const KseCoord<T>& irCoord);

	unsigned long m_nDim;
	T* m_pCoord;
};

template KseCoord<float>;
template KseCoord<double>;