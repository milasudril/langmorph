#ifndef LANGMORPH_ISQRT_HPP
#define LANGMORPH_ISQRT_HPP

#include <numeric>
#include <concepts>
#include <compare>
#include <cstddef>

namespace langmorph
{
	template<class T>
	struct isqrt_max
	{};

	template<>
	struct isqrt_max<unsigned int>
	{
		static constexpr unsigned int value = 65535;
	};

	template<>
	struct isqrt_max<int>
	{
		static constexpr int value = 46340;
	};

	template<>
	struct isqrt_max<size_t>
	{
		static constexpr size_t value = 0xffffffff;
	};


	template<std::integral T>
	inline auto isqrt(T y)
	{
		T l = 0;
		T r = isqrt_max<T>::value;

		while(l != r)
		{
			auto const m = std::midpoint(r, l);
			auto const cmp = m*m <=> y;
			if(cmp < 0)
			{ l = m; }
			else
			if(cmp == 0)
			{ return m; }
			else
			{r = m - 1;}
		}

		return l;
	}
}

#endif