#include "../../include/onions/math.h"

namespace onion
{

	Frac::Frac()
	{
		numerator = 0;
		denominator = 1;
	}
	
	Frac::Frac(Int numerator, Int denominator, bool reduce) : numerator(numerator), denominator(denominator)
	{
		// TODO something? if denominator is 0

		if (reduce)
		{
			Int divisor = gcd(numerator, denominator);

			if (divisor > 1)
			{
				numerator /= divisor;
				denominator /= divisor;
			}
		}
	}

	bool Frac::operator<(Int n) const
	{
		if (denominator > 0)
			return n * denominator < numerator; // TODO
		else
			return n * denominator > numerator; // TODO
	}

	bool Frac::operator>(Int n) const
	{
		if (denominator > 0)
			return n * denominator > numerator; // TODO
		else
			return n * denominator < numerator; // TODO
	}

	bool Frac::operator<=(Int n) const
	{
		return !operator>(n);
	}

	bool Frac::operator>=(Int n) const
	{
		return !operator<(n);
	}

	Frac& Frac::operator=(Int n)
	{
		numerator = n;
		denominator = 1;

		return *this;
	}

	Frac& Frac::operator=(const Frac& other)
	{
		numerator = other.numerator;
		denominator = other.denominator;

		return *this;
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

	Frac operator*(const Frac& lhs, const Frac& rhs)
	{
		Int divisors[2] = {
			gcd(lhs.numerator, rhs.denominator),
			gcd(rhs.numerator, lhs.denominator)
		};

		Int numer[2] = { lhs.numerator / divisors[0], rhs.numerator / divisors[1] };
		Int denom[2] = { lhs.denominator / divisors[1], rhs.denominator / divisors[0] };

		divisors[0] = gcd(numer[0], denom[0]);
		divisors[1] = gcd(numer[1], denom[1]);

		return Frac(
			(numer[0] / divisors[0]) * (numer[1] / divisors[1]), 
			(denom[0] / divisors[0]) * (denom[1] / divisors[1])
		);
	}

}