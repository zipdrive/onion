#pragma once
#include <string>

namespace onion
{

	template <typename T>
	struct _primitive
	{
		using type = T;
	};

	template <typename T>
	using primitive = typename _primitive<T>::type;


	template <typename T>
	struct type_limits
	{
		/// <summary>Retrieves the minimum value of the primitive allowed.</summary>
		static constexpr T min()
		{
			return std::numeric_limits<T>::min();
		}

		/// <summary>Retrieves the maximum value of the primitive allowed.</summary>
		static constexpr T max()
		{
			return std::numeric_limits<T>::max();
		}
	};



	template <typename T>
	struct type_size
	{
		// The size of the primitive(s) that compose the type.
		static constexpr std::size_t primitive = sizeof(T);

		// The total size of the type.
		static constexpr std::size_t whole = sizeof(T);
	};

}