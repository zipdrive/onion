#pragma once
#include "primitive.h"

namespace onion
{

	using Int = primitive<int>;
	using Uint = primitive<unsigned int>;
	using Float = primitive<float>;
	using Double = primitive<double>;


	/*
	NOTE: type_limits for Int and Uint are so low to ensure that the dot product of any two vec3i (within the passed limits) will not cause an integer overflow
	*/

	template <>
	struct type_limits<Int>
	{
		static constexpr Int min()
		{
			return std::numeric_limits<Int>::min() / (1 << ((sizeof(Int) * 4) + 1));
		}

		static constexpr Int max()
		{
			return std::numeric_limits<Int>::max() / (1 << ((sizeof(Int) * 4) + 1));
		}
	};

	template <>
	struct type_limits<Uint>
	{
		static constexpr Uint min()
		{
			return 0;
		}

		static constexpr Uint max()
		{
			return std::numeric_limits<Uint>::max() / (1 << ((sizeof(Uint) * 4) + 1));
		}
	};



	struct Frac
	{
		// The numerator of the fraction.
		Int numerator;

		// The denominator of the fraction.
		Int denominator;

		/// <summary>Constructs a fraction with 0 in the numerator and 1 in the denominator.</summary>
		Frac();

		/// <summary>Constructs a fraction.</summary>
		/// <param name="numerator">The numerator of the fraction.</param>
		/// <param name="denominator">The denominator of the fraction.</param>
		/// <param name="reduce">True if the fraction should be reduced, false otherwise.</param>
		Frac(Int numerator, Int denominator, bool reduce = false);

		/// <summary>Determines if the fraction is less than the given integer.</summary>
		/// <param name="n">The integer to compare the fraction with.</param>
		/// <returns>True if the fraction is less than the given integer, false otherwise.</returns>
		bool operator<(Int n) const;

		/// <summary>Determines if the fraction is greater than the given integer.</summary>
		/// <param name="n">The integer to compare the fraction with.</param>
		/// <returns>True if the fraction is less than the given integer, false otherwise.</returns>
		bool operator>(Int n) const;

		/// <summary>Determines if the fraction is less than or equal to the given integer.</summary>
		/// <param name="n">The integer to compare the fraction with.</param>
		/// <returns>True if the fraction is less than the given integer, false otherwise.</returns>
		bool operator<=(Int n) const;

		/// <summary>Determines if the fraction is greater than or equal to the given integer.</summary>
		/// <param name="n">The integer to compare the fraction with.</param>
		/// <returns>True if the fraction is less than the given integer, false otherwise.</returns>
		bool operator>=(Int n) const;



		Frac& operator=(Int n);

		Frac& operator=(const Frac& other);
	};

	Int operator*(Int lhs, const Frac& rhs);

	Frac operator*(const Frac& lhs, const Frac& rhs);

	


	template <typename T, typename U>
	struct _primitive_arithmetic
	{
		using type = T;
	};

	template <typename T>
	struct _primitive_arithmetic<T, Float>
	{
		using type = Float;
	};

	template <>
	struct _primitive_arithmetic<Uint, Int>
	{
		using type = Int;
	};

	template <>
	struct _primitive_arithmetic<Int, Frac>
	{
		using type = Int;
	};

	template <typename T, typename U>
	using primitive_arithmetic = typename _primitive_arithmetic<T, U>::type;



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