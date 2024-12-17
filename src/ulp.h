#pragma once
#include <iostream>
#include <format>
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
	static_assert(std::is_same_v<Ty, decltype(inf)>);
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

/// Runs many random tests and returns the maximum error found
auto MaxULPRounded(auto referenceFunc, auto approxFunc, auto randFunc, size_t iter)
{
	using Ty = std::invoke_result_t<decltype(randFunc)>;
	using Int = UIntType<Ty>;

	Int maxError = 0;
	for (uint64_t i = 0; (i < iter) || !iter; i++)
	{
		// Sample random input
		Ty x = randFunc();

		// Evaluate approx and exact
		Ty approx = approxFunc(x);
		auto [inf, sup] = referenceFunc(x);

		// Check for bad NaN
		if (!std::isfinite(approx) && std::isfinite(inf) && std::isfinite(sup))
		{
			std::cout << std::format("Bad NaN: {}\n", x);
			return (Int)0;
		}

		// Update maxError
		uint32_t error = ULPDistance(approx, std::pair<Ty, Ty>{ inf, sup });
		if (error > maxError)
		{
			maxError = error;
			if (!iter) std::cout << std::format("error: {} x: {}\n", maxError, x);
		}
	}

	return maxError;
}