#include <iostream>
#include <format>

#include <flttestlib.h>

__m256d MySin(__m256d x)
{
	return x;
}

__m256 MySin(__m256 x)
{
	return x;
}

double ExpMap(double x)
{
	return exp(x);
}

int main()
{
	static std::mt19937_64 gen{ std::random_device{}() };
	SpecialDistribution<double> dist{ 1, 10, { 1, 4, 7 }, false };

	for (;;)
	{
		std::cout << std::format("{}\n", dist(gen));
	}
}