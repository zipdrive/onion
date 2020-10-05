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
	struct type_size
	{
		/// <summary>Retrieves the size of the primitives that compose the type.</summary>
		static constexpr std::size_t primitive = sizeof(T);

		/// <summary>Retrieves the total size of the type.</summary>
		static constexpr std::size_t whole = sizeof(T);
	};

}