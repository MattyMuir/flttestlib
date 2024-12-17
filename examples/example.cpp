#include <iostream>
#include <format>

#include <flttestlib.h>

int main()
{
	//1.0996832909591162e-28   xn: 1.099683290959118e-28
	double x	= 1.0996832909591162e-28;
	double xn	= 1.099683290959118e-28;

	auto dist = ULPDistance(x, xn);
}