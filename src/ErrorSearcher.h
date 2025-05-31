#pragma once
#include <iostream>
#include <format>

#include "BS_thread_pool.hpp"

#include "ulp.h"

template <typename Ty, typename ReferenceFun, typename ApproxFun>
class ErrorSearcher
{
protected:
	using UInt = UIntType<Ty>;

public:
	ErrorSearcher(ReferenceFun reference_, ApproxFun approx_, size_t numThreads_ = 0)
		: reference(reference_), approx(approx_), numThreads(GetNumThreads(numThreads_)), pool(numThreads) {}

	template <typename RandFun>
	UInt MaxError(RandFun rand, size_t iter = 0)
	{
		return iter ? MaxErrorFixed(rand, iter) : MaxErrorContinuous(rand);
	}

protected:
	ReferenceFun reference;
	ApproxFun approx;
	size_t numThreads;
	BS::thread_pool<BS::tp::none> pool;

	size_t GetNumThreads(size_t target)
	{
		return target ? target : std::thread::hardware_concurrency() - 1;
	}

	template <typename RandFun>
	UInt MaxErrorFixed(RandFun rand, size_t iter)
	{
		size_t iterPerThread = iter / pool.get_thread_count();
		std::vector<std::future<std::pair<UInt, Ty>>> futs;
		for (size_t i = 0; i < numThreads; i++)
			futs.push_back(pool.submit_task([this, rand, iterPerThread]() { return this->FixedWorker(rand, iterPerThread); }));

		UInt maxError = 0;
		Ty criticalArg{};

		for (auto& fut : futs)
		{
			auto [err, x] = fut.get();
			if (err > maxError)
			{
				maxError = err;
				criticalArg = x;
			}
		}

		std::cout << std::format("error: {} x: {}\n", maxError, criticalArg);
		return maxError;
	}

	template <typename RandFun>
	UInt MaxErrorContinuous(RandFun rand)
	{
		UInt maxError = 0;
		bool shouldStop = false;
		for (size_t i = 0; i < numThreads; i++)
			pool.detach_task([this, rand, &maxError, &shouldStop]() { this->ContinuousWorker(rand, maxError, shouldStop); });

		std::thread stopThread{ [this, &shouldStop]() { this->Stopper(shouldStop); } };
		stopThread.join();
		pool.wait();

		return maxError;
	}

	template <typename RandFun>
	std::pair<UInt, Ty> FixedWorker(RandFun rand, size_t iter)
	{
		UInt maxError = 0;
		Ty criticalArg{};
		for (size_t i = 0; i < iter; i++)
		{
			// Sample random input
			Ty x = rand();

			// Evaluate approx and exact
			Ty a = approx(x);
			auto e = reference(x);

			// Update maxError
			uint32_t error = ULPDistance(a, e);
			if (error > maxError)
			{
				maxError = error;
				criticalArg = x;
			}
		}

		return { maxError, criticalArg };
	}

	template <typename RandFun>
	void ContinuousWorker(RandFun rand, UInt& maxError, bool& shouldStop)
	{
		while (!shouldStop)
		{
			// Sample random input
			Ty x = rand();

			// Evaluate approx and exact
			Ty a = approx(x);
			auto e = reference(x);

			// Update maxError
			uint32_t error = ULPDistance(a, e);
			if (error > maxError)
			{
				maxError = error;
				std::cout << std::format("error: {} x: {}\n", error, x);
			}
		}
	}

	void Stopper(bool& shouldStop)
	{
		std::cin.get();
		shouldStop = true;
	}
};

// Deduction guides
template <typename ReferenceFun, typename ApproxFun>
ErrorSearcher(ReferenceFun r, ApproxFun a) -> ErrorSearcher<decltype(a({})), ReferenceFun, ApproxFun>;