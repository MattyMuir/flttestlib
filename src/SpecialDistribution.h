#pragma once
#include <vector>
#include <random>

/// Extended reciprocal distribution that occasionally picks values from a list of 'special' values
template <typename Ty, template <typename> class DistTy = ReciprocalDistributionEx>
class SpecialDistribution
{
public:
	SpecialDistribution(Ty min, Ty max, const std::vector<Ty>& special_, bool favourEndpoints)
		: dist(min, max, favourEndpoints)
	{
		for (Ty s : special_)
			if (s >= min && s <= max)
				special.push_back(s);
	}

	template <typename Engine>
	Ty operator()(Engine& eng)
	{
		uint64_t branch = eng();

		// Randomly return special value
		if (!special.empty() && branch < (uint64_t)((Ty)Engine::max() * 0.1))
			return special[branch % special.size()];

		return dist(eng);
	}

protected:
	std::vector<Ty> special;
	ReciprocalDistributionEx<Ty> dist;
};