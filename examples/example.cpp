#include <iostream>
#include <format>

#include <flttestlib.h>

double Approx(double x)
{
	return sin(x);
}

std::pair<double, double> Exact(double x)
{
	double v = sin(x);
	return { std::nextafter(v, -INFINITY), std::nextafter(v, INFINITY) };
}

int main()
{
	auto [inf, sup] = MakeBounded<double, mpfr_pow>(3.0, 2.1);
	std::cout << std::format("[{}, {}]\n", inf, sup);
}