#include <nil/random.hpp>

#include <algorithm>
#include <vector>

#include <nil/array.hpp>
#include <nil/windows.hpp>
#include <nil/time.hpp>

namespace nil
{
	namespace random
	{
		namespace
		{
			ulong advanced_status[32];
			std::size_t advanced_offset;
			ulong status = static_cast<ulong>(time());

			bool const obfuscation1[] =
			{
				false,
				true,
				false,
				false,
				true,
				false,
				false,
				false,
				true,
				true,
				false
			};

			ulong const obfuscation2[] =
			{
				17,
				2,
				3,
				11,
				5,
				13,
				19,
				17,
				23,
				7,
				29
			};
		}

		ulong time_passed()
		{
#ifdef NIL_WINDOWS
			return static_cast<ulong>(::GetTickCount());
#else
			//who cares
			return 0;
#endif
		}

		ulong rotate_right(ulong number, ulong offset)
		{
			return (number << static_cast<ulong>(sizeof(ulong) - offset)) | (number >> offset);
		}

		ulong rotate_left(ulong number, ulong offset)
		{
			return (number >> static_cast<ulong>(sizeof(ulong) - offset)) | (number << offset);
		}

		struct rapist
		{
			bool is_right;
			ulong offset;

			rapist(bool is_right, ulong offset):
				is_right(is_right),
				offset(offset)
			{
			}

			void apply(ulong & number)
			{
				if(is_right)
					number ^= rotate_right(number, offset);
				else
					number ^= rotate_left(number, offset);
			}
		};

		ulong get_byte(std::string const & data, std::size_t offset)
		{
			return static_cast<ulong>(static_cast<unsigned char>(data[offset]));
		}

		void seed(std::string const & data)
		{
			advanced_offset = 0;
			
			if(data.size() < nil::countof(advanced_status) * sizeof(ulong))
				throw nil::exception("Seed input too small");
			bool wrap_around = false;
			ulong repetition = 1020379;
			for(std::size_t i = 0, end = data.size(), table_offset = 0; i < end; i += sizeof(ulong), table_offset++)
			{
				ulong value = 0;
				for(std::size_t j = 0; j < sizeof(ulong); j++)
					value |= get_byte(data, i + j) << (j * 8);

				for(std::size_t j = 0; j < nil::countof(obfuscation1); j++)
				{
					rapist fun(obfuscation1[j], obfuscation2[j]);
					fun.apply(value);
				}

				value ^= time_passed();

				if(table_offset == nil::countof(advanced_status))
				{
					table_offset = 0;
					wrap_around = true;
				}

				value ^= repetition;
				repetition = rotate_left(repetition, value & 0x1f);
				repetition += 611953;

				ulong & table_entry = advanced_status[table_offset];
				if(wrap_around)
					table_entry ^= value;
				else
					table_entry = value;
			}
		}

		ulong advanced_uint()
		{
			std::size_t size = nil::countof(advanced_status);
			for(std::size_t i = 0; i < size; i++)
			{
				ulong & value = advanced_status[i];
				value ^= time_passed();
				if(i != advanced_offset)
					value ^= advanced_status[advanced_offset];
			}

			ulong maximum = static_cast<ulong>(size) - 1;
			for(ulong i = 0; i < 64; i++)
			{
				ulong offset1 = uint(0, maximum);
				ulong offset2 = uint(0, maximum);
				ulong & value1 = advanced_status[offset1];
				ulong & value2 = advanced_status[offset2];
				for(std::size_t j = 0; j < nil::countof(obfuscation1); j++)
				{
					rapist fun(obfuscation1[j], obfuscation2[j]);
					fun.apply(value1);
				}
				value1 ^= value2;
				std::swap(value1, value2);
			}

			advanced_offset++;
			if(advanced_offset == size)
				advanced_offset = 0;

			ulong output = 0;
			for(std::size_t i = 0; i < nil::countof(advanced_status); i++)
			{
				ulong bit = (advanced_status[i] >> uint(0, static_cast<ulong>(sizeof(ulong) - 1))) & 1; 
				output <<= 1;
				output |= bit;
			}

			return output;
		}

		ulong advanced_uint(ulong minimum, ulong maximum)
		{
			if(minimum > maximum)
				std::swap(minimum, maximum);
			ulong range = maximum - minimum + 1;
			return (advanced_uint() % range) + minimum;
		}

		ulong uint()
		{
			status ^= status << 11;
			status ^= status >> 7;
			status ^= status >> 15;
			status ^= status << 18;
			return status;
		}

		ulong uint(ulong minimum, ulong maximum)
		{
			if(minimum > maximum)
				std::swap(minimum, maximum);
			ulong range = maximum - minimum + 1;
			return (uint() % range) + minimum;
		}

		template <typename type>
			type real()
		{
			return uint() / static_cast<type>(~0u);
		}

		template <typename type>
			type real(type minimum, type maximum)
		{
			if(minimum > maximum)
				std::swap(minimum, maximum);
			type range = maximum - minimum;
			return real<type>() * range + minimum;
		}

		float sp_float()
		{
			return real<float>();
		}

		float sp_float(float minimum, float maximum)
		{
			return real<float>(minimum, maximum);
		}

		double dp_float()
		{
			return real<double>();
		}

		double dp_float(double minimum, double maximum)
		{
			return real<double>(minimum, maximum);
		}
	}
}
