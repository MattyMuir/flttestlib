#include <iostream>
#include <charconv>
#include <format>
#include <random>

#include <flttestlib.h>

#define ERROR(msg) { std::cout << msg << '\n'; return 1; }

template <typename Ty>
int ULPDistanceTest()
{
	// === Parameters ====
	size_t NumIter = 30'000'000;
	int64_t MaxDistance = 10;
	// ===================

	static std::mt19937_64 gen{ std::random_device{}() };
	ReciprocalDistributionEx<Ty> dist{ -INFINITY, INFINITY, false };
	std::uniform_int_distribution<int64_t> distanceDist{ -MaxDistance, MaxDistance };

	for (size_t i = 0; i < NumIter; i++)
	{
		Ty x = dist(gen);
		Ty xn = x;

		int64_t distance = distanceDist(gen);
		for (int64_t d = 0; d < std::abs(distance); d++)
			xn = std::nextafter(xn, (distance > 0) ? INFINITY : -INFINITY);

		if (std::cmp_not_equal(ULPDistance(x, xn), std::abs(distance)))
			ERROR(std::format("x: {}   xn: {}\n", x, xn));
	}

	// Unit tests
	if (ULPDistance((Ty)0, std::numeric_limits<Ty>::denorm_min()) != 1)
		ERROR("Unit test 0 failed");
	if (ULPDistance(-std::numeric_limits<Ty>::denorm_min(), (Ty)0) != 1)
		ERROR("Unit test 1 failed")
	if (ULPDistance(std::numeric_limits<Ty>::max(), std::numeric_limits<Ty>::infinity()) != 1)
		ERROR("Unit test 2 failed")
	if (ULPDistance(std::numeric_limits<Ty>::lowest(), -std::numeric_limits<Ty>::infinity()) != 1)
		ERROR("Unit test 3 failed")

	return 0;
}

template <typename Ty, template <typename> class DistTy>
int DistributionTest()
{
	// === Parameters ===
	size_t NumDist = 10'000;
	size_t NumIter = 10'000;
	// ==================

	constexpr bool IsExtended = std::is_same_v<DistTy<Ty>, ReciprocalDistributionEx<Ty>>;

	static std::mt19937_64 gen{ std::random_device{}() };
	constexpr Ty MinEndpoint = IsExtended ? -INFINITY : std::numeric_limits<Ty>::denorm_min();
	constexpr Ty MaxEndpoint = IsExtended ? INFINITY : std::numeric_limits<Ty>::max();
	static ReciprocalDistributionEx<Ty> endpointDist{ MinEndpoint, MaxEndpoint, true };
	for (size_t d = 0; d < NumDist; d++)
	{
		// Generate random endpoints
		Ty min = endpointDist(gen);
		Ty max = endpointDist(gen);
		if (min > max) std::swap(min, max);

		if (min == INFINITY || max == -INFINITY) continue;

		// Generate distribution
		auto distCreator = [min, max]() {
			if constexpr (IsExtended)
				return DistTy<Ty>{ min, max, false };
			else
				return DistTy<Ty>{ min, max };
		};
		DistTy<Ty> dist = distCreator();

		for (size_t i = 0; i < NumIter; i++)
		{
			Ty sample = dist(gen);

			bool valid = (sample >= min) && (sample <= max);
			if (!valid)
				ERROR(std::format("min: {} max: {} sample: {}\n", min, max, sample));
		}
	}

	return 0;
}

int main(int argc, char** argv)
{
	// Check number of arguments is correct
	if (argc != 2)
		ERROR("Test must have exactly one extra argument");

	// Get second argument
	std::string arg{ argv[1] };

	// Convert to integer
	size_t testIdx;
	auto convRes = std::from_chars(arg.data(), arg.data() + arg.size(), testIdx);
	if (convRes.ec != std::errc())
		ERROR("Test index could not be parsed");

	switch (testIdx)
	{
	case 0: return ULPDistanceTest<float>();
	case 1: return ULPDistanceTest<double>();
	case 2: return DistributionTest<float, ReciprocalDistribution>();
	case 3: return DistributionTest<double, ReciprocalDistribution>();
	case 4: return DistributionTest<float, ReciprocalDistributionEx>();
	case 5: return DistributionTest<double, ReciprocalDistributionEx>();
	default: ERROR("Invalid test index");
	}
}