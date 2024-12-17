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
	static std::uniform_real_distribution<double> dist{ 0, 1e-5 };

	ULPHistogram(MakeBounded<double, mpfr_sin>, MakeSerial<double, MySin>, -20.0, 0.0, 0.5);
}