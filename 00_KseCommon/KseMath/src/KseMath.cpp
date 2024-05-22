#include "pch.h"
#include "framework.h"

#include "KseMath/KseMath.h"

template <typename T>static T ___KseSysTol = 1e-3;

template <typename T>void KseMath_SetTol(const T iTol)
{
	___KseSysTol = iTol;
}

template <typename T>T KseMath_GetTol()
{
	return ___KseSysTol;
}