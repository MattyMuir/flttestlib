#pragma once
#include <limits>

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