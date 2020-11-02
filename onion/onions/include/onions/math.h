#pragma once
#include "primitive.h"

namespace onion
{

	using Int = primitive<int>;
	using Uint = primitive<unsigned int>;
	using Float = primitive<float>;
	using Double = primitive<double>;

	template <typename T, typename U>
	struct _primitive_multiplication
	{
		using type = T;
	};

	template <typename T>
	struct _primitive_multiplication<T, Float>
	{
		using type = Float;
	};

	template <>
	struct _primitive_multiplication<Uint, Int>
	{
		using type = Int;
	};

	template <typename T, typename U>
	using primitive_multiplication = typename _primitive_multiplication<T, U>::type;



	struct Frac
	{
		// The numerator of the fraction.
		Int numerator;

		// The denominator of the fraction.
		Int denominator;

		/// <summary>Constructs a fraction.</summary>
		/// <param name="numerator">The numerator of the fraction.</param>
		/// <param name="denominator">The denominator of the fraction.</param>
		Frac(Int numerator, Int denominator);
	};

	Int operator*(Int lhs, const Frac& rhs);



	/// <summary>Implements the binary GCD algorithm to find the greatest common divisor of two nonnegative integers.</summary>
	/// <param name="a">The first integer. Should be nonnegative.</param>
	/// <param name="b">The second integer. Should be nonnegative.</param>
	/// <returns>The greatest common divisor of both integers.</returns>
	template <typename T>
	T gcd(T a, T b)
	{
		if (a < b)
			return gcd(b, a);

		else if (b == 0)
			return a;

		else if (a % b == 0)
			return b;

		else if (a % 2 == 0 && b % 2 == 0)
			return 2 * gcd(a / 2, b / 2);
		
		else if (a % 2 != b % 2)
			return a % 2 == 0 ? gcd(a / 2, b) : gcd(a, b / 2);
		
		else
			return gcd(a - b, b);
	}

}