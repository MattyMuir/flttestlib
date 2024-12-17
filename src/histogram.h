#pragma once
#include <iostream>
#include <format>
#include <random>

auto IdentityMap(auto x) { return x; }

template <typename Ty>
using MapTy = Ty(*)(Ty);

template <typename Ty>
void ULPHistogram(auto referenceFunc, auto approxFunc, Ty min, Ty max, Ty step, MapTy<Ty> map = IdentityMap, size_t iter = 10'000)
{
	static std::mt19937_64 gen{ std::random_device{}() };

	for (Ty low = min; low < max; low += step)
	{
		Ty high = low + step;
		std::uniform_real_distribution<Ty> dist{ low, high };

		uint64_t err = MaxULPRounded(referenceFunc, approxFunc, [&]() { return map(dist(gen)); }, iter);
		std::cout << std::format("{:.5f},{:.5f},{}\n", low, high, err);
	}
}