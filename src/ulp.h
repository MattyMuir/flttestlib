#pragma once
#include <type_traits>
#include <bit>
#include <limits>

template <typename Ty>
using IntType = std::conditional_t<sizeof(Ty) == 4, uint32_t, uint64_t>;

template <typename Ty>
using UIntType = std::conditional_t<sizeof(Ty) == 4, uint32_t, uint64_t>;

/// Returns the ULP distance between two floating point values
template <typename Ty>
UIntType<Ty> ULPDistance(Ty x, Ty y)
{
	using Int = UIntType<Ty>;

	if (x > y) std::swap(x, y);

	Int xPunn = std::bit_cast<Int>(x);
	Int yPunn = std::bit_cast<Int>(y);

	bool xSign = xPunn >> (sizeof(Ty) * 8 - 1);
	bool ySign = yPunn >> (sizeof(Ty) * 8 - 1);

	if (!xSign && !ySign)
		return yPunn - xPunn;
	if (xSign && ySign)
		return xPunn - yPunn;

	constexpr Int MinPos = std::bit_cast<Int>(std::numeric_limits<Ty>::denorm_min());
	constexpr Int MinNeg = std::bit_cast<Int>(-std::numeric_limits<Ty>::denorm_min());
	constexpr Int Delta = MinPos + MinNeg - 2;

	return (xPunn - Delta) + yPunn;
}

/// Returns the ULP error between an approximate value and an interval containing the exact answer
template <typename Ty, typename ITy>
UIntType<Ty> ULPDistance(Ty approx, ITy exact)
{
	auto [inf, sup] = exact;
	return std::max(ULPDistance(approx, inf), ULPDistance(approx, sup));
}

/// Returns the signed ULP distance between two floating point values
template <typename Ty>
IntType<Ty> ULPDistanceSigned(Ty x, Ty y)
{
	using Int = IntType<Ty>;

	if (x > y)
		return -(Int)ULPDistance(y, x);
	return (Int)ULPDistance(x, y);
}