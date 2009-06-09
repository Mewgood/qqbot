#ifndef NIL_MEMORY_HPP
#define NIL_MEMORY_HPP

#include <cstddef>

namespace nil
{
	void write_big_endian(unsigned int input, char * output, std::size_t size);
	void write_little_endian(unsigned int input, char * output, std::size_t size);

	unsigned read_big_endian(char const * input, std::size_t size);
	unsigned read_little_endian(char const * input, std::size_t size);

	unsigned read_little_endian(char const * data, std::size_t offset, std::size_t count);

	unsigned get_bit(char const * data, std::size_t offset);
}

#endif
