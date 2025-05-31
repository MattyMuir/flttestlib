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
	std::uniform_real_distribution<double> dist{ 0, 1e-5 };
	return dist(gen);
}

int main()
{
	ErrorSearcher searcher{ MakeBounded<double, mpfr_sin>, Approx };
	uint64_t _ = searcher.MaxError(Rand);
}