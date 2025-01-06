#pragma once
#include <limits>

#include <immintrin.h>

#ifndef FTL_NO_MPFR
#include <mpfr.h>

using MpfrFunction1D = int(*)(mpfr_ptr, mpfr_srcptr, mpfr_rnd_t);
using MpfrFunction2D = int(*)(mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t);

template <typename Ty, MpfrFunction1D Func>
std::pair<Ty, Ty> MakeBounded(Ty x)
{
	// Initialize xMp
	mpfr_t xMp;
	mpfr_init2(xMp, std::numeric_limits<Ty>::digits);
	mpfr_set_d(xMp, x, MPFR_RNDN);

	// Evaluate function
	mpfr_t yMp;
	mpfr_init2(yMp, std::numeric_limits<Ty>::digits);
	int ternary = Func(yMp, xMp, MPFR_RNDD);
	double y = mpfr_get_d(yMp, MPFR_RNDD);

	// Free memory
	mpfr_clear(xMp);
	mpfr_clear(yMp);

	if (ternary == 0)
		return { y, y };
	if (ternary < 0)
		return { y, std::nextafter(y, INFINITY) };

	throw;
}

template <typename Ty, MpfrFunction2D Func>
std::pair<Ty, Ty> MakeBounded(Ty x, Ty y)
{
	// Initialize xMp, yMp
	mpfr_t xMp, yMp;
	mpfr_init2(xMp, std::numeric_limits<Ty>::digits);
	mpfr_set_d(xMp, x, MPFR_RNDN);
	mpfr_init2(yMp, std::numeric_limits<Ty>::digits);
	mpfr_set_d(yMp, y, MPFR_RNDN);

	// Evaluate function
	mpfr_t zMp;
	mpfr_init2(zMp, std::numeric_limits<Ty>::digits);
	int ternary = Func(zMp, xMp, yMp, MPFR_RNDD);
	double z = mpfr_get_d(zMp, MPFR_RNDD);

	// Free memory
	mpfr_clear(xMp);
	mpfr_clear(yMp);
	mpfr_clear(zMp);

	if (ternary == 0)
		return { z, z };
	if (ternary < 0)
		return { z, std::nextafter(z, INFINITY) };

	throw;
}
#endif

template <typename Ty>
using FuncType1D = Ty(*)(Ty);

template <typename Ty, FuncType1D<Ty> Func>
Ty Overload(Ty x)
{
	return Func(x);
}

template <typename Ty>
using SimdType = std::conditional_t<std::is_same_v<Ty, float>, __m256, __m256d>;
template <typename Ty>
using SimdFunc = SimdType<Ty>(*)(SimdType<Ty>);

template <typename Ty, SimdFunc<Ty> Func>
Ty MakeSerial(Ty x)
{
	// Broadcast input to pack
	SimdType<Ty> p;
	if constexpr (std::is_same_v<Ty, float>)
		p = _mm256_set1_ps(x);
	else
		p = _mm256_set1_pd(x);

	// Evaluate function
	SimdType<Ty> res = Func(p);

	// Extract result from pack
	if constexpr (std::is_same_v<Ty, float>)
		return std::bit_cast<Ty>(_mm256_extract_epi32(_mm256_castps_si256(res), 0));
	else
		return std::bit_cast<Ty>(_mm256_extract_epi64(_mm256_castpd_si256(res), 0));
}