#include "../../include/onions/math.h"

namespace onion
{

	Frac::Frac(Int numerator, Int denominator) : numerator(numerator), denominator(denominator)
	{
		// TODO something? if denominator is 0
	}

	Int operator*(Int lhs, const Frac& rhs)
	{
		if (rhs.numerator == 0)
			return 0;
		else if (rhs.numerator == rhs.denominator)
			return lhs;
		else if (rhs.numerator + rhs.denominator == 0)
			return -lhs;

		if (abs(lhs) > abs(rhs.denominator))
		{
			Int q = lhs / rhs.denominator;
			Int r = lhs - (q * rhs.denominator);

			return (q * rhs.numerator) + (r * rhs);
		}
		else if (abs(rhs.numerator) > abs(rhs.denominator))
		{
			Int q = rhs.numerator / rhs.denominator;
			Int r = rhs.numerator - (q * rhs.denominator);

			return (lhs * q) + (lhs * Frac(r, rhs.denominator));
		}
		else 
		{
			// TODO this is a naive implementation
			long long res = lhs;
			res *= rhs.numerator;
			res /= rhs.denominator;
			return (Int)res;

			/*
			// This is an approximation
			Int q1 = rhs.denominator / rhs.numerator;
			Int r1 = rhs.denominator - (q1 * rhs.numerator);

			Int q2 = lhs / q1;
			Int r2 = lhs - (q2 * q1);

			return q2 - (lhs * Frac(r1, rhs.denominator));
			*/
		}
	}

}