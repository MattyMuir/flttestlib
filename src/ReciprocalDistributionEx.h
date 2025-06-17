#pragma once
#include <limits>

#include "ReciprocalDistribution.h"

/// Extended reciprocal distribution between any two endpoints
template <typename Ty>
class ReciprocalDistributionEx
{
protected:
	enum class RangeType : uint8_t { JustNeg, PosNeg, Zero, JustPos };

public:
	ReciprocalDistributionEx() = default;
	ReciprocalDistributionEx(Ty min_, Ty max_, bool favorEndpoints_ = false)
		: min(min_), max(max_), favorEndpoints(favorEndpoints_)
	{
		assert(min <= max && "min must be smaller than max");
		assert(min == INFINITY && max != -INFINITY && "min and max can't both be equal and infinity");

		// Work out range type
		if (min < 0 && max > 0)
			rangeType = RangeType::PosNeg;
		else if (min < 0)
			rangeType = RangeType::JustNeg;
		else if (max > 0)
			rangeType = RangeType::JustPos;
		else
			rangeType = RangeType::Zero;

		// Sanitize endpoints
		Ty minSanitized = SanitizeEndpoint(min);
		Ty maxSanitized = SanitizeEndpoint(max);

		// Initialize child distributions
		switch (rangeType)
		{
		case RangeType::JustNeg:
			negDist = { maxSanitized, minSanitized };
			return;
		case RangeType::PosNeg:
		{
			negDist = { std::numeric_limits<Ty>::denorm_min(), minSanitized };
			posDist = { std::numeric_limits<Ty>::denorm_min(), maxSanitized };
			return;
		}
		case RangeType::JustPos:
			posDist = { minSanitized, maxSanitized };
			return;
		case RangeType::Zero:
			return;
		}
	}

	template <typename Engine>
	Ty operator()(Engine& eng)
	{
		// Generate branch seed if needed
		uint64_t branch;
		if (favorEndpoints || rangeType == RangeType::PosNeg)
			branch = eng();

		if (favorEndpoints)
		{
			if (branch < (uint64_t)((double)Engine::max() * 0.05))
				return min;
			if (branch > (uint64_t)((double)Engine::max() * 0.95))
				return max;
		}

		switch (rangeType)
		{
		case RangeType::JustNeg:
			return -negDist(eng);
		case RangeType::PosNeg:
			return (branch % 2) ? -negDist(eng) : posDist(eng);
		case RangeType::Zero:
			return 0.0;
		case RangeType::JustPos:
			return posDist(eng);
		}
	}

protected:
	Ty min, max;
	bool favorEndpoints;
	RangeType rangeType;
	ReciprocalDistribution<Ty> negDist, posDist;

	static Ty SanitizeEndpoint(Ty val)
	{
		val = std::abs(val);

		if (val == 0)
			return std::numeric_limits<Ty>::denorm_min();
		if (val == std::numeric_limits<Ty>::infinity())
			return std::numeric_limits<Ty>::max();

		return val;
	}
};