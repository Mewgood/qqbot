#include <nil/memory.hpp>

namespace nil
{
	void write_big_endian(unsigned int input, char * output, std::size_t size)
	{
		if(size)
		{
			for(char * i = output + size - 1; i >= output; --i)
			{
				*i = (char)input;
				input >>= 8;
			}
		}
	}

	void write_little_endian(unsigned int input, char * output, std::size_t size)
	{
		if(size)
		{
			for(char * i = output, * end = output + size; i < end; ++i)
			{
				*i = (char)input;
				input >>= 8;
			}
		}
	}

	unsigned read_big_endian(char const * input, std::size_t size)
	{
		unsigned char const * unsigned_input = reinterpret_cast<unsigned char const *>(input);
		unsigned int output = 0;
		if(size)
		{
			for(unsigned char const * i = unsigned_input, * end = unsigned_input + size; i < end; ++i)
			{
				output <<= 8;
				output |= *i;
			}
		}
		return output;
	}

	unsigned read_little_endian(char const * input, std::size_t size)
	{
		unsigned char const * unsigned_input = reinterpret_cast<unsigned char const *>(input);
		unsigned int output = 0;
		if(size)
		{
			for(unsigned char const * i = unsigned_input + size - 1; i >= unsigned_input; --i)
			{
				output <<= 8;
				output |= (unsigned int)*i;
			}
		}
		return output;
	}

	unsigned get_bit(char const * data, std::size_t offset)
	{
		std::size_t byte = offset / 8;
		std::size_t bit = offset % 8;
		return (data[byte] >> bit) & 1;
	}

	unsigned read_little_endian(char const * data, std::size_t offset, std::size_t count)
	{
		unsigned output = 0;
		for(std::size_t i = 0; i < count; i++)
			output |= get_bit(data, offset + i) << i;
		return output;
	}
}
