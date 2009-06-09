#include <nil/crc32.hpp>

namespace nil
{
	namespace
	{
		unsigned int crc32_table[256];

		struct initialise_crc32_table
		{
			initialise_crc32_table()
			{
				char const polynomial_terms[] =
				{
					 0,  1,  2,  4,  5,  7,  8,
					10, 11, 12, 16, 22,	23, 26
				};

				unsigned int polynomial_pattern = 0;
				for(std::size_t i = 0; i < sizeof(polynomial_terms); ++i)
				{
					unsigned int shifts = 31 - polynomial_terms[i];
					polynomial_pattern |= 1 << shifts;
				}

				for(std::size_t i = 0; i < sizeof(crc32_table) / sizeof(int); ++i)
				{
					unsigned int new_table_entry = static_cast<unsigned int>(i);
					for(int j = 0; j < 8; ++j)
					{
						bool first_bit_was_set = (new_table_entry & 1);
						new_table_entry >>= 1;
						if(first_bit_was_set == true)
						{
							new_table_entry ^= polynomial_pattern;
						}
						crc32_table[i] = new_table_entry;
					}
				}
			}
		}
			crc32_initialiser;
	}

	crc32::crc32():
		sum(0)
	{
	}

	crc32::crc32(char const * data, std::size_t size):
		sum(0)
	{
	}

	unsigned int crc32::hash(char const * data, std::size_t size)
	{
		sum ^= 0xffffffff;
		while(size > 0)
		{
			unsigned int index = (*data ^ sum) & 0xff;
			unsigned int shifted_sum = sum >> 8;
			sum = crc32_table[index] ^ shifted_sum;
			--size;
			++data;
		}
		sum ^= 0xffffffff;
		return sum;
	}

	unsigned int crc32::checksum() const
	{
		return sum;
	}
}
