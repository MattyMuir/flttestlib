#include <iostream>
#include <format>

#include <flttestlib.h>

double Approx(double x)
{
	return x;
}

double Rand()
{
	thread_local std::mt19937_64 gen{ std::random_device{}() };
	ReciprocalDistributionEx<double> dist{ DBL_MIN, 716.3554905424517 };
	return dist(gen);
}

int main()
{
	ErrorSearcher searcher{ MakeBounded<double, mpfr_eint>, Overload<double, std::expint> };
	uint64_t _ = searcher.MaxError(Rand);
}