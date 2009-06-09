#ifndef NIL_ARRAY_HPP
#define NIL_ARRAY_HPP

#include <cstddef>

namespace nil
{
	template <typename type, std::size_t size>
		std::size_t countof(type (&array)[size])
	{
		return size;
	}
}
#endif
