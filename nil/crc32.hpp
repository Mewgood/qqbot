#ifndef NIL_CRC32_HPP
#define NIL_CRC32_HPP

#include <cstddef>

namespace nil
{
	class crc32
	{
	public:
		crc32();
		crc32(char const * data, std::size_t size);
		unsigned int hash(char const * data, std::size_t size);
		unsigned int checksum() const;

	private:
		unsigned int sum;
	};
}

#endif
